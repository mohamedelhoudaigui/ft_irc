#ifndef USER_MANAGEMENT_HPP
#define USER_MANAGEMENT_HPP

#include <string>
#include <vector>
#include <sys/epoll.h>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <cerrno>
#include <cstring>

#include "user.hpp"
#include "parser.hpp"

#define BUFFER_SIZE 512

class UserManag
{
	public:
		UserManag();
		UserManag(const UserManag & other);
		const UserManag & operator=(const UserManag & other);
		~UserManag();

		void			process(struct epoll_event event);
		void			set_epoll_fd(int _epoll_fd);
		void    		process_buffer(User & user);

		void			add_user(int fd);
		void			remove_user(int fd);
		User &			get_user(int fd);
		bool			check_user(int fd);	
		
	private:
		std::vector<User>						users;
		int										epoll_fd;
		Parser									parser;
};


#endif