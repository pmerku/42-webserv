NAME=not-apache
INCLUDES=./includes
FLAGS=-Wall -Wextra -Werror -std=c++98 -pedantic-errors
CC=clang++

HEADERS=

FILES=\
	main.cpp

FILES:=$(addprefix ./src/, $(FILES))
HEADERS:=$(addprefix ./src/includes/, $(HEADERS))
OBJS=$(FILES:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS) $(HEADERS)
	$(CC) -I $(INCLUDES) $(FLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp $(HEADERS)
	$(CC) -I $(INCLUDES) $(FLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
