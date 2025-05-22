#pragma once

#include "includes.hpp"
#include "replys.hpp"
#include "channels.hpp"

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

		std::string		get_host_name();
		std::string		get_nick_name();
		void			set_nick_name(std::string nick);

		std::string		get_real_name();
		void			set_real_name(std::string real);

		std::string		get_user_name();
		void			set_user_name(std::string username);

		bool			get_auth();
		void			set_auth(bool a);

		bool			get_nick_step();
		void			set_nick_step(bool a);

		bool			get_user_step();
		void			set_user_step(bool a);

		bool			get_pass_step();
		void			set_pass_step(bool a);

		std::string		get_ip_address();

		void			send_reply(std::string reply);
		
		std::string		get_displayed_nick(Channel &channel, User *user);
		private:
			int				fd;
			std::string		ip_address;

			bool			nick_step;
			bool			user_step;
			bool			pass_step;
			bool			auth;

			std::string 	user_name;
			std::string		real_name;
			std::string 	nick_name;
			std::string		user_buffer;
			bool			purge;

};
