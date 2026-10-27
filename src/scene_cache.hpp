#pragma once

#include "scene.hpp"

#include <variant>
#include <expected>
#include <memory>
#include <string>

namespace slugcat::vrm::scene_cache {

std::expected<std::shared_ptr<scene::Scene>, std::monostate> load(std::string const& path);

}
