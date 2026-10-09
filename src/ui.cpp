#include "ui.hpp"
#include "mods/svc/ui.h"

#include "config.hpp"
#include "helpers/result.hpp"

namespace slugcat::vrm::ui {

namespace {

UiElementHandle elemPath = 0;
UiElementHandle elemScale = 0;

ModResult build(ModContext*, UiElementHandle panel, void*, ModError*) {
    svc_ui->pane_add_section(mod_ctx, panel, "Settings");

    UiControlDesc control1 = UI_CONTROL_DESC_INIT;
    control1.kind = UI_CONTROL_FILE_PICKER;
    control1.label = "Path";
    control1.help_rml = "Path to the VRM file";
    control1.binding = UI_BINDING_CONFIG_VAR;
    control1.config_var = config::cVarVrmPathHandle;
    svc_ui->pane_add_control(mod_ctx, panel, &control1, &elemPath);

    UiControlDesc control2 = UI_CONTROL_DESC_INIT;
    control2.kind = UI_CONTROL_NUMBER;
    control2.label = "VRM Scale";
    control2.help_rml = "Scale of the VRM in the world";
    control2.binding = UI_BINDING_CONFIG_VAR;
    control2.config_var = config::cVarVrmScale;
    control2.min = 1;
    control2.max = 1000;
    control2.step = 1;
    svc_ui->pane_add_control(mod_ctx, panel, &control2, &elemScale);

    UiControlDesc control3 = UI_CONTROL_DESC_INIT;
    control3.kind = UI_CONTROL_NUMBER;
    control3.label = "Brightness";
    control3.help_rml = "Brightness of the VRM";
    control3.binding = UI_BINDING_CONFIG_VAR;
    control3.config_var = config::cVarVrmBrightness;
    control3.min = 1;
    control3.max = 200;
    control3.step = 1;
    svc_ui->pane_add_control(mod_ctx, panel, &control3, &elemScale);

    svc_ui->pane_add_section(mod_ctx, panel, "Debug");

    UiControlDesc control4 = UI_CONTROL_DESC_INIT;
    control4.kind = UI_CONTROL_TOGGLE;
    control4.label = "Draw Link";
    control4.help_rml = "Draw Link for debugging purposes";
    control4.binding = UI_BINDING_CONFIG_VAR;
    control4.config_var = config::cVarRenderLink;
    svc_ui->pane_add_control(mod_ctx, panel, &control4, &elemScale);

    return MOD_OK;
}

}  // namespace

void init() {
    UiModsPanelDesc panel = UI_MODS_PANEL_DESC_INIT;
    panel.build = build;
    helpers::checkResult(svc_ui->register_mods_panel(mod_ctx, &panel));
}

}  // namespace slugcat::vrm::ui
