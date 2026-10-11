#include "pipelines.hpp"

#define XXH_STATIC_LINKING_ONLY
#include "helpers/channel.hpp"
#include "helpers/thread.hpp"
#include "render.hpp"
#include "shader.hpp"

#include <thread>
#include <mutex>
#include <xxhash.h>

#include <unordered_map>

namespace slugcat::vrm::pipelines {

namespace {

using namespace std::string_literals;
using namespace std::string_view_literals;

struct PipelineRequest {
    XXH64_hash_t hash;
    PipelineDesc desc;
};

std::atomic_bool sPipelineThreadShutdown;
std::thread sPipelineCompilationThread;
helpers::Channel<PipelineRequest> sPipelineChannel;
std::unordered_map<XXH64_hash_t, std::optional<wgpu::RenderPipeline>> sPipelines;
std::mutex sPipelinesMutex;

constexpr wgpu::VertexAttribute vtxAttrPosition[]{{
    .format = wgpu::VertexFormat::Float32x3,
    .shaderLocation = 0,
}};
constexpr wgpu::VertexAttribute vtxAttrTexCoord[]{{
    .format = wgpu::VertexFormat::Float32x2,
    .shaderLocation = 1,
}};
constexpr wgpu::VertexAttribute vtxAttrNormal[]{{
    .format = wgpu::VertexFormat::Float32x3,
    .shaderLocation = 2,
}};
constexpr wgpu::VertexAttribute vtxAttrSkinJoints[]{{
    .format = wgpu::VertexFormat::Uint16x4,
    .shaderLocation = 6,
}};
constexpr wgpu::VertexAttribute vtxAttrSkinWeights[]{{
    .format = wgpu::VertexFormat::Float32x4,
    .shaderLocation = 7,
}};

wgpu::ShaderModule getShaderForPipeline(PipelineDesc const& desc) {
    std::vector<std::pair<std::string, bool>> shaderFeatures;
    if (desc.normals) {
        shaderFeatures.emplace_back("NORMALS"s, true);
    }
    if (desc.gxShading) {
        shaderFeatures.emplace_back("GX_SHADING"s, true);
    }
    if (desc.skinned) {
        shaderFeatures.emplace_back("SKINNED"s, true);
    }
    if (desc.normalFormat != wgpu::TextureFormat::Undefined) {
        shaderFeatures.emplace_back("NORMAL_BUFFER"s, true);
    }

    return shader::compileShader("shaders::model", shaderFeatures);
}

template <size_t N>
wgpu::VertexBufferLayout attrBuffer(uint32_t stride, wgpu::VertexAttribute const (&attrs)[N]) {
    return {
        .stepMode = wgpu::VertexStepMode::Vertex,
        .arrayStride = stride,
        .attributeCount = std::size(attrs),
        .attributes = attrs,
    };
}

void compileSinglePipeline(PipelineRequest const& request) {
    auto const& desc = request.desc;
    auto const shaderModule = getShaderForPipeline(desc);

    std::vector<wgpu::VertexBufferLayout> vtxBuffers;
    vtxBuffers.resize(8, {});
    vtxBuffers[render::VTX_BUF_POSITION] = attrBuffer(12, vtxAttrPosition);
    vtxBuffers[render::VTX_BUF_TEXCOORD] = attrBuffer(8, vtxAttrTexCoord);
    vtxBuffers[render::VTX_BUF_NORMAL] = attrBuffer(12, vtxAttrNormal);
    vtxBuffers[render::VTX_BUF_SKIN_JOINTS] = attrBuffer(8, vtxAttrSkinJoints);
    vtxBuffers[render::VTX_BUF_SKIN_WEIGHTS] = attrBuffer(16, vtxAttrSkinWeights);

    wgpu::DepthStencilState depthStencil{
        .format = desc.depthFormat,
        .depthWriteEnabled = WGPUOptionalBool_True,
        .depthCompare = wgpu::CompareFunction::Greater,
    };

    static constexpr wgpu::BlendState blend{
        .color = {.operation = wgpu::BlendOperation::Add,
                  .srcFactor = wgpu::BlendFactor::SrcAlpha,
                  .dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha},
        .alpha = {.operation = wgpu::BlendOperation::Add,
                  .srcFactor = wgpu::BlendFactor::One,
                  .dstFactor = wgpu::BlendFactor::One},
    };

    std::vector<wgpu::ColorTargetState> targets;
    targets.push_back({
        .format = desc.colorFormat,
        .blend = &blend,
        .writeMask = wgpu::ColorWriteMask::All,
    });
    if (desc.normalFormat != wgpu::TextureFormat::Undefined) {
        targets.push_back({
            .format = desc.normalFormat,
            .writeMask = wgpu::ColorWriteMask::All,
        });
    }

    wgpu::FragmentState const fragmentState{
        .module = shaderModule,
        .entryPoint = "fs_main"sv,
        .targetCount = targets.size(),
        .targets = targets.data(),
    };

    wgpu::RenderPipelineDescriptor const finalDesc{
        .label = "VRM pipeline"sv,
        .layout = render::sPipelineLayout,
        .vertex =
            {
                .module = shaderModule,
                .entryPoint = "vs_main"sv,
                .bufferCount = vtxBuffers.size(),
                .buffers = vtxBuffers.data(),
            },
        .primitive =
            {
                .topology = wgpu::PrimitiveTopology::TriangleList,
                .frontFace = wgpu::FrontFace::CCW,
                .cullMode = wgpu::CullMode::Back,
            },
        .depthStencil = &depthStencil,
        .multisample =
            {
                .count = 1,
                .mask = 0xFFFF'FFFF,
            },
        .fragment = &fragmentState,
    };

    wgpu::RenderPipeline pipeline = render::sDevice.CreateRenderPipeline(&finalDesc);

    std::lock_guard lock(sPipelinesMutex);
    sPipelines[request.hash] = std::move(pipeline);
}

void pipelineThreadMain() {
    helpers::setThreadName("VRM pipeline compilation thread");
    helpers::setThreadLowPriority();

    while (true) {
        auto const req = sPipelineChannel.read();
        if (sPipelineThreadShutdown) {
            break;
        }

        compileSinglePipeline(req);
    }
}

} // namespace

std::optional<wgpu::RenderPipeline> try_get_pipeline(PipelineDesc const& desc) {
    std::unique_lock lock(sPipelinesMutex);

    auto const hash = XXH3_64bits(&desc, sizeof(desc));
    auto const found = sPipelines.find(hash);
    if (found != sPipelines.end()) {
        return found->second;
    }

    sPipelines[hash] = std::nullopt;
    sPipelineChannel.write({hash, desc});

    return std::nullopt;
}

void init() {
    sPipelineCompilationThread = std::thread(pipelineThreadMain);
}

void shutdown() {
    sPipelineThreadShutdown = true;
    sPipelineChannel.write(PipelineRequest{});
    sPipelineCompilationThread.join();
}

} // namespace slugcat::vrm::pipelines
