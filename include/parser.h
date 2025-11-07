/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 13:36:27 by inikelsk          #+#    #+#             */
/*   Updated: 2025/11/07 09:33:19 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include <ctype.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include "../libft/include/libft.h"

/* ************************************************************************** */
/*                              STRUCTURES                                    */
/* ************************************************************************** */

/* Token types: pipe = |, in = <, out = >, apend = >>, heredoc = << */
typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC
}	t_token_type;

/* Token structure:
 * type = type of token
 * text = token as a string
 * was_quoted = flag for HEREDOC expansion control (only expand w/o quotes)
 * next = pointer to the next t_token node */
typedef struct s_token
{
	t_token_type	type;
	char			*text;
	bool			was_quoted;
	struct s_token	*next;
}	t_token;

/* A small holder for head/tail so callers don't pass two pointers */
typedef struct s_token_list
{
	t_token	*head;
	t_token	*tail;
}	t_token_list;

/* Redirection entry:
 * type = type of redirection token;
 * target = filename (or heredoc delimiter) string;
 * was_quoted = flag for HEREDOC expansion control (only expand w/o quotes);
 * hd_path = NEW! heredoc temp file path. */
typedef struct s_redir
{
	t_token_type	type;
	char			*target;
	bool			was_quoted;
	char			*hd_path;
	struct s_redir	*next;
}	t_redir;

/* Command node (in a pipeline):
 * argv = array of commands as strings
 * redirs = pointer to a redirection node
 * next = pointer to the next t_command node */
typedef struct s_command
{
	char				**argv;
	t_redir				*redirs;
	struct s_command	*next;
}	t_command;

/* Final parser result returned from parse():
 * commands = pointer to commands ready for execution (NULL if none or on error)
 * error = non-NULL on error (caller frees)
 * incomplete_pipe = true if input ended with a single trailing pipe */
typedef struct s_parse_result
{
	t_command	*commands;
	char		*error;
	bool		incomplete_pipe;
}	t_parse_result;

/* A dynamic buffer struct to store buffer pointer, length, and cap. */
typedef struct s_dynamic_buf
{
	char	*buf;
	size_t	len;
	size_t	cap;
}	t_dyn_buf;

/* Shell state struct */
typedef struct s_shell
{
	char	**envp;
	char	**xenv;
	int		last_exit_status;
	int		stdin_backup;
	int		stdout_backup;
	bool	interactive;
	bool	in_child;
}	t_shell;

/* Tokenizer-specific context: groups token list, shell and error pointer 
so that tokenizer-local helpers stay within the 4-argument limit. */
typedef struct s_tok_context
{
	t_token_list	list;
	t_shell			*shell;
	char			**error;
}	t_tok_context;

/* ************************************************************************** */
/*                                 PROTOTYPES                                 */
/* ************************************************************************** */

/* Basic utils */
int			append_str(t_dyn_buf *dynamic_buf, const char *s);
int			append_char(t_dyn_buf *dynamic_buf, char c);
int			is_word_char(char c);
char		*strdup_range(const char *s, size_t from, size_t to);
char		*get_exit_status_str(t_shell *shell);
void		init_tok_context(t_tok_context *ctx, t_shell *shell, char **error,
				size_t *i);

/* Buffer utils */
t_dyn_buf	*dynbuf_create_and_init(void);
int			ensure_buffer_capacity(t_dyn_buf *dynamic_buf, size_t need);

/* Expansion helper */
int			expand_dollar(const char *s, size_t *j, t_dyn_buf *buf,
				t_shell *shell);

/* Parse specific parts */
int			parse_single_quote(const char *s, size_t *i, char **out);
int			parse_double_quote(const char *s, size_t *i, char **out,
				t_shell *shell);
int			parse_unquoted_word(const char *s, size_t *i, char **out,
				t_shell *shell);
int			deal_with_pipes(t_token **tokens, char **error, bool *incomplete);

/* Tokenize helpers */
t_redir		*new_redir(t_token_type type, char *target, bool was_quoted);
t_token		*new_token(t_token_type type, char *text);
int			append_token(t_token_list *list, t_token *node);
int			create_token_pipe(size_t *i, t_token_list *list);
int			create_token_redirection(const char *line, size_t *i,
				t_token_list *list);
int			create_token_quote_or_word(const char *line, size_t *i,
				t_token_list *list, t_shell *shell);

/* Top-level tokenize & command building */
t_token		*tokenize(const char *line, char **error, t_shell *shell);
t_command	*build_command_from_tokens(t_token **tp, char **error);
int			consume_redirection_target(t_token *token, t_command *cmd);

/* Top-level parse entrypoint */
int			parse(const char *line, t_parse_result *result, t_shell *shell);

/* Cleanup */
void		free_tokens(t_token *t);
void		free_commands(t_command *cmd);
void		dynamic_buf_free(t_dyn_buf *dynamic_buf);

/* Necessary declarations defined in minishell.h */
char		*get_env_value(char *key, char **envp);

#endif
