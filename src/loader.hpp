#pragma once

#include <memory>

#include "scene.hpp"

namespace slugcat::vrm::loader {

std::tuple<std::shared_ptr<scene::Scene>, wgpu::CommandBuffer> loadScene(char const* path);
void applyLoad(wgpu::CommandEncoder& encoder, wgpu::CommandBuffer const& state);

}
