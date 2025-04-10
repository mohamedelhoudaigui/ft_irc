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

void    Parser::parse(User & user)
{
    (void)user;
    // not implmeneted
}

bool    Parser::replay(User & user)
{
    ssize_t bytes_sent = send(user.get_fd(), "ack!\n", 6, 0);
    if (bytes_sent == -1)
    {
        perror("send");
        return false;
    }
    return true;
}