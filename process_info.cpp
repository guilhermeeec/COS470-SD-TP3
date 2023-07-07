#include "process_info.h"

Process_info::Process_info(int pid, const std::string& ip, int port)
    : pid_(pid), ip_(ip), port_(port) {}

int Process_info::get_pid() const {
    return pid_;
}

const std::string& Process_info::get_ip() const {
    return ip_;
}

int Process_info::get_port() const {
    return port_;
}

std::ostream& operator<<(std::ostream& out, const Process_info& process_info) {
    out << "pid = " << process_info.get_ip() << std::endl; 
    return out;
}