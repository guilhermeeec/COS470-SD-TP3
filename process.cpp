// g++ -std=c++11 -o process process.cpp -lrpc -lpthread

#include <iostream>
#include <fstream>
#include <queue>
#include "rpc/server.h"
#include "rpc/client.h"
#include <mutex>
#include <chrono>
#include <iomanip> 
#include <string>
#include <sstream>
#include <unistd.h>

#define GRANT               0
#define REQUEST             1
#define RELEASE             2
#define NUM_THREADS         1
#define PROCESS_IP          "127.0.0.1"
#define COORDINATOR_IP      "127.0.0.1"
#define COORDINATOR_PORT    9090
#define BASE_PORT           9091

bool is_granted = false;

int get_id(int argc, char** argv) {
    if(argc != 4) {
        std::cout << "Numero incorreto de argumentos" << std::endl;
        exit(-1);
    }
    return std::stoi(argv[1]);
}

int get_iter_number(int argc, char** argv) {
    if(argc != 4) {
        std::cout << "Numero incorreto de argumentos" << std::endl;
        exit(-1);
    }
    return std::stoi(argv[2]);
}

int get_sleep_time(int argc, char** argv) {
    if(argc != 4) {
        std::cout << "Numero incorreto de argumentos" << std::endl;
        exit(-1);
    }
    return std::stoi(argv[3]);
}

void grant()
{
    is_granted = true;
}

void create_rcv_grant_server(int pid, int rcv_grant_port) 
{
    rpc::server grant_server(rcv_grant_port);
    grant_server.bind("grant", &grant);
    grant_server.async_run(NUM_THREADS);
}
// TODO: ARQUIVO SEPARADO PARA FUNÇÕES ÚTEIS

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

void log_results(int pid, int sleep_time) 
{
    std::ofstream results;
    results.open("resultado.txt", std::ios::app);
    if (results.is_open()) {
        results << "[" << pid << "] - " << get_date() << std::endl;
        results.close();
        sleep(sleep_time);
    } else {
        std::cout << "Erro ao abrir o arquivo!" << std::endl;
    }
}


int main(int argc, char** argv) 
{
    int pid = get_id(argc, argv);
    int iter_number = get_iter_number(argc, argv);
    int sleep_time = get_sleep_time(argc, argv);

    int rcv_grant_port = BASE_PORT + pid;
    rpc::server grant_server(rcv_grant_port);
    grant_server.bind("grant", &grant);
    grant_server.async_run(NUM_THREADS);

    rpc::client client(COORDINATOR_IP, COORDINATOR_PORT);

    for(int r=0; r<iter_number; r++) 
    {
        client.call("request", pid, PROCESS_IP, rcv_grant_port);

        // TODO 2: pensar em usar blocking wait
        while(!is_granted);
        log_results(pid, sleep_time);
        
        client.call("release", pid, PROCESS_IP);
        is_granted = false;
    }
    return 0;
}