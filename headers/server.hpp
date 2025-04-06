#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <sys/epoll.h>
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

#include "users.hpp"

#define MAX_EVENTS 10


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
		void	epoll_loop();
		void	server_action();
		void	user_action(struct epoll_event event);
		void	start();

	private:

		// epoll data:
		int 				epoll_fd;
		int					server_fd;
		struct sockaddr_in	addr;
		struct epoll_event	event;
		struct epoll_event	events[MAX_EVENTS];

		// irc server data:
		int 				port;
		std::string 		password;
		Users				users_management;
};

#endif