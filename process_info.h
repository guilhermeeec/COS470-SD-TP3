#pragma once

#include <string>
#include <iostream>

class Process_info {
    public:
        Process_info(int pid, const std::string& ip, int port);

        int get_pid() const;
        const std::string& get_ip() const;
        int get_port() const;

    private:
        int pid_;
        std::string ip_;
        int port_;
};

std::ostream& operator<<(std::ostream& out, const Process_info& process_info);