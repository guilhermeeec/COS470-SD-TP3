#pragma once

#include "record.h"
#include "process_info.h"
#include <map>
#include <mutex>
#include <vector>

class Record_list {
    public:
        Record_list();
        void add_record(msg_type_t msg_type, Process_info process_info);
        void add_record(msg_type_t msg_type, int pid);
        int store_records();
        std::map<int,int> print_acess_count();

    private:
        std::mutex mtx_map;
        std::map<int,int> pid_to_access_count;
        std::vector<Record> records_list; 
};