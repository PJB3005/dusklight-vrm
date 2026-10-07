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
    .default_int = kScaleBase,
};

constexpr ConfigVarDesc cVarRenderLinkDesc{
    .struct_size = sizeof(cVarRenderLinkDesc),
    .name = "render_link",
    .type = CONFIG_VAR_BOOL,
    .default_bool = false,
};

}  // namespace

ConfigVarHandle cVarPathHandle;
ConfigVarHandle cVarVrmScaleHandle;
ConfigVarHandle cVarRenderLinkHandle;

void init() {
    checkResult(svc_config->register_var(mod_ctx, &cVarVrmPathDesc, &cVarPathHandle));
    checkResult(svc_config->register_var(mod_ctx, &cVarVrmScaleDesc, &cVarVrmScaleHandle));
    checkResult(svc_config->register_var(mod_ctx, &cVarRenderLinkDesc, &cVarRenderLinkHandle));
}

}  // namespace slugcat::vrm::config