#pragma once

#include <queue>
#include <mutex>
#include "process_info.h"
#include <iostream>

class Process_fifo {
    friend std::ostream& operator<<(std::ostream& out, const Process_fifo& process_fifo);

    public:
        bool empty();
        void push(const Process_info& process);
        Process_info pop();
        Process_info head();

    private:
        std::queue<Process_info> fifo;
        std::mutex mtx_fifo;
};

std::ostream& operator<<(std::ostream& out, const Process_fifo& process_fifo);
