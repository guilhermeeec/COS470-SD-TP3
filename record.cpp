#include "record.h"
#include "utils.h"

Record::Record(msg_type_t msg_type, int pid) 
    : msg_type_(msg_type), pid_(pid) 
{
    time_ = get_date();
}

Record::Record(msg_type_t msg_type, Process_info process_info) 
    : msg_type_(msg_type) 
{
    pid_ = process_info.get_pid();
    time_ = get_date();
}

msg_type_t Record::get_msg_type() const {
    return this->msg_type_;
}

int Record::get_pid() const {
    return this->pid_;
}

std::string Record::get_time() const {
    return this->time_;
}

std::ostream& operator<<(std::ostream& out, const Record& record) {
    switch (record.get_msg_type())
    {
        case REQUEST:
        out << "[R] Request-" << record.get_pid() << "-" << record.get_time() << std::endl; 
        break;

        case GRANT:
        out << "[S] Grant-" << record.get_pid() << "-" << record.get_time() << std::endl; 
        break;

        case RELEASE:
        out << "[R] Release-" << record.get_pid() << "-" << record.get_time() << std::endl; 
        break;

        default:
        break;
    }
    return out;
}