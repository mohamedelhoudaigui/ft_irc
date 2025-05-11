#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <string>
#include <cstdlib>
#include <iostream>
#include <errno.h>
#include <vector>

#include "parser.hpp"

#define MAX_EVENTS 500

class Server
{
	public:

		// canonical form:
		Server();
		Server(int port, std::string password);
		Server(const Server & other);
		const Server & operator=(const Server & other);
		~Server();

		// server methods:
		void	set_nonblocking(int fd);
		void	poll_loop();
		void	server_action();
		void    user_action(struct pollfd event);
		void	remove_client(int fd);
		void	start();
		std::string		get_creation_time();

	private:

		// epoll data:
		int							server_fd;
		struct sockaddr_in			addr;
		std::vector<struct pollfd>	fds;

		int 				port;
		std::string 		password;
		Parser				parser;
		std::string			creation_time;
};

#endif