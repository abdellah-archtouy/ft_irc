NAME = ircserv

NAME_B = Bot

SRC =	main.cpp \
		Commands.cpp \
		Server.cpp \
		User.cpp \
		Commands/join.cpp \
		Commands/privmsg.cpp \
		Commands/mode.cpp

SRC_B = Bot.cpp

INCLUDE = server.hpp user.hpp Channels.hpp 

INCLUDE_B = Bot.hpp

OBJ = $(SRC:%.cpp=%.o)

OBJ_B = $(SRC_B:%.cpp=%.o)

CPPFALGS =  -std=c++98 -Wall -Wextra -Werror# -g -fsanitize=address

CC = c++

all: $(NAME)

$(NAME): $(OBJ) $(INCLUDE)
	$(CC)   $(CPPFALGS) $(OBJ) -o $@

%.o : %.cpp $(INCLUDE)
	$(CC)  $(CPPFALGS) -c $< -o $@

bonus : $(OBJ_B) $(INCLUDE_B)
	$(CC)   $(CPPFALGS) $(OBJ_B) -o $(NAME_B)

clean:
	@rm -rf $(OBJ)
	@rm -rf $(OBJ_B)

fclean: clean
	rm -rf $(NAME)
	rm -rf $(NAME_B)

re: fclean all
