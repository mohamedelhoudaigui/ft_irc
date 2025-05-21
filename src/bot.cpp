#include "../headers/bot.hpp"

void print_usage(void) {
    std::cerr << "usage: " << "./ircbot server_address port password" << std::endl;
}

Bot::Bot(const std::string& server, int port, const std::string& password)
    : sockfd(0), server_address(server), port(port),
      nickname("Gambabot"), username("gambabot"), realname("Dice Rolling Bot"),
      channel(""), server_password(password) {
    srand(time(NULL));
}

Bot::~Bot() {
    if (sockfd > 0)
        close(sockfd);
}

void Bot::connect_to_server() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        throw std::runtime_error("Error creating socket");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (server_address == "localhost")
        server_address = "127.0.0.1";
    if (inet_pton(AF_INET, server_address.c_str(), &server_addr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid address");
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        throw std::runtime_error("Connection failed");
    }

    fds[0].fd = sockfd;
    fds[0].events = POLLIN;
}

void Bot::authenticate() {
    std::string pass_msg = "PASS " + server_password + "\r\n";
    std::string nick_msg = "NICK " + nickname + "\r\n";
    std::string user_msg = "USER " + username + " 0 * :" + realname + "\r\n";

    send_message(pass_msg);
    send_message(nick_msg);
    send_message(user_msg);
}

void Bot::join_channel() {
    std::string join_msg = "JOIN #bot\r\n";
    send_message(join_msg);
}

void Bot::send_message(const std::string& message) {
    if (send(sockfd, message.c_str(), message.length(), 0) < 0) {
        throw std::runtime_error("Error sending message");
    }
}

void Bot::handle_roll_command(const std::string& sender, const std::string& channel) {
    int die1 = (rand() % 6) + 1;
    int die2 = (rand() % 6) + 1;
    std::string message = "PRIVMSG " + channel + " :" + sender + " rolled a " + 
                          to_string(die1) + " and a " + to_string(die2) + "\r\n";
    send_message(message);
}

void Bot::handle_flip_command(const std::string& sender, const std::string& channel) {
    std::string coinflip = (rand() % 2) ? "HEADS" : "TAILS";
    std::string message = "PRIVMSG " + channel + " :" + sender + " flipped " + coinflip + "\r\n";
    send_message(message);
}

void Bot::handle_server_response() {
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));
    
    if (recv(sockfd, buffer, sizeof(buffer) - 1, 0) <= 0) {
        throw std::runtime_error("Connection closed by server");
    }

    std::string response(buffer);
    std::stringstream ss(response);
    std::string line;

    while (std::getline(ss, line)) {
        if (line.empty() || line == "\r") continue;

        if (line.find("PING") != std::string::npos) {
            std::string pong_msg = "PONG " + line.substr(5) + "\r\n";
            send_message(pong_msg);
        }
        else if (line.find("PRIVMSG") != std::string::npos) {
            size_t nick_end = line.find('!');
            if (nick_end == std::string::npos)
                continue ;
            
            std::string sender = line.substr(1, nick_end - 1);
            size_t channel_start = line.find('#');
            if (channel_start == std::string::npos)
                continue ;
            
            size_t channel_end = line.find(' ', channel_start);
            if (channel_end == std::string::npos)
                continue ;
            
            std::string channel = line.substr(channel_start, channel_end - channel_start);
            size_t msg_start = line.find(':', channel_end);
            if (msg_start == std::string::npos)
                continue ;
            
            std::string message = line.substr(msg_start + 1);
            
            if (message.find("!roll") != std::string::npos) {
                handle_roll_command(sender, channel);
            }
            else if (message.find("!flip") != std::string::npos) {
                handle_flip_command(sender, channel);
            }
        }
    }
}

void Bot::run() {
    try {
        connect_to_server();
        authenticate();
        join_channel();

        while (true) {
            if (poll(fds, 1, -1) < 0) {
                throw std::runtime_error("Poll error");
            }

            if (fds[0].revents & POLLIN) {
                handle_server_response();
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void    valid_password(std::string password)
{
    if (password.size() < 4 || password.size() > 20)
    {
        std::cerr << "password must be between 4 and 20 charcters, only alphanumeric" << std::endl;
        exit(1);
    }

    for (size_t i = 0; i < password.size(); ++i)
    {
        if (!isalnum(password[i]))
        {
            std::cerr << "password must be between 4 and 20 charcters, only alphanumeric" << std::endl;
            exit(1);
        }
    }
}


int main(int argc, char* argv[]) {
    if (argc != 4) {
        print_usage();
        return 1;
    }

    std::string server_address = argv[1];
    valid_password(argv[2]);
    long portCheck = std::atol(argv[2]);
    if (portCheck <= 0 || portCheck > 65535) {
        std::cerr << "Error: Port number must be between 1 and 65535" << std::endl;
        return 1;
    }
    int port = static_cast<int>(portCheck);
    std::string password = argv[3];

    try {
        Bot bot(server_address, port, password);
        bot.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Connection error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
} 