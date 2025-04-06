#include "../headers/server.hpp"
#include "../headers/utils.hpp"

int main (int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "usage : ./ircserv port password" << std::endl;
		exit(1);
	}

	Server a(parse_port(av[1]), av[2]);
	a.start();
	a.epoll_loop();

	return (0);
}