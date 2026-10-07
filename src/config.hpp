#pragma once
#include "mods/svc/config.h"

namespace slugcat::vrm::config {

extern ConfigVarHandle cVarPathHandle;
extern ConfigVarHandle cVarVrmScaleHandle;
extern ConfigVarHandle cVarRenderLinkHandle;

constexpr int64_t kScaleBase = 100;

void init();

}