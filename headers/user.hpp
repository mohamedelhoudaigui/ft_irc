#ifndef USER_HPP
#define USER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <stdio.h>

#include "replys.hpp"

#define BUFFER_SIZE 512

class User
{
	public:
		User();
		User(int fd);
		User(const User & other);
		const User & operator=(const User & other);
		bool operator==(const User & other);
		~User();

		std::string		get_socket_address();
		int				get_fd();

		void			add_to_buffer(char* buffer, ssize_t bytes_recv);
		void			clear_buffer();
		std::string 	get_buffer();
		void			set_buffer(std::string buffer);

		bool			get_purge();
		void			set_purge(bool s);

		
		std::string		get_nick_name();
		void			set_nick_name(std::string nick);

		std::string		get_real_name();
		void			set_real_name(std::string real);

		std::string		get_user_name();
		void			set_user_name(std::string username);

		bool			get_auth();
		void			set_auth(bool a);

		int				get_auth_steps();
		void			add_auth_step();

		std::string		get_ip_address();

		void			send_reply(std::string reply);
		
		private:
			int				fd;
			std::string		ip_address;

			int				steps;
			bool			auth;

			std::string 	user_name;
			std::string		real_name;
			std::string 	nick_name;
			std::string		user_buffer;
			bool			purge;

};


#endif