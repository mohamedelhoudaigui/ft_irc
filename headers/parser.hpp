#ifndef PARSER_HPP
#define PARSER_HPP

#include <poll.h>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <cerrno>
#include <map>

#include "user.hpp"
#include "replys.hpp"
#include "utils.hpp"

class Server;

#define BUFFER_SIZE 512

class Channel;

class   cmd_line
{
    public:
        std::string                 cmd;
        std::vector<std::string>    args;
        std::string                 trailing;
        void    clear();
};

class Parser
{
    public:
            Parser();
            Parser(std::string password, Server* server);
            Parser(const Parser & other);
            const Parser & operator=(const Parser & other);
            ~Parser();

            void            parse(User & user);
            void            redirect_cmd(User & user, cmd_line & c);
            bool            isEnded(User & user);
            bool            check_auth(User & user);

            void			process(struct pollfd event);
            void			set_epoll_fd(int _epoll_fd);
            void    		process_buffer(User & user, char* buffer, ssize_t bytes_recv);

            void			add_user(int fd);
            void			remove_user(int fd);
            User &			get_user(int fd);
            bool			check_user(int fd);
            bool			check_nick_name(std::string nick);

            void	        process_auth(User & user);
            //ADDED BY CAZIANE
            void            privmsg(std::string receiver, std::string msg, User &user);
            void            topic(User &user, std::string channel_name, std::string new_topic, Channel &channel);
            void            handleModeCommand(User* user, std::vector<std::string>& args);
            void	        topic_command(std::string channel_name, std::string new_topic, User& user);


            User*           find_user_by_nickname(Channel &channel,const std::string nickname);
            User*           find_invited_user(const std::string nickname);

    private:
        std::string                     server_password;
        std::vector<User*>              users;
        std::map<std::string, Channel>  channels;
        Server*                         server;

};

std::ostream& operator<<(std::ostream& os, const cmd_line& c);

#endif
