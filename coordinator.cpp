/*
g++ -std=c++11 -c utils.cpp -o utils.o 
g++ -std=c++11 -c process_info.cpp -o process_info.o 
g++ -std=c++11 -c process_fifo.cpp -o process_fifo.o 
g++ -std=c++11 -c coordinator.cpp -o coordinator.o
g++ -std=c++11 utils.o process_info.o process_fifo.o coordinator.o -lrpc -lpthread -o coordinator
*/

#include <iostream>
#include <queue>
#include "rpc/server.h"
#include "rpc/client.h"
#include <mutex>
#include <chrono>
#include <iomanip> 
#include <string>
#include <sstream>
#include "process_info.h"
#include "process_fifo.h"
#include "utils.h"
#include "record_list.h"

std::mutex mtx_send_grant;

Process_fifo fifo;
Record_list stats("log.txt");

void send_grant(Process_info& process) 
{
    std::string ip = process.get_ip();
    int port = process.get_port();
    stats.add_record(GRANT, process);
    rpc::client client(ip, port);  
    client.call("grant");
}

void request(int pid, const std::string& ip, int port) 
{
    Process_info process(pid, ip, port);
    stats.add_record(REQUEST, process);

    mtx_send_grant.lock();
    if(fifo.empty())
        send_grant(process);
    fifo.push(process);
    mtx_send_grant.unlock();
}

void release(int pid, const std::string& ip)
{
    stats.add_record(RELEASE, pid);

    fifo.pop();
    if(!fifo.empty()) {
        Process_info top_process = fifo.head();
        send_grant(top_process);
    }
}

int main(int argc, char** argv) 
{
    if(argc != 2) {
        std::cout << "Usage: coordinator [thread_num]" << std::endl;
        return -1;
    }
    int num_threads = std::stoi(argv[1]);

    rpc::server srv(9090);

    srv.bind("request", &request);
    srv.bind("release", &release);

    srv.async_run(num_threads);

    std::cin.ignore();
    stats.print_access_count();
    //while(1);

    // TODO 6: terminal

    return 0;
}