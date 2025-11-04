# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/13 09:23:01 by acossari          #+#    #+#              #
#    Updated: 2025/11/04 10:01:34 by antoniocoss      ###   ########.fr        #
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

# Execution sources
EXEC_SRCS = $(SRCDIR)/execution/executor.c \
            $(SRCDIR)/execution/redirections.c \
            $(SRCDIR)/execution/redirections_backup.c \
            $(SRCDIR)/execution/pipeline.c \
            $(SRCDIR)/execution/heredoc_expand.c \
            $(SRCDIR)/execution/heredoc_prepare.c \
            $(SRCDIR)/execution/continuation.c

# Builtin sources
BUILTIN_SRCS = $(SRCDIR)/builtins/builtin_dispatcher.c \
               $(SRCDIR)/builtins/pwd.c \
               $(SRCDIR)/builtins/echo.c \
               $(SRCDIR)/builtins/env.c \
               $(SRCDIR)/builtins/cd.c \
               $(SRCDIR)/builtins/export.c \
               $(SRCDIR)/builtins/unset.c \
               $(SRCDIR)/builtins/exit.c

# Utils sources
UTILS_SRCS = $(SRCDIR)/utils/cmd_utils.c \
             $(SRCDIR)/utils/memory_utils.c \
             $(SRCDIR)/utils/path_resolver.c \
             $(SRCDIR)/utils/error_handler.c \
             $(SRCDIR)/utils/process_utils.c \
             $(SRCDIR)/utils/execve_utils.c \
             $(SRCDIR)/utils/env_utils.c \
             $(SRCDIR)/utils/export_utils.c \
             $(SRCDIR)/utils/xenv_utils.c \
             $(SRCDIR)/utils/array_utils.c \
             $(SRCDIR)/utils/search_utils.c \
             $(SRCDIR)/utils/exit_utils.c \
             $(SRCDIR)/utils/heredoc_utils.c \
             $(SRCDIR)/utils/heredoc_expand_utils.c \
             $(SRCDIR)/utils/shell_utils.c \
             $(SRCDIR)/utils/input_utils.c \
             $(SRCDIR)/utils/pipeline_utils.c \

# Signal sources
SIGNAL_SRCS = $(SRCDIR)/signals/signals.c \
              $(SRCDIR)/signals/signals_parent_ps1.c \
              $(SRCDIR)/signals/signals_parent_wait.c \
              $(SRCDIR)/signals/signals_child_ps2.c

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
              $(SRCDIR)/parser/utils_parser.c

# Main source
MAIN_SRC = $(SRCDIR)/main.c

# All sources
SRCS = $(EXEC_SRCS) $(BUILTIN_SRCS) $(UTILS_SRCS) $(SIGNAL_SRCS) \
       $(PARSER_SRCS) $(MAIN_SRC)

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

.PHONY: all clean fclean re
