#include "scene_cache.hpp"

#include "loader.hpp"
#include "mods/svc/log.hpp"
#include "render.hpp"

#include <chrono>
#include <gx/GXAurora.h>
#include <mutex>
#include <unordered_map>

namespace slugcat::vrm::scene_cache {

namespace {

// I tried to make the loading cool and multithreaded but instead I was hit with
// "Dawn isn't thread safe"
// So RIP to that.

std::unordered_map<std::string, std::expected<std::shared_ptr<scene::Scene>, std::monostate>>
    sScenes;
std::mutex sScenesMutex;

} // namespace

std::expected<std::shared_ptr<scene::Scene>, std::monostate> load(std::string const& path) {
    std::unique_lock lock(sScenesMutex);

    auto const found = sScenes.find(path);
    if (found != sScenes.end()) {
        return found->second;
    }

    AuroraGXSync();

    try {
        auto const then = std::chrono::steady_clock::now();
        mods::log::debug("Loading '{}'...", path);
        auto load_result = loader::loadScene(path.c_str());
        auto const now = std::chrono::steady_clock::now();

        render::sQueue.Submit(1, &std::get<1>(load_result));
        sScenes[path] = std::move(std::get<0>(load_result));

        const std::chrono::duration<double, std::milli> fp_ms = now - then;
        mods::log::info("Loaded '{}' in {}ms!", path, fp_ms.count());
    } catch (std::exception const& e) {
        mods::log::error("Failed to load '{}': {}", path, e.what());
        sScenes[path] = std::unexpected(std::monostate{});
    }

    return sScenes[path];
}

} // namespace slugcat::vrm::scene_cache
