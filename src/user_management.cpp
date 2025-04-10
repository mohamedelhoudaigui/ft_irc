#include "../headers/user_management.hpp"

// canonical form :

UserManag::UserManag() {}

UserManag::UserManag(const UserManag & other) {
	*this = other;
}

const UserManag & UserManag::operator=(const UserManag & other) {
	if (this != &other)
    {
		this->users = other.users;
	}
	return (*this);
}

UserManag::~UserManag()
{
	for (size_t i = 0 ; i < this->users.size(); ++i)
    {
        close(users[i].get_fd());
    }
}

// UserManag methods:

void    UserManag::set_epoll_fd(int _epoll_fd) {
    epoll_fd = _epoll_fd;
}

void	UserManag::process(struct epoll_event event) {

    int user_fd = event.data.fd;

    add_user(user_fd);
    User & user = get_user(user_fd);

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
                user.add_to_buffer(buffer);
                process_buffer(user);
                break ;
        }
    }
}

// parsing command :
void    UserManag::process_buffer(User & user)
{
    if (parser.parse(user) == false)
    {
        remove_user(user.get_fd());
    }
}


// operations on UserManag vector :

bool    UserManag::check_user(int fd) {
    for (size_t i = 0; i < users.size(); ++i)
    {
        if (users[i].get_fd() == fd)
            return (true);
    }
    return (false);
}

User &    UserManag::get_user(int fd) { // use check user before calling this !!
    for (size_t i = 0; i < users.size(); ++i)
    {
        if (users[i].get_fd() == fd)
            return (users[i]);
    }
    throw std::runtime_error("user not found");
}

void    UserManag::add_user(int fd) {
    if (!check_user(fd)) {
        User new_user(fd);
        new_user.get_socket_address();
        users.push_back(new_user);
    }
}

void    UserManag::remove_user(int fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    const User & user = get_user(fd);
    users.erase(find(users.begin(), users.end(), user));
    // std::cout << "user removed" << std::endl;
}