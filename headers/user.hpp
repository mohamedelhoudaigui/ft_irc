#ifndef USER_HPP
#define USER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>

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

		std::string		get_socket_address() const;
		int				get_fd() const;
		void			add_to_buffer(char* buffer);
		std::string &	get_buffer();
		void			clear_buffer();

		
		private:
			int			fd;
			std::string	ip_address;
			
			bool			auth;
			std::string 	user_name;
			std::string 	nick_name;
			std::string		user_buffer;

};


#endif