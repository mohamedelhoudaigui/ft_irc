#ifndef USER_HPP
#define USER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

class User
{
	public:
		User();
		User(int fd);
		User(const User & other);
		const User & operator=(const User & other);
		bool operator==(const User & other);
		~User();

		void	get_socket_address() const;
		int		get_fd() const;

	private:
		int		fd;
		char*	ip_address;

};


#endif