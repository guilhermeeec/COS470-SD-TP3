/*
g++ -std=c++11 -c utils.cpp -o utils.o 
g++ -std=c++11 -c process.cpp -o process.o
g++ -std=c++11 utils.o process.o -lrpc -lpthread -o process
*/

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
#include "utils.h"

#define GRANT               0
#define REQUEST             1
#define RELEASE             2
#define PROCESS_IP          "127.0.0.1"
#define COORDINATOR_IP      "127.0.0.1"
#define COORDINATOR_PORT    9090
#define BASE_PORT           9091

bool is_granted = false;

int get_ith_integer_argument(int argc, char** argv, int index) {
    if(argc != 4) {
        std::cout << "Uso: process [id] [iter_number] [sleep_time]" << std::endl;
        exit(-1);
    }
    if(index > argc) 
        exit(-1);
    return std::stoi(argv[index]);
}

void grant()
{
    is_granted = true;
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
    int pid = get_ith_integer_argument(argc, argv, 1);
    int iter_number = get_ith_integer_argument(argc, argv, 2);
    int sleep_time = get_ith_integer_argument(argc, argv, 3);

    int rcv_grant_port = BASE_PORT + pid;
    rpc::server grant_server(rcv_grant_port);
    grant_server.bind("grant", &grant);
    grant_server.async_run(1);

    rpc::client client(COORDINATOR_IP, COORDINATOR_PORT);

    for(int r=0; r<iter_number; r++) 
    {
        client.call("request", pid, PROCESS_IP, rcv_grant_port);

        // TODO 8: pensar em usar blocking wait
        while(!is_granted);
        log_results(pid, sleep_time);
        
        client.call("release", pid, PROCESS_IP);
        is_granted = false;
    }
    return 0;
}