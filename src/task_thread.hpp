#pragma once

namespace slugcat::vrm::task_thread {

struct TaskRequest {
    virtual ~TaskRequest() = default;
    virtual void execute() = 0;
};

void run_task(std::unique_ptr<TaskRequest> request);

void init();
void shutdown();

}
