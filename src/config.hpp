#pragma once
#include "mods/svc/config.h"

namespace slugcat::vrm::config {

extern ConfigVarHandle cVarVrmPathHandle;
extern ConfigVarHandle cVarVrmScale;
extern ConfigVarHandle cVarVrmBrightness;
extern ConfigVarHandle cVarRenderLinkHandle;

constexpr int64_t kPercentValueBase = 100;

void init();

}