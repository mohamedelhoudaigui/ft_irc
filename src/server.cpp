#include "../headers/server.hpp"

// canonical form :

Server::Server() {}

Server::Server(int _port, std::string _password):
port(_port),
password(_password),
user_management(),
parser(_password)
{}

const Server & Server::operator=(const Server & other)
{
	if (this != &other)
	{
		this->epoll_fd = other.epoll_fd;
		this->server_fd = other.server_fd;
		this->addr = other.addr;
		this->event = other.event;
		for (int i = 0; i < MAX_EVENTS; ++i)
		{
			this->events[i] = other.events[i];
		}
		this->port = other.port;
		this->password = other.password;
        this->parser = other.parser;
	}

	return (*this);
}


Server::Server(const Server & other)
{
    *this = other;
}

Server::~Server()
{
	close(epoll_fd);
	close(server_fd);
}

// server methods:

void	Server::set_nonblocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void    Server::server_action()
{
    while (true)
    {
        struct sockaddr_in user_addr;
        socklen_t user_len = sizeof(user_addr);
        int user_fd = accept(server_fd, (struct sockaddr*)&user_addr, &user_len);
        if (user_fd == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // no more connections to accept
                break;
            }
            else
            {
                perror("accept");
                break;
            }
        }

        // make user socket non-blocking
        set_nonblocking(user_fd);

        // add user to epoll
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = user_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, user_fd, &event) == -1) {
            perror("epoll_ctl user");
            close(user_fd);
        }
    }
}

void    Server::user_action(struct epoll_event event)
{
    user_management.process(event, parser);
}

void    Server::epoll_loop() {

    while (true)
    {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (n == -1)
        {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < n; i++)
        {
            if (events[i].data.fd == server_fd)
                server_action();
            else
                user_action(events[i]);
        }
    }
}

void	Server::start() {

    // creates the server socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // set socker level and set SO_REUSEADDR option that makes the (ip - port) immediatly usable
	int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    set_nonblocking(server_fd);

    // bind the server socket to (0.0.0.0:port) IP protocol family
	memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // listen with max queue(usually 4096)
	if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // create epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    user_management.set_epoll_fd(epoll_fd);

    // add server socker to epoll instance
	event.events = EPOLLIN | EPOLLET;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl");
        close(server_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

	std::cout << "server listening on port " << port << std::endl;
}
