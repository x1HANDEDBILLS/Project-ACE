#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <sys/un.h>
#include <string>

class SocketServer {
private:
    int server_fd, client_fd;
    sockaddr_un address;
    const char* socket_path = "/tmp/xace.sock";

public:
    SocketServer();
    ~SocketServer();
    
    bool init();
    void send_telemetry(const std::string& json_data);
    std::string poll_commands();
    void cleanup();
};

#endif