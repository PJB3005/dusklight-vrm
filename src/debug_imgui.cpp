#include <numbers>

#include "debug_imgui.hpp"
#if ENABLE_IMGUI
#include "helpers/math.hpp"
#include "helpers/result.hpp"
#include "imgui.h"
#include "mod.hpp"
#include "mods/svc/imgui.h"
#include "scene.hpp"

IMPORT_SERVICE(ImguiService, svc_imgui);

namespace slugcat::vrm::debug_imgui {

using namespace slugcat::vrm::scene;

namespace {

void show_entity(Scene& scene, EntityId idx) {
    ImGui::PushID(idx);

    auto const& ent = scene.get_entity(idx);

    if (ImGui::SmallButton(ent.name.c_str())) {
        scene.viewing = idx;
    }

    ImGui::Indent(4);

    for (auto const child : ent.children) {
        show_entity(scene, child);
    }

    ImGui::Unindent(4);

    ImGui::PopID();
}

void show_scene(Scene& scene) {
    if (ImGui::BeginChild("##tree", ImVec2(300, 0),
            ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened))
    {
        show_entity(scene, scene.root);
    }

    ImGui::EndChild();
    ImGui::SameLine();

    ImGui::BeginGroup();

    auto& selected = scene.get_entity(scene.viewing);

    ImGui::Text("Selected: %s", selected.name.c_str());

    glm::vec3 rotEuler = glm::eulerAngles(selected.rotation) * helpers::Rad2Deg;

    auto const changedTrans = ImGui::InputFloat3("Translation", &selected.translation.x);
    auto const changedRot = ImGui::InputFloat3("Rotation", &rotEuler.x);
    ImGui::InputFloat4("Quat", &selected.rotation.x);
    auto const changedScale = ImGui::InputFloat3("Scale", &selected.scale.x);

    auto [decompTrans, decompRot, decompScale] = matrix::decompose(selected.globalXform);
    auto decompRotEuler = glm::eulerAngles(decompRot) * helpers::Rad2Deg;

    ImGui::BeginDisabled();

    ImGui::InputFloat3("Global Translation", &decompTrans.x);
    ImGui::InputFloat3("Global Rotation", &decompRotEuler.x);
    ImGui::InputFloat4("Global Quat", &decompRot.x);
    ImGui::InputFloat3("Global Scale", &decompScale.x);

    ImGui::EndDisabled();

    if (changedRot) {
        selected.rotation = glm::quat(rotEuler * helpers::Deg2Rad);
    }

    if (selected.mesh) {
        auto const& mesh = selected.mesh;
        int id = 0;
        for (auto const& primitive : mesh->primitives) {
            auto& mat = *primitive.material;
            ImGui::PushID(id);
            ImGui::Text("Material: %s", mat.name.c_str());
            ImGui::ColorEdit4("color", &mat.color.r);

            ImGui::PopID();

            id += 1;
        }
    }

    ImGui::EndGroup();
}

bool active;

void on_imgui_frame(ModContext*, void*) {
    if (!active) {
        return;
    }

    int idx = 0;
    for (auto const actor : gAllActors) {
        auto& scene = *actor->scene;
        ImGui::PushID(idx);
        if (ImGui::Begin("Real")) {
            show_scene(scene);
        }

        ImGui::End();
        ImGui::PopID();

        idx += 1;
    }
}

void on_imgui_menu(ModContext*, void*) {
    if (ImGui::BeginMenu("VRM")) {
        ImGui::MenuItem("Show scene", nullptr, &active);

        ImGui::EndMenu();
    }
}

}

void init() {
    helpers::checkResult(svc_imgui->set_callback(mod_ctx, IMGUI_CALLBACK_FRAME, on_imgui_frame, nullptr));
    helpers::checkResult(svc_imgui->set_callback(mod_ctx, IMGUI_CALLBACK_MENU_BAR, on_imgui_menu, nullptr));
}

}  // namespace slugcat::vrm::debug_imgui

#else

namespace slugcat::vrm::debug_imgui {
void init() {
    // Nada.
}
} // namespace slugcat::vrm::debug_imgui

#endif
