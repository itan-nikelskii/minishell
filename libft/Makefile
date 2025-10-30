# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/24 16:21:57 by acossari          #+#    #+#              #
#    Updated: 2025/10/30 15:45:26 by inikelsk         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Name of the static library to create
NAME	=	libft.a

# Compiler to use
CC	=	cc

# Compiler flags:
#   -Wall   : enable all warnings
#   -Wextra : enable extra warnings
#   -Werror : treat warnings as errors
CFLAGS	=	-Wall -Wextra -Werror

# Mandatory source files (.c)
SRCS	=	src/ft_isalpha.c src/ft_isdigit.c src/ft_isalnum.c src/ft_isascii.c \
	src/ft_isprint.c src/ft_strlen.c src/ft_memset.c src/ft_bzero.c \
	src/ft_memcpy.c src/ft_memmove.c src/ft_strlcpy.c src/ft_strlcat.c \
	src/ft_toupper.c src/ft_tolower.c src/ft_strchr.c src/ft_strrchr.c \
	src/ft_strcmp.c src/ft_strncmp.c src/ft_memchr.c src/ft_memcmp.c src/ft_strnstr.c \
	src/ft_atoi.c src/ft_calloc.c src/ft_strdup.c src/ft_substr.c \
	src/ft_strjoin.c src/ft_strtrim.c src/ft_split.c src/ft_itoa.c \
	src/ft_strmapi.c src/ft_striteri.c src/ft_putchar_fd.c src/ft_putstr_fd.c \
	src/ft_putendl_fd.c src/ft_putnbr_fd.c src/ft_isspace.c src/ft_realloc.c \
	src/get_next_line/ft_get_next_line.c src/get_next_line/ft_get_next_line_utils.c \

# Object files (.o) for mandatory sources
OBJS	=	$(SRCS:.c=.o)

# Bonus source files (.c) for linked-list functions
BONUS_SRCS	=	src/ft_lstnew_bonus.c src/ft_lstadd_front_bonus.c src/ft_lstsize_bonus.c \
	src/ft_lstlast_bonus.c src/ft_lstadd_back_bonus.c src/ft_lstdelone_bonus.c \
	src/ft_lstclear_bonus.c src/ft_lstiter_bonus.c src/ft_lstmap_bonus.c

# Object files (.o) for bonus sources
BONUS_OBJS	=	$(BONUS_SRCS:.c=.o)

# Default target: builds the library when you run `make`
all:	$(NAME)

# Automatic variables:
#	$@ expands to the current target
#	$^ expands to all prerequisites
#	$< expands to the first prerequsite

# Rule to create the library archive from object files
#	r: replace or insert files into the archive
#	c: create the archive if it doesn’t exist
#	s: write an index (symbol table) for faster linking
$(NAME):	$(OBJS)
	ar rcs $@ $^

# Rule to build the library including bonus objects
bonus:	$(OBJS) $(BONUS_OBJS)
	ar rcs $(NAME) $^

# Pattern rule: compile any .c into corresponding .o
#	% is make's pattern metacharacter capturing the "stem" (base filename)
#	for each stem.c it generates stem.o in a 1:1 correspondence

# Compiler flags:
#	-c: compile only: generate .o files without linking
#	-o: specify output file name for the generated file
%.o:	%.c include/libft.h
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

# Remove all object files (mandatory and bonus)
# 	-f: suppresses errors if files are missing
clean:
	rm -f $(OBJS) $(BONUS_OBJS)

# Remove object files and the library
fclean:	clean
	rm -f $(NAME)

# Rebuild everything from scratch
re:	fclean all

# Declare phony targets (not real files)
.PHONY:	all clean fclean re bonus
