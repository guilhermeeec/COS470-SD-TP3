#pragma once

#include <string>
#include "process_info.h"
#include <iostream>

enum msg_type_t {
    GRANT = 0,
    REQUEST = 1,
    RELEASE = 2
};

class Record {
    public:
        Record(msg_type_t msg_type, int pid);
        Record(msg_type_t msg_type, Process_info process_info);

        msg_type_t get_msg_type() const;
        int get_pid() const;
        std::string get_time() const;
    
    private:
        msg_type_t msg_type_;
        int pid_;
        std::string time_;
};

std::ostream& operator<<(std::ostream& out, const Record& record);