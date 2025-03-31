#include "../headers/user.hpp"

// canonical form :


User::User() {}

User::User(int _fd): fd(_fd) {}


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

void	User::get_socket_address() const {
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(addr);
    
    // Get local address
    if (getsockname(fd, (struct sockaddr *)&addr, &addr_size) == 0) {
        char local_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr.sin_addr, local_ip, INET_ADDRSTRLEN);
        printf("Local IP: %s, Port: %d\n", local_ip, ntohs(addr.sin_port));
    }
    
    // Get peer (remote) address - for connected sockets
    if (getpeername(fd, (struct sockaddr *)&addr, &addr_size) == 0) {
        char remote_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr.sin_addr, remote_ip, INET_ADDRSTRLEN);
        printf("Remote IP: %s, Port: %d\n", remote_ip, ntohs(addr.sin_port));
    }
}