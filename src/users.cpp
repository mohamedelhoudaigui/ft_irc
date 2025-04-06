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
	for (size_t i = 0 ; i < this->users.size(); ++i)
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
    
        switch (bytes_recv)
        {
            case -1:
                if (errno == EAGAIN || errno == EWOULDBLOCK) { // no more data to read
                    break;
                } else {
                    perror("recv");
                    remove_user(user_fd);
                    break;
                }
    
            case 0:
                remove_user(user_fd);
                break;
    
            default:
                // parse user command here :
                parse_command(user, std::string(buffer));
                std::cout << buffer;

                ssize_t bytes_sent = send(user_fd, "ack!\n", 6, 0);
    
                if (bytes_sent == -1) {
                    perror("send");
                    remove_user(user_fd);
                    break;
                }

        }
    }
}

// parsing command :
void    Users::parse_command(const User & user, std::string buffer)
{
    (void)user;
    (void)buffer;
    // not implemented yet
}


// operations on users vector :

bool    Users::check_user(int fd) {
    for (size_t i = 0; i < users.size(); ++i)
    {
        if (users[i].get_fd() == fd)
            return (true);
    }
    return (false);
}

const User &    Users::get_user(int fd) { // use check user before calling this !!
    for (size_t i = 0; i < users.size(); ++i)
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

void    Users::remove_user(int fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    const User & user = get_user(fd);
    users.erase(find(users.begin(), users.end(), user));
    // std::cout << "user removed" << std::endl;
}