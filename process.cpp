// g++ -std=c++11 -o process process.cpp -lrpc -lpthread

#include <iostream>
#include <queue>
#include "rpc/server.h"
#include "rpc/client.h"
#include <mutex>
#include <chrono>
#include <iomanip> 
#include <string>
#include <sstream>

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
    if(argc != 3) {
        std::cout << "Numero incorreto de argumentos" << std::endl;
        exit(-1);
    }
    return std::stoi(argv[1]);
}

int get_iter_number(int argc, char** argv) {
    if(argc != 3) {
        std::cout << "Numero incorreto de argumentos" << std::endl;
        exit(-1);
    }
    return std::stoi(argv[2]);
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

void log_results(int pid) 
{
    // TODO 3
    std::cout << "[ID=" << pid << "] Acessando arquivo resultados.txt" << std::endl;
}

void process_loop(int iter_number, int pid, int rcv_grant_port, rpc::client& client) 
{
    for(int r=0; r<iter_number; r++) 
    {
        client.call("request", pid, PROCESS_IP, rcv_grant_port);

        // TODO 2: pensar em usar blocking wait
        while(!is_granted);
        log_results(pid);
        client.call("release", pid, PROCESS_IP);
    }
}

int main(int argc, char** argv) 
{
    int pid = get_id(argc, argv);
    int iter_number = get_iter_number(argc, argv);

    int rcv_grant_port = BASE_PORT + pid;
    rpc::server grant_server(rcv_grant_port);
    grant_server.bind("grant", &grant);
    grant_server.async_run(NUM_THREADS);

    //create_rcv_grant_server(pid, rcv_grant_port);

    rpc::client client(COORDINATOR_IP, COORDINATOR_PORT);
    //process_loop(iter_number, pid, rcv_grant_port, client);
    for(int r=0; r<iter_number; r++) 
    {
        client.call("request", pid, PROCESS_IP, rcv_grant_port);

        // TODO 2: pensar em usar blocking wait
        while(!is_granted);
        log_results(pid);
        client.call("release", pid, PROCESS_IP);
    }
    return 0;
}