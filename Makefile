NAME = ircserv

SRC = main.cpp  Server.cpp User.cpp

INCLUDE = server.hpp user.hpp

OBJ = $(SRC:%.cpp=%.o)

CPPFALGS =  -std=c++98 -Wall -Wextra -Werror #-g -fsanitize=address

CC = c++

all: $(NAME)

$(NAME): $(OBJ) $(INCLUDE)
	$(CC)   $(CPPFALGS) $(OBJ) -o $@

%.o : %.cpp $(INCLUDE)
	$(CC)  $(CPPFALGS) -c $< -o $@

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(NAME)

re: fclean all
