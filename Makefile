NAME = cub3d

CC = cc -g3
INC = ./include
LIBFT = ./include/libft/libft.a
LIBMLX = ./include/mlx/libmlx.a
CFLAGS = -Wall -Werror -Wextra -I$(INC)
MLXFLAGS = -framework OpenGL -framework AppKit
INCLUDE = -I 

SRCS = ./src/main.c ./src/window_management.c\

OBJS = $(SRCS:.c=.o)

%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	@$(MAKE) -C ./include/mlx
	@$(MAKE) -C ./include/libft
	@$(CC) $(CFLAGS) $(MLXFLAGS) $(OBJS) $(LIBMLX) $(LIBFT) -o $(NAME)

clean:
	@rm -rf $(OBJS)
	@$(MAKE) clean -C ./include/libft
	@$(MAKE) clean -C ./include/mlx

fclean: clean
	@rm -rf $(NAME)
	@$(MAKE) fclean -C ./include/libft
	@$(MAKE) clean -C ./include/mlx

re: fclean all

.PHONY: all clean fclean re