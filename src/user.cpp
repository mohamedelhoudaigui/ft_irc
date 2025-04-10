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
        this->user_buffer = other.user_buffer;
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

std::string	User::get_socket_address() const
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(addr);
    
    if (getsockname(fd, (struct sockaddr *)&addr, &addr_size) == 0) {
        char local_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr.sin_addr, local_ip, INET_ADDRSTRLEN);
        return (std::string(local_ip));
    }
    return ("error getting address");
}

void		User::add_to_buffer(char* buffer)
{
    std::string new_data(buffer);

    if (user_buffer.size() + new_data.size() > BUFFER_SIZE)
        user_buffer.clear();

    user_buffer.append(buffer);

    if (user_buffer.size() > BUFFER_SIZE)
        user_buffer = user_buffer.substr(user_buffer.size() - BUFFER_SIZE);
}

std::string &	User::get_buffer()
{
    return (this->user_buffer);
}

void    User::clear_buffer() {
    this->user_buffer.clear();
}