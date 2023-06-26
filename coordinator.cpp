// g++ -std=c++11 -o coordinator coordinator.cpp -lrpc -lpthread

#include <iostream>
#include <queue>
#include "rpc/server.h"
#include "rpc/client.h"
#include <mutex>
#include <chrono>
#include <iomanip> 
#include <string>
#include <sstream>

#define GRANT           0
#define REQUEST         1
#define RELEASE         2
#define NUM_THREADS     12

std::mutex mtx_fifo;
std::mutex mtx_send_grant;
std::mutex mtx_write_stats;

class Process_info {
    public:
        Process_info(int pid, const std::string& ip, int port)
            : pid_(pid), ip_(ip), port_(port) {}

        const int get_pid() const {
            return pid_;
        }

        const std::string& get_ip() const {
            return ip_;
        }

        int get_port() const {
            return port_;
        }

    private:
        int pid_;
        std::string ip_;
        int port_;
};

class Process_fifo {
    public:
        bool empty() { 
            mtx_fifo.lock();
            bool is_empty = fifo.empty();
            mtx_fifo.unlock();
            return is_empty;
        }

        void push(Process_info process) {
            mtx_fifo.lock();
            fifo.push(process);
            mtx_fifo.unlock();
        }

        Process_info pop() {
            mtx_fifo.lock();
            Process_info head_process = fifo.front();
            fifo.pop();
            mtx_fifo.unlock();
            return head_process;
        }

        Process_info head() {
            mtx_fifo.lock();
            Process_info process = fifo.front();
            mtx_fifo.unlock();
            return process;
        }

    private:
        std::queue<Process_info> fifo;
};

Process_fifo fifo;

std::string get_date() 
{
    auto current_time = std::chrono::system_clock::now();

    auto time = std::chrono::system_clock::to_time_t(current_time);
    //auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(current_time.time_since_epoch()) % 1000;
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(current_time.time_since_epoch()) % 1000000;
    auto timeinfo = *std::localtime(&time);

    std::stringstream ss;
    ss << std::setfill('0');
    ss << std::setw(2) << timeinfo.tm_hour << ":"
       << std::setw(2) << timeinfo.tm_min << ":"
       << std::setw(2) << timeinfo.tm_sec << "."
       //<< std::setw(3) << milliseconds.count() << "."
       << std::setw(6) << microseconds.count();

    return ss.str();
}

void store_statistics(int msg_type, const Process_info& process) 
{
    mtx_write_stats.lock();

    // TODO 2: as estatísticas devem ficar em uma estrutura
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

    // TODO 1: terminal

    return 0;
}