SRCS = ./src/server.cpp \
		./src/users.cpp \
		./src/user.cpp \
		./src/utils.cpp \
		./src/main.cpp

HEADERS = ./headers/server.hpp \
			./headers/users.hpp \
			./headers/utils.hpp \
			./headers/user.hpp

OBJ_DIR = objects

OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRCS))

CC = c++

RM = rm -f

CFLAGS = -std=c++98 -Wall -Wextra -Werror -O3 -g

NAME = ircserv

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: clean