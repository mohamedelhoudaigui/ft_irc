#include "../headers/users.hpp"
#include <cstring>

// canonical form :

Users::Users() {}

Users::Users(const Users & other) {
	*this = other;
}

const Users & Users::operator=(const Users & other) {
	if (this != &other) {
		this->users = other.users;
	}
	return (*this);
}

Users::~Users() {
	for (int i = 0 ; i < this->users.size(); ++i)
    {
        close(users[i].get_fd());
    }
}

// users methods:

void    Users::set_epoll_fd(int _epoll_fd) {
    epoll_fd = _epoll_fd;
}

void	Users::process(struct epoll_event event) {

    int user_fd = event.data.fd;

    add_user(user_fd);
    const User & user = get_user(user_fd);

    if (event.events == EPOLLIN)
    {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
    
        ssize_t bytes_recv = recv(user_fd, buffer, BUFFER_SIZE, 0);
        std::cout << bytes_recv << std::endl;
    
        switch (bytes_recv)
        {
            case -1:
                if (errno == EAGAIN || errno == EWOULDBLOCK) { // no more data to read
                    break;
                } else {
                    perror("recv");
                    remove_user(user_fd, event);
                    break;
                }
    
            case 0:
                remove_user(user_fd, event);
                break;
    
            default:
                // parse user command here :

                ssize_t bytes_sent = send(user_fd, "ping\n", 6, 0);
    
                if (bytes_sent == -1) {
                    perror("send");
                    remove_user(user_fd, event);
                    break;
                }

                std::cout << buffer;
        }
    }
}


bool    Users::check_user(int fd) {
    for (int i = 0; i < users.size(); ++i)
    {
        if (users[i].get_fd() == fd)
            return (true);
    }
    return (false);
}

const User &    Users::get_user(int fd) { // use check user before calling this !!
    for (int i = 0; i < users.size(); ++i)
    {
        if (users[i].get_fd() == fd)
            return (users[i]);
    }
    throw std::runtime_error("user not found");
}

void    Users::add_user(int fd) {
    if (!check_user(fd)) {
        User new_user(fd);
        new_user.get_socket_address();
        users.push_back(new_user);
    }
}

void    Users::remove_user(int fd, struct epoll_event event) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    const User & user = get_user(fd);
    users.erase(find(users.begin(), users.end(), user));
    std::cout << "user removed" << std::endl;
}