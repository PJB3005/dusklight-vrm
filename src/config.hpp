#pragma once

#include <mods/svc/config.hpp>

namespace slugcat::vrm::config {

extern mods::config::Handle<std::string> cVarVrmPathHandle;
extern mods::config::Handle<int64_t> cVarVrmScale;
extern mods::config::Handle<int64_t> cVarVrmBrightness;
extern mods::config::Handle<bool> cVarRenderLink;

constexpr int64_t kPercentValueBase = 100;

void init();
void shutdown();

}
