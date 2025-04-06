#include "../headers/user.hpp"

// canonical form :

User::User(): fd(-1), ip_address("") {}

User::User(int _fd) {
    this->fd = _fd;
    ip_address = get_socket_address();
}

User::User(const User & other) {
	*this = other;
}

const User & User::operator=(const User & other) {
	if (this != &other) {
		this->fd = other.fd;
		this->ip_address = other.ip_address;
	}
	return (*this);
}

bool User::operator==(const User & other) {
    if (this->fd == other.fd)
        return true;
    return false;
}

User::~User() {}

// user methdos:

int	User::get_fd() const {
	return (fd);
}

std::string	User::get_socket_address() const {
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(addr);
    
    // Get local address
    if (getsockname(fd, (struct sockaddr *)&addr, &addr_size) == 0) {
        char local_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr.sin_addr, local_ip, INET_ADDRSTRLEN);
        return (std::string(local_ip));
    }
    return ("");
}