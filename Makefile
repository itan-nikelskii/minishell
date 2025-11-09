# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/13 09:23:01 by acossari          #+#    #+#              #
#    Updated: 2025/11/09 19:42:04 by antoniocoss      ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
#                                  TARGETS                                     #
# **************************************************************************** #

NAME = minishell

# **************************************************************************** #
#                              COMPILER CONFIG                                 #
# **************************************************************************** #

CC = cc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -lreadline

# AddressSanitizer support (use 'make asan' or 'make SANITIZE=1')
ifdef SANITIZE
    CFLAGS += -fsanitize=address -g3
    LDFLAGS += -fsanitize=address
endif

# **************************************************************************** #
#                               DIRECTORIES                                    #
# **************************************************************************** #

SRCDIR = src
INCDIR = include
OBJDIR = obj
LIBFTDIR = libft

# **************************************************************************** #
#                              SOURCE FILES                                    #
# **************************************************************************** #

# Builtin sources
BUILTIN_SRCS = $(SRCDIR)/builtins/builtin_dispatcher.c \
               $(SRCDIR)/builtins/cd.c \
               $(SRCDIR)/builtins/echo.c \
               $(SRCDIR)/builtins/env.c \
               $(SRCDIR)/builtins/exit.c \
               $(SRCDIR)/builtins/export.c \
               $(SRCDIR)/builtins/pwd.c \
               $(SRCDIR)/builtins/unset.c

# Execution sources
EXEC_SRCS = $(SRCDIR)/execution/continuation.c \
            $(SRCDIR)/execution/executor.c \
            $(SRCDIR)/execution/hd_expand.c \
            $(SRCDIR)/execution/hd_prepare.c \
            $(SRCDIR)/execution/pipeline.c \
            $(SRCDIR)/execution/redir_backup.c \
            $(SRCDIR)/execution/redir_setup.c \
            $(SRCDIR)/execution/signals.c

# Parser sources
PARSER_SRCS = $(SRCDIR)/parser/buffer.c \
              $(SRCDIR)/parser/build_command.c \
              $(SRCDIR)/parser/cleanup_parser.c \
              $(SRCDIR)/parser/create_word_token.c \
              $(SRCDIR)/parser/expansion.c \
              $(SRCDIR)/parser/parse_quotes.c \
              $(SRCDIR)/parser/parse.c \
              $(SRCDIR)/parser/pipes_parser.c \
              $(SRCDIR)/parser/redirection.c \
              $(SRCDIR)/parser/structs_and_tokens.c \
              $(SRCDIR)/parser/tokenize.c \
              $(SRCDIR)/parser/utils_parser.c \
              $(SRCDIR)/parser/utils_string.c \

# Utils sources
UTILS_SRCS = $(SRCDIR)/utils/array_utils.c \
             $(SRCDIR)/utils/env_utils.c \
             $(SRCDIR)/utils/error_utils.c \
             $(SRCDIR)/utils/execve_utils.c \
             $(SRCDIR)/utils/exit_utils.c \
             $(SRCDIR)/utils/export_utils.c \
             $(SRCDIR)/utils/generic_utils.c \
             $(SRCDIR)/utils/hd_expand_utils.c \
             $(SRCDIR)/utils/hd_utils.c \
             $(SRCDIR)/utils/path_utils.c \
             $(SRCDIR)/utils/pipeline_utils.c \
             $(SRCDIR)/utils/search_utils.c \
             $(SRCDIR)/utils/shell_utils.c \
             $(SRCDIR)/utils/xenv_utils.c

# Main source
MAIN_SRC = $(SRCDIR)/main.c

# All sources
SRCS = $(EXEC_SRCS) $(BUILTIN_SRCS) $(UTILS_SRCS) $(PARSER_SRCS) $(MAIN_SRC)

# **************************************************************************** #
#                              OBJECT FILES                                    #
# **************************************************************************** #

# Transform source paths to object paths in obj/ directory
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)

# **************************************************************************** #
#                                LIBRARIES                                     #
# **************************************************************************** #

LIBFT = $(LIBFTDIR)/libft.a

# **************************************************************************** #
#                            COMPILATION FLAGS                                 #
# **************************************************************************** #

INCLUDES = -I$(INCDIR) -I$(LIBFTDIR)/include

# **************************************************************************** #
#                            HEADER DEPENDENCIES                               #
# **************************************************************************** #

HEADERS = $(INCDIR)/minishell.h $(INCDIR)/parser.h

# **************************************************************************** #
#                                 BUILD RULES                                  #
# **************************************************************************** #

# Default target
all: $(LIBFT) $(NAME)

# Build with AddressSanitizer
asan:
	$(MAKE) fclean
	$(MAKE) all SANITIZE=1

# Build minishell executable
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(LDFLAGS) -o $(NAME)

# **************************************************************************** #
#                            COMPILATION RULES                                 #
# **************************************************************************** #

# Pattern rule: compile any .c file to .o in obj/ directory
$(OBJDIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# **************************************************************************** #
#                              LIBRARY RULES                                   #
# **************************************************************************** #

# Build libft library
$(LIBFT):
	$(MAKE) -C $(LIBFTDIR)

# **************************************************************************** #
#                               CLEAN RULES                                    #
# **************************************************************************** #

# Remove object files and obj directory
clean:
	rm -rf $(OBJDIR)
	$(MAKE) clean -C $(LIBFTDIR)

# Remove object files and executables
fclean: clean
	rm -f $(NAME)
	$(MAKE) fclean -C $(LIBFTDIR)

# Rebuild everything from scratch
re: fclean all

# **************************************************************************** #
#                                 PHONY TARGETS                                #
# **************************************************************************** #

.PHONY: all clean fclean re asan
