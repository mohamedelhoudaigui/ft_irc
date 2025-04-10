#include "../headers/parser.hpp"


Parser::Parser() {}


const Parser & Parser::operator=(const Parser & other) {
   if (this != &other) {
        // nothing to copy
   }
   return (*this); 
}

Parser::Parser(const Parser & other) {
    *this = other;
}

Parser::~Parser() {}


//-------------------------------


bool    Parser::parse(User & user)
{
    if (!isEnded(user))
    {
        std::cout << "still buffering no \\r\\n" << std::endl;
        return (true);
    }

    std::cout << "buffering ended:" << std::endl;
    std::cout << user.get_buffer();


    return (replay(user));
}

bool    Parser::replay(User & user)
{
    ssize_t bytes_sent = send(user.get_fd(), "ack!\r\n", 6, 0);
    if (bytes_sent == -1)
    {
        perror("send");
        return (false);
    }

    user.clear_buffer();
    return (true);
}

bool    Parser::isEnded(User & user) {
    std::string & user_buffer = user.get_buffer();
    if (user_buffer.size() < 2)
        return (false);
    if (user_buffer.substr(user_buffer.size() - 2, 2) == POSTFIX)
        return (true);
    return (false);
}