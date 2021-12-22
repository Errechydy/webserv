NAME = webserv

SRCS = main.cpp MySocket.cpp SocketData.cpp ResponseBuilder.cpp ConfigFile.cpp Cgi.cpp Tools.cpp

FLAGS = -Wall -Wextra -Werror -g3 -fsanitize=address

all: $(NAME)

$(NAME) :
	clang++ -std=c++98 $(FLAGS) $(SRCS) -o $(NAME)

debug :
	clang++ -std=c++98 -g  $(SRCS) -o $(NAME)

clean:

fclean:
	rm -rf $(NAME)

re :fclean all
