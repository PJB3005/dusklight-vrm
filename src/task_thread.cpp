#include "task_thread.hpp"

#include "helpers/channel.hpp"
#include "helpers/thread.hpp"

#include <thread>

namespace slugcat::vrm::task_thread {

namespace {

std::atomic_bool sTaskThreadShutdown;
std::thread sTaskThread;
helpers::Channel<std::unique_ptr<TaskRequest>> sTaskChannel;

void pipelineThreadMain() {
    helpers::setThreadName("systems.slugcat.vrm background thread");
    helpers::setThreadLowPriority();

    while (true) {
        auto const req = sTaskChannel.read();
        if (sTaskThreadShutdown) {
            break;
        }

        req->execute();
    }
}

} // namespace

void run_task(std::unique_ptr<TaskRequest> request) {
    sTaskChannel.write(std::move(request));
}

void init() {
    sTaskThread = std::thread(pipelineThreadMain);
}

void shutdown() {
    sTaskThreadShutdown = true;
    sTaskChannel.write(nullptr);
    sTaskThread.join();
}

}
