#include "../headers/server.hpp"

int main ()
{
	Server a(6666, "password");
	a.start();
	a.epoll_loop();

	return (0);
}