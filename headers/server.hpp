#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
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

	private:

		// epoll data:
		int					server_fd;
		struct sockaddr_in	addr;
		struct pollfd		fds[MAX_EVENTS];
		int					nfds;

		// irc server data:
		int 				port;
		std::string 		password;
		Parser				parser;
};

#endif