#include "socket.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/socket.h> 
#include <sys/un.h>     

SocketServer::SocketServer() : server_fd(-1), client_fd(-1) {}

bool SocketServer::init() {
    unlink(socket_path); // Clean up stale socket files

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("[!] Socket creation failed");
        return false;
    }

    // Set non-blocking so the 2000Hz loop NEVER waits for a slow GUI
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    memset(&address, 0, sizeof(sockaddr_un));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socket_path, sizeof(address.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(sockaddr_un)) == -1) {
        perror("[!] Socket bind failed");
        return false;
    }
    
    chmod(socket_path, 0666); // Ensure Python has permissions to read/write

    if (listen(server_fd, 5) == -1) {
        perror("[!] Socket listen failed");
        return false;
    }

    return true;
}

void SocketServer::send_telemetry(const std::string& json_data) {
    if (client_fd != -1) {
        // The newline is the delimiter for Python's readline()
        std::string packet = json_data + "\n";
        
        // MSG_NOSIGNAL prevents the C++ app from crashing if Python closes suddenly
        ssize_t sent = send(client_fd, packet.c_str(), packet.length(), MSG_NOSIGNAL | MSG_DONTWAIT);
        
        if (sent == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cout << "[ACE] Connection Lost (FD: " << client_fd << ")" << std::endl;
                close(client_fd);
                client_fd = -1;
            }
        }
    }
}

std::string SocketServer::poll_commands() {
    // 1. Check for new connections
    if (client_fd == -1) {
        int new_client = accept(server_fd, NULL, NULL);
        if (new_client >= 0) {
            client_fd = new_client;
            int flags = fcntl(client_fd, F_GETFL, 0);
            fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
            std::cout << "[ACE] Python GUI connected (FD: " << client_fd << ")" << std::endl;
        }
    }

    // 2. Read incoming commands
    if (client_fd != -1) {
        char buffer[16384]; 
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
        
        if (bytes > 0) {
            buffer[bytes] = '\0';
            // We return the string. If multiple commands are present, 
            // Logic::handle_command should be prepared to split them or process the latest.
            return std::string(buffer);
        } 
        else if (bytes == 0) {
            std::cout << "[ACE] Python GUI disconnected" << std::endl;
            close(client_fd);
            client_fd = -1;
        }
    }
    return "";
}

void SocketServer::cleanup() {
    if (client_fd != -1) { close(client_fd); client_fd = -1; }
    if (server_fd != -1) { close(server_fd); server_fd = -1; }
    unlink(socket_path);
}

SocketServer::~SocketServer() { cleanup(); }