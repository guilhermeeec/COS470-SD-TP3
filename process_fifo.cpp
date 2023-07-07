#include "process_fifo.h"

bool Process_fifo::empty() {
    std::lock_guard<std::mutex> lock(mtx_fifo);
    return fifo.empty();
}

void Process_fifo::push(const Process_info& process) {
    std::lock_guard<std::mutex> lock(mtx_fifo);
    fifo.push(process);
}

Process_info Process_fifo::pop() {
    std::lock_guard<std::mutex> lock(mtx_fifo);
    Process_info head_process = fifo.front();
    fifo.pop();
    return head_process;
}

Process_info Process_fifo::head() {
    std::lock_guard<std::mutex> lock(mtx_fifo);
    return fifo.front();
}

std::ostream& operator<<(std::ostream& out, const Process_fifo& process_fifo) {
    std::queue<Process_info> fifo_copy = process_fifo.fifo;
    out << "Queue: ";
    while (!fifo_copy.empty()) {
        Process_info process_info_element = fifo_copy.front();
        out << process_info_element << " ";
        fifo_copy.pop();
    }
    out << std::endl; 
    return out;
}
