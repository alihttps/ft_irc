NAME = ircserv

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I./Includes

SRC = main.cpp \
      Server/server.cpp \
      Client/Client.cpp \
      Channel/Channel.cpp \
      Client/Commands/PASS.cpp \
      Client/Commands/NICK.cpp \
      Client/Commands/USER.cpp \
      Client/Commands/JOIN.cpp \
      Client/Commands/PART.cpp \
      Client/Commands/PRIVMSG.cpp \
      Client/Commands/KICK.cpp \
      Client/Commands/INVITE.cpp \
      Client/Commands/TOPIC.cpp \
      Client/Commands/MODE.cpp \
      Client/Commands/QUIT.cpp \
      Client/Commands/PING.cpp

OBJ = $(SRC:.cpp=.o)

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all
