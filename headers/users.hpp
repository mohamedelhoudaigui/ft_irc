#ifndef USERS_HPP
#define USERS_HPP

#include <string>
#include <type_traits>
#include <vector>
#include <sys/epoll.h>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <iostream>

#include "user.hpp"

#define BUFFER_SIZE 512

class Users
{
	public:
		Users();
		Users(const Users & other);
		const Users & operator=(const Users & other);
		~Users();

		void			process(struct epoll_event event);
		void			add_user(int fd);
		void			remove_user(int fd, struct epoll_event event);
		const User &	get_user(int fd);
		bool			check_user(int fd);	

		void			set_epoll_fd(int _epoll_fd);
		
	private:
		std::vector<User>	users;
		int					epoll_fd;
};


#endif