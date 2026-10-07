#pragma once

#include "JSystem/J3DGraphBase/J3DPacket.h"
#include "glm/glm.hpp"
#include "mods/svc/gfx.h"
#include "mods/svc/interp.hpp"
#include "scene.hpp"

#include "webgpu/webgpu_cpp.h"

namespace slugcat::vrm::render {

extern wgpu::Device sDevice;
extern wgpu::Queue sQueue;
extern GfxDeviceInfo sDeviceInfo;
extern wgpu::Limits sLimits;

extern wgpu::BindGroupLayout sBindGroupLayoutGlobal;
extern wgpu::BindGroupLayout sBindGroupLayoutMaterial;
extern wgpu::BindGroupLayout sBindGroupLayoutObject;
extern wgpu::PipelineLayout sPipelineLayout;

void init();

struct UniformGlobal {
    glm::mat4 projViewMtx;
};

struct UniformMaterial {
    glm::vec4 color;
};

struct UniformObject {
    glm::mat4 modelMtx;
};

constexpr u32 GXMaxLights = 8;

struct GXLight {
    glm::vec3 position;
    float _pad0;
    glm::vec3 direction;
    float _pad1;
    glm::vec4 color;
    glm::vec3 cos_att;
    float _pad2;
    glm::vec3 dist_att;
    float _pad3;
};

struct GXLightingData {
    glm::vec4 ambientColor;
    GXLight lights[GXMaxLights];
    u32 activeLights;
    u32 _pad[3];
};

struct UniformGXShading {
    GXLightingData lighting;

    glm::vec4 kColor0;
    glm::vec4 color1;
};

class FoobarPacket final : public J3DPacket {
public:
    wgpu::RenderPipeline pipeline;
    std::shared_ptr<scene::Scene> scene;
    std::vector<mods::interp::InterpMatrix> entityMatrices;

    UniformGXShading shading{};

    FoobarPacket();
    void draw() override;

private:
    [[nodiscard]] glm::mat4 readEntityMatrix(scene::EntityId id) const;
};

}
