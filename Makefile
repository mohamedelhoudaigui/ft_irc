SRCS = ./src/server.cpp \
		./src/parser.cpp \
		./src/user.cpp \
		./src/utils.cpp \
		./src/main.cpp \
		./src/channels.cpp

BONUS_SRCS = ./src/bot.cpp

HEADERS = ./headers/server.hpp \
			./headers/utils.hpp \
			./headers/parser.hpp \
			./headers/replys.hpp \
			./headers/user.hpp \
			./headers/channels.hpp

BONUS_HEADERS = ./headers/bot.hpp

OBJ_DIR = objects

OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRCS))
BONUS_OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(BONUS_SRCS))

CC = c++

RM = rm -f

CFLAGS = -std=c++98 -Wall -Wextra -Werror -O3 -fsanitize=address -g3

NAME = ircserv
BONUS_NAME = ircbot

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

bonus: all $(BONUS_NAME)

$(BONUS_NAME): $(BONUS_OBJS)
	$(CC) $(CFLAGS) -o $(BONUS_NAME) $(BONUS_OBJS)

$(BONUS_OBJ_DIR)/%.o: %.cpp $(BONUS_HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME) $(BONUS_NAME)

re: fclean all

.PHONY: all bonus clean fclean re