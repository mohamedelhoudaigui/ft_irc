#pragma once

#include "includes.hpp"
#include "parser.hpp"


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

		// poll data:
		int							server_fd;
		struct sockaddr_in			addr;
		std::vector<struct pollfd>	fds;

		int 				port;
		std::string 		password;
		Parser				parser;
		std::string			creation_time;
};
