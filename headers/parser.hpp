#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <sstream>

#include "user.hpp"
#include "replys.hpp"

typedef struct cmd_line
{
    std::string                 cmd;
    std::vector<std::string>    args;
    std::string                 trailing;

}   cmd_line;


class Parser
{
    public:
        Parser();
        Parser(std::string password);
        Parser(const Parser & other);
        const Parser & operator=(const Parser & other);
        ~Parser();

        void    parse(User & user);
        void    redirect_cmd(User & user, cmd_line c);
        bool    isEnded(User & user);

    private:
        std::string server_password;

};

std::ostream& operator<<(std::ostream& os, const cmd_line& c);


#endif