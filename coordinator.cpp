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

#define COORDINATOR_PORT    9090

std::mutex mtx_send_grant;

Process_fifo fifo;
Record_list stats("log.txt");
int num_releases_received = 0;
int num_processes;
int num_repetitions;


void check_release_completion() {
    while(num_releases_received != num_processes * num_repetitions) {}
    std::cout << std::endl << "Received the specified number of releases. Finishing coordinator execution." << std::endl;
    std::exit(0);
}

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
    if (!fifo.empty()) {
        Process_info top_process = fifo.head();
        send_grant(top_process);
    }

    num_releases_received++;
}

void terminal_interaction() {
    bool running = true;
    while(running) {
        std::cout << std::endl;
        std::cout << "Terminal commands:" << std::endl;
        std::cout << "1- Print current requests queue" << std::endl;
        std::cout << "2- Print the number of times each process was attended" << std::endl;
        std::cout << "3- Finish coordinator execution" << std::endl;
        std::cout << "[IN]: ";

        char in = std::cin.get();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        std::cout << std::endl;

        switch (in)
        {
            case '1':
            std::cout << fifo;
            break;

            case '2':
            stats.print_access_count();
            break;

            case '3':
            running = false;
            break;

            default:
            std::cout << "Invalid option" << std::endl;
            break;
        }
    }
}

int main(int argc, char** argv) 
{
    if(argc != 2 && argc != 4) {
        std::cout << "Temrinal Usage: coordinator [thread_num]" << std::endl;
        std::cout << "Test Usage: coordinator [thread_num] [processes_num] [repetitions_num]" << std::endl;
        return -1;
    }

    bool terminal = true;
    int num_threads = std::stoi(argv[1]);
    if (argc == 4) {
        terminal = false;
        num_processes = std::stoi(argv[2]);
        num_repetitions = std::stoi(argv[3]);
    }

    rpc::server srv(COORDINATOR_PORT);

    srv.bind("request", &request);
    srv.bind("release", &release);

    srv.async_run(num_threads);

    if(terminal)
        terminal_interaction();
    else
        check_release_completion();


    return 0;
}