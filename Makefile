NAME = philo
CC = cc
CFLAGS = -pthread -Wall -Wextra -Werror -g

SRC     = main.c \
          parse_args.c \
          init_simulation.c \
          events.c \
          print.c \
          utils.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)    
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)  

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@  

clean:
	rm -f $(OBJ)          

fclean: clean
	rm -f $(NAME)       

re: fclean all

.PHONY: all clean fclean re libft