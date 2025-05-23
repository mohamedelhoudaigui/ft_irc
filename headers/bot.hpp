#pragma once

#include <poll.h>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>

class Bot {
private:
    int sockfd;
    std::string server_address;
    int port;
    std::string nickname;
    std::string username;
    std::string realname;
    std::string channel;
    std::string server_password;
    struct sockaddr_in server_addr;
    struct pollfd fds[1];

    void connect_to_server();
    void authenticate();
    void join_channel();
    void handle_server_response();
    void send_message(const std::string& message);
    void handle_roll_command(const std::string& sender, const std::string& channel);
    void handle_flip_command(const std::string& sender, const std::string& channel);

public:
    Bot(const std::string& server, int port, const std::string& password, const std::string& channel);
    ~Bot();
    void run();
};

template <typename T>
std::string to_string(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}
