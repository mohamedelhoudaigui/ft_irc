#include "../headers/user.hpp"

// canonical form :

User::User():
                    fd(-1),
                    ip_address(""),
                    nick_step(false),
                    user_step(false),
                    pass_step(false),
                    auth(false),
                    user_name(""),
                    real_name(""),
                    nick_name("*"),
                    purge(false)
{}

User::User(int _fd):
                    fd(_fd),
                    ip_address(get_socket_address()),
                    nick_step(false),
                    user_step(false),
                    pass_step(false),
                    auth(false),
                    user_name(""),
                    real_name(""),
                    nick_name("*"),
                    purge(false)
             
{}

User::User(const User & other)
{
	*this = other;
}

const User & User::operator=(const User & other)
{
	if (this != &other) {
		this->fd = other.fd;
		this->ip_address = other.ip_address;
        this->auth = other.auth;
        this->nick_step = other.nick_step;
        this->user_step = other.user_step;
        this->pass_step = other.pass_step;
        this->user_name = other.user_name;
        this->real_name = other.real_name;
        this->nick_name = other.nick_name;
        this->user_buffer = other.user_buffer;
        this->purge = other.purge;
	}

	return (*this);
}

bool User::operator==(const User & other)
{
    if (this->fd == other.fd)
        return true;
    return false;
}

User::~User() {}

// user methdos:

void        User::send_reply(std::string reply)
{   
    ssize_t bytes_sent = send(fd, reply.c_str(), reply.size(), 0);
    if (bytes_sent == -1)
        perror("send");
}

std::string	User::get_socket_address()
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

int	User::get_fd()
{
    return (fd);
}

void		User::add_to_buffer(char* buffer, ssize_t bytes_recv)
{
    user_buffer.append(buffer, bytes_recv);
}

void			User::set_buffer(std::string buffer)
{
    this->user_buffer = buffer;
}

std::string 	User::get_buffer()
{
    return (this->user_buffer);
}

void    User::clear_buffer()
{
    this->user_buffer.clear();
}


bool    User::get_purge()
{
    return (purge);
}

void    User::set_purge(bool s)
{
    this->purge = s;
}

std::string User::get_host_name()
{
    return (this->ip_address);
}

std::string User::get_nick_name()
{
    return (this->nick_name);
}

void    User::set_nick_name(std::string nick)
{
    this->nick_name = nick;
}

std::string User::get_real_name()
{
    return (this->real_name);
}

void    User::set_real_name(std::string real)
{
    this->real_name = real;
}

std::string User::get_user_name()
{
    return (this->user_name);
}

void    User::set_user_name(std::string username)
{
    this->user_name = username;
}

bool    User::get_auth()
{
    return (this->auth);
}

void    User::set_auth(bool a)
{
    this->auth = a;
}

std::string User::get_ip_address()
{
    return ip_address;
}


bool		User::get_nick_step()
{
    return (this->nick_step);
}

void		User::set_nick_step(bool a)
{
    this->nick_step = a;
}

bool		User::get_user_step()
{
    return (this->user_step);
}

void		User::set_user_step(bool a)
{
    this->user_step = a;
}

bool		User::get_pass_step()
{
    return (this->pass_step);
}

void		User::set_pass_step(bool a)
{
    this->pass_step = a;
}

