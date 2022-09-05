#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>
#include <exception>
#include <vector>
#include <unistd.h>
#include "class_exeption.hpp"

#define PORT 8081
#define IP "0.0.0.0"
#define MAX_QUEUED_CONNEXIONS 512
#define NB_SERVER 1
#define MAX_CONNEXIONS 2048
#define BUFFER_SIZE 4096

class ErrorSocket;
class ErrorSetSockOpt;
class ErrorBind;
class ErrorListen;
struct Client {
    Client(){
        socket = -1;
        fd_file = -1;
        for (int i = 0; i < 2; i++){
            cgi_in[i] = -1;
            cgi_out[i] = -1;
        }
        sockaddr = sockaddr_in();
    }
    Client(int new_socket, sockaddr_in new_addr){
        socket = new_socket;
        fd_file = -1;
        for (int i = 0; i < 2; i++){
            cgi_in[i] = -1;
            cgi_out[i] = -1;
        }
        sockaddr = new_addr;
    }
    Client(const Client &src){
        *this = src;
    }
    ~Client(){}
    Client &operator=(const Client &src) {
        socket = src.socket;
        fd_file = src.fd_file;
        for (int i = 0; i < 2; i++){
            cgi_in[i] = src.cgi_in[i];
            cgi_out[i] = src.cgi_out[i];
        }
        sockaddr = src.sockaddr;
        return (*this);
    }

    struct sockaddr_in sockaddr;
    int socket;
    int cgi_in[2];
    int cgi_out[2];
    int fd_file;
};

std::vector<Client> vector_clients;


void config_socket(int *lst_socket){
    
    struct sockaddr_in address;
    address.sin_addr.s_addr = inet_addr(IP);
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    memset(&address.sin_zero, 0, sizeof(address.sin_zero));

    if ((*lst_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw ErrorSocket();

    fcntl(*lst_socket, F_SETFL, O_NONBLOCK);
    int reuse = 1;

    if (setsockopt(*lst_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0)
        throw ErrorSetSockOpt();
    if (bind(*lst_socket, (struct sockaddr *) &address, sizeof(address)))
        throw ErrorBind();
    if (listen(*lst_socket, MAX_QUEUED_CONNEXIONS) < 0)
        throw ErrorListen();
}

void config_fd(int &bc_sock, int *lst_socket, fd_set *rfd, fd_set *wfd){
    
    for (size_t i = 0; i < NB_SERVER; i++){
        FD_SET(lst_socket[i], rfd);
        for (size_t j = 0; j < vector_clients.size(); j++){
            if (vector_clients[j].socket != -1){
                FD_SET(vector_clients[j].socket, rfd);
                FD_SET(vector_clients[j].socket, wfd);
            }
            if (vector_clients[j].cgi_in[1] != -1)
                FD_SET(vector_clients[j].cgi_in[1], wfd);
            if (vector_clients[j].cgi_out[0] != -1)
                FD_SET(vector_clients[j].cgi_out[0], rfd);
            if (vector_clients[j].fd_file != -1)
                FD_SET(vector_clients[j].fd_file, rfd);
        }
    }
    for (size_t i = 0; i < NB_SERVER; i++){
        if (bc_sock < lst_socket[i])
            bc_sock = lst_socket[i];
        for (size_t j = 0; j < vector_clients.size(); j++){
            if (bc_sock < vector_clients[j].socket)
                bc_sock = vector_clients[j].socket;
            if (bc_sock < vector_clients[j].cgi_in[0])
                bc_sock = vector_clients[j].cgi_in[0];
            if (bc_sock < vector_clients[j].cgi_in[1])
                bc_sock = vector_clients[j].cgi_in[1];
            if (bc_sock < vector_clients[j].cgi_out[0])
                bc_sock = vector_clients[j].cgi_out[0];
            if (bc_sock < vector_clients[j].cgi_out[1])
                bc_sock = vector_clients[j].cgi_out[1];
        }
    }
}

void add_clients(int *lst_socket, fd_set *rfd){
    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    for (size_t i = 0; i < NB_SERVER; i++){
        if (vector_clients.size() < MAX_CONNEXIONS && FD_ISSET(lst_socket[i], rfd)) {
            if ((new_socket = accept(lst_socket[i], (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
                return ;
            fcntl(new_socket, F_SETFL, O_NONBLOCK);
            vector_clients.push_back(Client(new_socket, address));
        }
    }
}

void read_request(Client *client, size_t j){
    int valread;
    char data[BUFFER_SIZE + 1];
    if ((valread = read(client->socket, data, BUFFER_SIZE)) <= 0){
        close(client->socket);
        vector_clients.erase(vector_clients.begin() + j);
        j--;
        return;
    }
    else{
        data[valread] = '\0';
    }
    std::cout << "data : '" << data << "'" << std::endl;
}

int main(){
    
    int bc_sock = -1;
    int lst_socket[NB_SERVER];
    fd_set rfd, wfd;
    Client *client;
    try {
        for (size_t i = 0; i < NB_SERVER; i++)
            config_socket(&lst_socket[i]);
    }
    catch (const std::exception &e){
        e.what();
        return -1;
    }
    while (1){
        
        config_fd(bc_sock, lst_socket, &rfd, &wfd);
        //std::cout << "bc_sock : " << bc_sock << ", clients : " << vector_clients.size() << std::endl;
        int activity = select(bc_sock + 1, &rfd, &wfd, NULL, NULL);
        if (activity < 0){
            std::cout << "Error: Fail select" << std::endl;
            perror("Select");
            return 1;
        }
        add_clients(lst_socket, &rfd);
        for (size_t i = 0; i < NB_SERVER; i++)
            for (size_t j = 0; j < vector_clients.size(); j++){
                client = &vector_clients[j];
                read_request(client, j);
            }
    }
    return 0;
}