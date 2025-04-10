#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <stdio.h>

#include "user.hpp"

class Parser
{
    public:
        Parser();
        Parser(const Parser & other);
        const Parser & operator=(const Parser & other);
        ~Parser();

        void    parse(User & user);
        bool    replay(User & user);

};



#endif