#include "../headers/server.hpp"
#include "../headers/parser.hpp"

// canonical form :

Server::Server() {}

Server::Server(int _port, std::string _password):
port(_port),
password(_password),
parser(_password, this)
{
    memset(fds, 0, sizeof(fds));
    nfds = 0;
}

const Server & Server::operator=(const Server & other)
{
	if (this != &other)
	{
		this->server_fd = other.server_fd;
		this->addr = other.addr;
        this->nfds = other.nfds;
		memcpy(this->fds, other.fds, sizeof(other.fds));

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
	for (int i = 0; i < nfds; ++i)
    {
        close(fds[i].fd);
    }
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
    struct sockaddr_in user_addr;
    socklen_t user_len = sizeof(user_addr);
    int user_fd = accept(server_fd, (struct sockaddr*)&user_addr, &user_len);
    if (user_fd == -1)
    {
        perror("accept");
        return ;
    }

    set_nonblocking(user_fd);

    if (nfds < MAX_EVENTS)
    {
        fds[nfds].fd = user_fd;
        fds[nfds].events = POLLIN;
        nfds++;
    }
    else
    {
        close(user_fd);
        std::cerr << "Maximum connections reached" << std::endl;
    }
}

void    Server::user_action(struct pollfd event)
{
    parser.process(event);
}

void Server::remove_client(int fd)
{
    for (int i = 0; i < nfds; ++i)
    {
        if (fds[i].fd == fd)
        {
            close(fd);
            if (i != 0)
            {
                fds[i] = fds[nfds - 1];
                fds[nfds - 1].fd = -1;
                nfds--;
            }
            std::cout << "client removed" << std::endl;
            break;
        }
    }
}

void    Server::poll_loop()
{
    while (true)
    {
        int n_events = poll(fds, nfds, -1);
        if (n_events == -1)
        {
            perror("poll");
            break;
        }
        for (int i = 0; i < nfds; ++i)
        {
            if (fds[i].revents == 0)
                continue;

            if (fds[i].revents & (POLLERR | POLLHUP))
            {
                remove_client(fds[i].fd);
                if (i == 0)
                {
                    std::cerr << "Server socket error." << std::endl;
                    return;
                }
                i--;
                continue;
            }

            if (fds[i].fd == server_fd)
            {
                server_action();
            }
            else
            {
                user_action(fds[i]); 
            }
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

    // Initialize pollfd array
    nfds = 1;
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    for (int i = 1; i < MAX_EVENTS; ++i)
        fds[i].fd = -1;

    std::cout << "Server listening on port " << port << std::endl;
    poll_loop();
}
