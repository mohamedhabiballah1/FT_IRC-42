NAME = ircserv
CC = c++
SRC = src/main.cpp src/Server.cpp src/ParseMessage.cpp src/Commands.cpp  \
src/Client.cpp src/CommandsTools.cpp src/Channel.cpp src/ChannelTools.cpp src/OperatorsCommands.cpp \
src/Bot.cpp
OBJ = $(SRC:.cpp=.o)
RM = rm -f
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g
LIB = include/Server.hpp include/ParseMessage.hpp include/Client.hpp \
include/Channel.hpp

all :$(NAME)

$(NAME) :$(OBJ)
	$(CC) $(OBJ) -o $(NAME) -g
	
%.o : %.cpp $(LIB)
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)
	
fclean: clean
	$(RM) $(NAME)
re : fclean all