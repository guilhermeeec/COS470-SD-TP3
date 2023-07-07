#include "record_list.h"
#include <fstream>
#include <iomanip>

Record_list::Record_list(const std::string filename) {
    this->log_file.open(filename, std::ios::app);
}

Record_list::~Record_list() {
    this->store_records();
    this->log_file.close();
}

void Record_list::add_record(msg_type_t msg_type, Process_info process_info) {
    std::lock_guard<std::mutex> lock(mtx);
    Record record(msg_type, process_info);
    //std::cout << record;
    this->records_list.push_back(record);
    if(msg_type == RELEASE)
        this->increment_count_map(process_info.get_pid());     
}

void Record_list::add_record(msg_type_t msg_type, int pid) {
    std::lock_guard<std::mutex> lock(mtx);
    Record record(msg_type, pid);
    //std::cout << record;
    this->records_list.push_back(record);
    if(msg_type == RELEASE)
        this->increment_count_map(pid);     
}

void Record_list::increment_count_map(int key) {
    this->dict_pid_to_acess[key]++;
    //std::cout << "key=" <<key << " value=" << this->dict_pid_to_acess[key] << std::endl;
}

int Record_list::store_records() {
    if(!this->log_file.is_open()) 
        return -1;
    for(auto record : this->records_list)
        this->log_file << record;
    return 0;
}

void Record_list::print_access_count() {
    std::cout << std::setw(10) << std::left << "PID" 
              << std::setw(10) << std::left << "Accesses" << std::endl;
    for (const auto& pair : this->dict_pid_to_acess) 
        std::cout << std::setw(10) << std::left << pair.first
                  << std::setw(10) << std::left << pair.second << std::endl;
}