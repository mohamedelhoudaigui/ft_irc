#include "../headers/server.hpp"
#include "../headers/utils.hpp"

void handle_shutdown_signal(int sig)
{
    printf("Received signal %d, shutting down gracefully...\n", sig);
    exit(1);
}

void handle_pipe_signal(int sig)
{
    printf("Broken pipe detected (signal %d)\n", sig);
    exit(2);
}

void    signals_handler()
{
    struct sigaction sa;

    sa.sa_handler = handle_shutdown_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction(SIGINT)");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        perror("sigaction(SIGQUIT)");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = handle_pipe_signal;
    if (sigaction(SIGPIPE, &sa, NULL) == -1)
    {
        perror("sigaction(SIGPIPE)");
        exit(EXIT_FAILURE);
    }
}

int main (int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "usage : ./ircserv port password" << std::endl;
		exit(1);
	}

	signals_handler();
    valid_password(av[2]);
	Server a(parse_port(av[1]), av[2]);
	a.start();
	a.poll_loop();

	return (0);
}
