#include "config.hpp"

#include "helpers/result.hpp"

namespace slugcat::vrm::config {

using helpers::checkResult;

namespace {

constexpr ConfigVarDesc cVarVrmPathDesc{
    .struct_size = sizeof(cVarVrmPathDesc),
    .name = "vrm_path",
    .type = CONFIG_VAR_STRING,
};

constexpr ConfigVarDesc cVarVrmScaleDesc{
    .struct_size = sizeof(cVarVrmScaleDesc),
    .name = "vrm_scale",
    .type = CONFIG_VAR_INT,
    .default_int = kPercentValueBase,
};

constexpr ConfigVarDesc cVarVrmBrightnessDesc{
    .struct_size = sizeof(cVarVrmBrightnessDesc),
    .name = "vrm_brightness",
    .type = CONFIG_VAR_INT,
    .default_int = kPercentValueBase,
};

constexpr ConfigVarDesc cVarRenderLinkDesc{
    .struct_size = sizeof(cVarRenderLinkDesc),
    .name = "render_link",
    .type = CONFIG_VAR_BOOL,
    .default_bool = false,
};

}  // namespace

ConfigVarHandle cVarVrmPathHandle;
ConfigVarHandle cVarVrmScale;
ConfigVarHandle cVarVrmBrightness;
ConfigVarHandle cVarRenderLink;

void init() {
    checkResult(svc_config->register_var(mod_ctx, &cVarVrmPathDesc, &cVarVrmPathHandle));
    checkResult(svc_config->register_var(mod_ctx, &cVarVrmScaleDesc, &cVarVrmScale));
    checkResult(svc_config->register_var(mod_ctx, &cVarVrmBrightnessDesc, &cVarVrmBrightness));
    checkResult(svc_config->register_var(mod_ctx, &cVarRenderLinkDesc, &cVarRenderLink));
}

}  // namespace slugcat::vrm::config