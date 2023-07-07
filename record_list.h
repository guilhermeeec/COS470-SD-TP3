#pragma once

#include "record.h"
#include "process_info.h"
#include <map>
#include <mutex>
#include <vector>
#include <fstream>

class Record_list {
    public:
        Record_list(const std::string filename);
        ~Record_list();
        void add_record(msg_type_t msg_type, Process_info process_info);
        void add_record(msg_type_t msg_type, int pid);
        int store_records();
        void print_access_count();

    private:
        std::mutex mtx;
        std::map<int,int> dict_pid_to_acess;
        std::vector<Record> records_list; 
        std::ofstream log_file;

        void increment_count_map(int key);
};