#pragma once

#include <queue>
#include <mutex>
#include "process_info.h"

class Process_fifo {
public:
    bool empty();
    void push(const Process_info& process);
    Process_info pop();
    Process_info head();

private:
    std::queue<Process_info> fifo;
    std::mutex mtx_fifo;
};
