#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip> 
#include "utils.h"

std::string get_date() 
{
    auto current_time = std::chrono::system_clock::now();

    auto time = std::chrono::system_clock::to_time_t(current_time);
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(current_time.time_since_epoch()) % 1000000;
    auto timeinfo = *std::localtime(&time);

    std::stringstream ss;
    ss << std::setfill('0');
    ss << std::setw(2) << timeinfo.tm_hour << ":"
       << std::setw(2) << timeinfo.tm_min << ":"
       << std::setw(2) << timeinfo.tm_sec << "."
       << std::setw(6) << microseconds.count();

    return ss.str();
}