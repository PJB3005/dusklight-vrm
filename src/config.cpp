#include "config.hpp"

#include "helpers/result.hpp"

namespace slugcat::vrm::config {

using helpers::checkResult;

namespace {

using namespace mods::config;
using namespace std::string_literals;

}  // namespace

Handle<std::string> cVarVrmPathHandle;
Handle<int64_t> cVarVrmScale;
Handle<int64_t> cVarVrmBrightness;
Handle<bool> cVarRenderLink;

void init() {
    cVarVrmPathHandle = register_var("vrm_path", ""s).value();
    cVarVrmScale = register_var("vrm_scale", kPercentValueBase).value();
    cVarVrmBrightness = register_var("vrm_brightness", kPercentValueBase).value();
    cVarRenderLink = register_var("render_link", false).value();
}

void shutdown() {
    cVarVrmPathHandle = {};
    cVarVrmScale = {};
    cVarVrmBrightness = {};
    cVarRenderLink = {};
}

}  // namespace slugcat::vrm::config
