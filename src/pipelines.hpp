#pragma once

#include <webgpu/webgpu_cpp.h>

#include <optional>

namespace slugcat::vrm::pipelines {

struct PipelineDesc {
    wgpu::TextureFormat colorFormat{};
    wgpu::TextureFormat depthFormat{};
    wgpu::TextureFormat normalFormat{};

    bool skinned{};
    bool normals{};
    bool gxShading{};
    bool _pad{};
};

static_assert(std::has_unique_object_representations_v<PipelineDesc>);

void init();
void shutdown();
std::optional<wgpu::RenderPipeline> try_get_pipeline(PipelineDesc const &desc);

} // namespace slugcat::vrm::pipelines
