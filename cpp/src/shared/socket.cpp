#include "socket.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

SocketServer::SocketServer() : server_fd(-1), client_fd(-1) {}

bool SocketServer::init() {
    unlink(socket_path);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("[!] Socket creation failed");
        return false;
    }

    // Server socket must be non-blocking for accept() to poll
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    memset(&address, 0, sizeof(sockaddr_un));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socket_path, sizeof(address.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(sockaddr_un)) == -1) {
        perror("[!] Socket bind failed");
        return false;
    }
    
    chmod(socket_path, 0666);

    if (listen(server_fd, 5) == -1) {
        perror("[!] Socket listen failed");
        return false;
    }

    return true;
}

void SocketServer::send_telemetry(const std::string& json_data) {
    if (client_fd != -1) {
        std::string packet = json_data + "\n";
        
        // MSG_DONTWAIT: Don't let the engine hang if the GUI is slow
        // MSG_NOSIGNAL: Don't crash the engine if the GUI is closed
        ssize_t sent = send(client_fd, packet.c_str(), packet.length(), MSG_NOSIGNAL | MSG_DONTWAIT);
        
        if (sent == -1) {
            // Check if the failure is just a full buffer
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Buffer is full. Do NOTHING. 
                // Just return and let the engine finish its 1ms cycle.
                return; 
            } else {
                // A real error happened (GUI was actually closed)
                std::cout << "[ACE] Connection Lost (FD: " << client_fd << ")" << std::endl;
                close(client_fd);
                client_fd = -1;
            }
        }
    }
}

std::string SocketServer::poll_commands() {
    // 1. ATTEMPT CONNECTION: Only if client_fd is exactly -1
    if (client_fd == -1) {
        int new_client = accept(server_fd, NULL, NULL);
        
        if (new_client >= 0) {
            // SUCCESS: We have a real connection
            client_fd = new_client;
            
            int flags = fcntl(client_fd, F_GETFL, 0);
            fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
            
            // This will now only print ONCE because client_fd is no longer -1
            std::cout << "[ACE] Python GUI connected (FD: " << client_fd << ")" << std::endl;
        }
    }

    // 2. READ DATA: Only if we have an active connection
    if (client_fd != -1) {
        char buffer[4096];
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes > 0) {
            buffer[bytes] = '\0';
            return std::string(buffer);
        } 
        else if (bytes == 0) {
            // DISCONNECT: Client closed the pipe
            std::cout << "[ACE] Python GUI disconnected (FD: " << client_fd << ")" << std::endl;
            close(client_fd);
            client_fd = -1; // Reset to -1 so we can accept a new connection
        }
        else {
            // Error handling: Ignore "no data" (EAGAIN), close on real errors
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                close(client_fd);
                client_fd = -1;
            }
        }
    }
    
    return "";
}

void SocketServer::cleanup() {
    if (client_fd != -1) {
        close(client_fd);
        client_fd = -1;
    }
    if (server_fd != -1) {
        close(server_fd);
        server_fd = -1;
    }
    unlink(socket_path);
}

SocketServer::~SocketServer() {
    cleanup();
}