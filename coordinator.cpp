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

#define GRANT           0
#define REQUEST         1
#define RELEASE         2
#define NUM_THREADS     12

std::mutex mtx_send_grant;
std::mutex mtx_write_stats;

Process_fifo fifo;

void store_statistics(int msg_type, const Process_info& process) 
{
    mtx_write_stats.lock();

    // TODO 4 e 5: as estatísticas devem ficar em uma estrutura
    switch (msg_type)
    {
        case REQUEST:
        std::cout << "[R] Request-" << process.get_pid() << "-" << get_date() << std::endl; 
        break;

        case RELEASE:
        std::cout << "[R] Release-" << process.get_pid() << "-" << get_date() << std::endl; 
        break;

        case GRANT:
        std::cout << "[S] Grant-" << process.get_pid() << "-" << get_date() << std::endl; 
        break;
    
        default:
        break;
    }
    mtx_write_stats.unlock();
}

void send_grant(Process_info& process) 
{
    std::string ip = process.get_ip();
    int port = process.get_port();
    store_statistics(GRANT, process);
    rpc::client client(ip, port);  
    client.call("grant");
}

void request(int pid, const std::string& ip, int port) 
{
    Process_info process(pid, ip, port);
    store_statistics(REQUEST, process);

    mtx_send_grant.lock();
    if(fifo.empty())
        send_grant(process);
    fifo.push(process);
    mtx_send_grant.unlock();
}

void release(int pid, const std::string& ip)
{
    Process_info requesting_process(pid, ip, 0);
    store_statistics(RELEASE, requesting_process);

    fifo.pop();
    if(!fifo.empty()) {
        Process_info top_process = fifo.head();
        send_grant(top_process);
    }
}

int main() 
{
    rpc::server srv(9090);

    srv.bind("request", &request);
    srv.bind("release", &release);

    srv.async_run(NUM_THREADS);

    while(1);

    // TODO 6: terminal

    return 0;
}