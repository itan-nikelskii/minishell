/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 09:12:43 by inikelsk          #+#    #+#             */
/*   Updated: 2025/10/27 11:45:53 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TODO:
// 0. FIXME:
// 		1. ctrl-\ is supposed to od nothing, but rn it gives a segfault
// 		2. [FIXED] | echo hello (leading pipe should be an error, but rn it's not)
//		3. [FIXED] echo hello || echo world (multiple pipes should be an error, but rn it's not)
//      4. really not sure about parse_single/double/unquoted functions; double check the differences;
//		   maybe create a central parse_ function and separate single/double/unquoted since all these
//		   are using the same parameters and variables
//		5. Check the error messages and choose whether to follow bash or have own (consistent!) system
//		6. Go over all functions and check that everything is free'd if something fails partway
//
// 1. Double check allowed functions (memcpy, realloc, etc. used but not in subject; write own/convert to ft_ versions)
// 2. Adjust main to check for leaks with valgrind without force interrupting the program
// 3. Organize into files (for Makefile, figure out why -lreadline is necessary for cc)
// 4. Make local helpers static
// 5. Proper documentation (should it go in .h or in .c files?)
// 6. Check if all includes are actually used
// 7. Norminette

#define _POSIX_C_SOURCE 200809L	// necessary for sa; TODO: theory

#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Global to store received signal number (the only global as per the subject).
- sig_atomic_t is an integer type guaranteed to be read/written atomically
  in respect to signals (=access won't be torn into partial bytes by an
  interrupting signal handler).
- volatile tells the compiler not to assume that the value stays unchanged;
  the compiler must actually load/store the variable every time it is accessed.
- static because the norm requires it. */
static volatile sig_atomic_t	g_signal_received = 0;

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

/* Redirection entry:
 * type = type of redirection token
 * target = filename (or heredoc delimiter) string
 * was_quoted = flag for HEREDOC expansion control (only expand w/o quotes) */
typedef struct s_redir
{
	t_token_type	type;
	char			*target;
	bool			was_quoted;
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
}	t_dynamic_buf;

/* ========================= Prototypes ========================= */	
// TODO: check if all are here; organize into groups; move to .h together with includes and structs

/* basic utils */
char		*strdup_range(const char *s, size_t from, size_t to);
int			is_word_char(char c);
t_token		*new_token(t_token_type type, char *text);
int			append_token(t_token **head, t_token **tail, t_token *node);
t_redir		*new_redir(t_token_type type, char *target, bool was_quoted);

/* Shell struct definition; TODO: figure out what's what and document */
typedef struct s_shell
{
	char	**envp;
	char	**xenv;
	int		last_exit_status;
	int		stdin_backup;
	int		stdout_backup;
}	t_shell;

/* env helpers */
// Using executor's get_env_value() instead
extern char	*get_env_value(char *key, char **envp);
// Now can access shell->last_exit_status
char		*get_exit_status_str(t_shell *shell);

/* buffer/expansion helpers */
int			ensure_buffer_capacity(t_dynamic_buf *dynamic_buf, size_t need);
int			append_char(t_dynamic_buf *dynamic_buf, char c);
int			append_str(t_dynamic_buf *dynamic_buf, const char *s);
char		*expand_variable(const char *s, size_t index, size_t *j, char **envp);

/* parse small parts */
int			parse_single_quote(const char *s, size_t *i, char **out);
int			parse_double_quote(const char *s, size_t *i, char **out, t_shell *shell);
int			parse_unquoted_word(const char *s, size_t *i, char **out, t_shell *shell);
int			expand_dollar(const char *s, size_t *j, t_dynamic_buf *buf, t_shell *shell);

/* tokenize helpers */
int			create_token_pipe(const char *line, size_t *i, t_token **head, t_token **tail);
int			create_token_redirection(const char *line, size_t *i, t_token **head, t_token **tail);
int			create_token_quote_or_word(const char *line, size_t *i, t_token **head, t_token **tail, t_shell *shell);

/* deal with pipes */
int			validate_pipes(t_token *tokens, char **error);
int			strip_trailing_pipe(t_token **tokens, bool *incomplete);
int			deal_with_pipes(t_token **tokens, char **error, bool *incomplete);

/* top-level tokenize & parse */
t_token		*tokenize(const char *line, char **error, t_shell *shell);
size_t		count_words_in_segment(t_token *t);
int			consume_redirection_target(t_token *token, t_command *cmd);
int			add_word_to_cmd_argv(t_command *cmd, const char *word, size_t *arg_index);
t_command	*init_t_command(t_token *token);
t_command	*build_command_from_tokens(t_token **tp, char **error);			// NEW! (for Itan) TODO: check it out
t_command	*parse_tokens_to_commands(t_token *t, char **error);			// NEW! (for Itan) TODO: check it out

/* top-level parse entrypoint (use in main) */
int			parse(const char *line, t_parse_result *result, t_shell *shell);

/* cleanup */
void		free_tokens(t_token *t);
void		free_commands(t_command *c);
void		dynamic_buf_free(t_dynamic_buf *dynamic_buf);

/* signal */
void		sigint_handler(int sig);

/* ============================ Basic utilities ============================ */

/* Duplicate and return substring s[from ... to]; return NULL on failure. */
char	*strdup_range(const char *s, size_t from, size_t to)
{
	char	*out;
	size_t	len;

	len = to - from;
	out = malloc(len + 1);
	if (!out)
		return (NULL);
	memcpy(out, s + from, len);			// TODO: switch to the ft_ version later
	out[len] = '\0';
	return (out);
}

/* If c is a word char (non-meta or space), return 1; otherwise, return 0. */
int	is_word_char(char c)
{
	if (isspace((unsigned char)c))		// TODO: switch to the ft_ version later
		return (0);
	if (c == '|' || c == '<' || c == '>' || c == '\'' || c == '"')
		return (0);
	return (1);
}

/* Create and return a new token node. */
t_token	*new_token(t_token_type type, char *text)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->text = text;
	token->was_quoted = false;
	token->next = NULL;
	return (token);
}

/* Append token to the linked list. */
int	append_token(t_token **head, t_token **tail, t_token *node)
{
	if (!node)
		return (-1);
	node->next = NULL;
	if (*head == NULL)
	{
		*head = node;
		*tail = node;
	}
	else
	{
		(*tail)->next = node;
		*tail = node;
	}
	return (0);
}

/* Create and return a new redirection node. */
t_redir	*new_redir(t_token_type type, char *target, bool was_quoted)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return NULL;
	redir->type = type;
	redir->target = target;
	redir->was_quoted = was_quoted;
	redir->next = NULL;
	return (redir);
}

/* Create a dynamic buffer and initialize it to default values; on success,
return a pointer to it; on failure, return NULL. */
t_dynamic_buf	*dynbuf_create_and_init(void)
{
	t_dynamic_buf	*dynamic_buf;
	
	dynamic_buf = malloc(sizeof(t_dynamic_buf));
	if (!dynamic_buf)
		return (NULL);
	dynamic_buf->buf = malloc(64);	// FIXME: random magic number, don't know what's best here
	if (!dynamic_buf->buf)
		return (free(dynamic_buf), NULL);
	dynamic_buf->len = 0;
	dynamic_buf->cap = 64;			// FIXME: see the fixme above
	dynamic_buf->buf[0] = '\0';		// null-terminate right away for good measure
	return (dynamic_buf);
}

/* ====================== Variable expansion helpers ====================== */

/* TODO: documentation (buffer[12] because an int in C is typically 32 bits => 
can hold up to 2,147,483,647 (10 digits + sign + \0); ^= is bitwise XOR to
reverse the characters in buffer because the digits were built backwards during
the division loop) */
char	*get_exit_status_str(t_shell *shell)
{
	char	buffer[12];
	int		n;
	int		len;
	int		i;

	if (!shell)
		n = 0;
	else
		n = shell->last_exit_status;
	len = 0;
	if (n == 0)
		buffer[len++] = '0';
	else
	{
		while (n > 0)
		{
			buffer[len++] = (n % 10) + '0';
			n /= 10;
		}
		i = 0;
		while (i < len / 2)
		{
			buffer[i] ^= buffer[len - 1 - i];
			buffer[len - 1 - i] ^= buffer[i];
			buffer[i] ^= buffer[len - 1 - i];
			i++;
		}
	}
	buffer[len] = '\0';
	return (strdup(buffer));					// TODO: turn into ft_ version
}

/* Ensure buffer capacity for 'need' amount of bytes and grow the buffer with
realloc if necessary. Uses:
- append_char() and append_str() rely on this to safely append chars/strings
  without buffer overflow;
- parse_double_quote() and parse_unquoted_word() build their resulting strings
  incrementally (may append literals and then expansions (env vars, $, etc) of 
  unknown size => need a dynamic buffer that can be grown piecewise. */
int	ensure_buffer_capacity(t_dynamic_buf *dynamic_buf, size_t need)
{
	char	*tmp;

	if (dynamic_buf->cap >= need)
		return (0);
	while (dynamic_buf->cap < need)
	{
		dynamic_buf->cap *= 2;
		if (dynamic_buf->cap == 0)
			dynamic_buf->cap = 64;				// FIXME: fix magic numbers
	}
	tmp = realloc(dynamic_buf->buf, dynamic_buf->cap);		// TODO: switch to the ft_ version later (write my own)
	if (!tmp)
		return (-1);
	dynamic_buf->buf = tmp;
	return (0);
}

/* Append char c to the buffer buf; return 0 on sucess, -1 on failure. */
int	append_char(t_dynamic_buf *dynamic_buf, char c)
{
	int	status;

	status = ensure_buffer_capacity(dynamic_buf, dynamic_buf->len + 2);
	if (status != 0)
		return (-1);
	dynamic_buf->buf[dynamic_buf->len] = c;
	dynamic_buf->len += 1;
	dynamic_buf->buf[dynamic_buf->len] = '\0';
	return (0);
}

/* Append string s to buffer buf; return 0 on sucess, -1 on failure. */
int	append_str(t_dynamic_buf *dynamic_buf, const char *s)
{
	size_t	s_len;
	int		status;

	s_len = strlen(s);					// TODO: switch to the ft_ version later
	status = ensure_buffer_capacity(dynamic_buf, dynamic_buf->len + s_len + 1);
	if (status != 0)
		return (-1);
	memcpy(dynamic_buf->buf + dynamic_buf->len, s, s_len);		// TODO: switch to the ft_ version later
	dynamic_buf->len += s_len;
	dynamic_buf->buf[dynamic_buf->len] = '\0';
	return (0);
}

/* TODO: documentation */
char	*expand_variable(const char *s, size_t index, size_t *j, char **envp)
{
	size_t	name_start;
	char	*name;
	char	*value;
	char	*result;

	name_start = index;
	while (isalnum((unsigned char)s[index]) || s[index] == '_')
		index++;
	name = strdup_range(s, name_start, index);
	if (!name)
		return (NULL);
	value = get_env_value(name, envp);
	if (value)
		result = strdup(value);				// TODO: switch to the ft_ version later
	else
		result = strdup("");				// TODO: switch to the ft_ version later
	free(name);
	*j = index;
	return (result);
}

/* TODO: documentation (after proper refactoring for norminette) */
int	expand_dollar(const char *s, size_t *j, t_dynamic_buf *buf, t_shell *shell)
{
	size_t	index;
	char	*expanded;

	index = *j + 1;
	if (s[index] == '?')
	{
		expanded = get_exit_status_str(shell);
		if (!expanded)
			return (-1);
		*j = index + 1;
	}
	else if (isalpha((unsigned char)s[index]) || s[index] == '_')		// TODO: switch to the ft_ version later
	{
		expanded = expand_variable(s, index, j, shell->envp);
		if (!expanded)
			return (-1);
	}
	else
	{
		if (append_char(buf, '$') != 0)
			return (-1);
		*j += 1;
		return (0);
	}
	if (append_str(buf, expanded) != 0)
		return (free(expanded), -1);
	return (free(expanded), 0);
}

/* Parse single quoted literal (no expansion) and store it in *out;
return 0 on sucess, -1 if something went wrong. */
int	parse_single_quote(const char *s, size_t *i, char **out)
{
	size_t	start;
	size_t	j;
	char	*str;

	start = *i + 1;
	j = start;
	while (s[j] != '\0' && s[j] != '\'')
		j++;
	if (s[j] != '\'')
		return (-1);
	str = strdup_range(s, start, j);
	if (!str)
		return (-1);
	*i = j + 1;
	*out = str;
	return (0);
}

/* Parse double-quoted string, allowing for $ expansion, and store it in *out;
return 0 on sucess, -1 if something went wrong. */
int	parse_double_quote(const char *s, size_t *i, char **out, t_shell *shell)
{
	size_t			j;
	t_dynamic_buf	*dynamic_buf;

	dynamic_buf = dynbuf_create_and_init();
	if (!dynamic_buf)
		return (-1);
	j = *i + 1;
	while (s[j] != '\0' && s[j] != '"')
	{
		if (s[j] == '$')
		{
			if (expand_dollar(s, &j, dynamic_buf, shell) != 0)
				return (dynamic_buf_free(dynamic_buf), -1);
			continue ;
		}
		if (append_char(dynamic_buf, s[j]) != 0)
			return (dynamic_buf_free(dynamic_buf), -1);
		j++;
	}
	if (s[j] != '"')
		return (dynamic_buf_free(dynamic_buf), -1);
	*out = dynamic_buf->buf;
	*i = j + 1;
	return (0);
}

/* Parse an unquoted word, allowing for $ expansion, and store it in *out;
return 0 on sucess, -1 if something went wrong. */
int	parse_unquoted_word(const char *s, size_t *i, char **out, t_shell *shell)
{
	size_t			j;
	t_dynamic_buf	*dynamic_buf;

	dynamic_buf = dynbuf_create_and_init();
	if (!dynamic_buf)
		return (-1);
	j = *i;
	while (s[j] != '\0' && is_word_char(s[j]))
	{
		if (s[j] == '$')
		{
			if (expand_dollar(s, &j, dynamic_buf, shell) != 0)
				return (dynamic_buf_free(dynamic_buf), -1);
			continue ;
		}
		if (append_char(dynamic_buf, s[j]) != 0)
			return (dynamic_buf_free(dynamic_buf), -1);
		j++;
	}
	*out = dynamic_buf->buf;
	*i = j;
	return (0);
}

/* Handle the '|' char: create a new pipe token, append it to the token list,
and return 0 on success or -1 on failure. */
int	create_token_pipe(const char *line, size_t *i, t_token **head, t_token **tail)			// FIXME: *line doesn't seem to be used anymore; delete?
{
	t_token	*token;

	// EDITED: Added (void)line to suppress unused parameter warning with -Werror
	(void)line;
	token = new_token(TOKEN_PIPE, strdup("|"));
	if (!token || append_token(head, tail, token) != 0)
		return (-1);
	*i = *i + 1;
	return (0);
}

/* Check for invalid redirection sequences:
 * more than two redir signs in a row;
 * <>, ><;
 * any valid redir sign followed by space(s) and another redir sign.
Return true if found an invalid sequence, false otherwise. */
static bool	is_invalid_redir_sequence(const char *line, size_t i)
{
	size_t	j;
	int		count;
	char	this;
	char	next;

	j = i;
	count = 0;
	this = line[i];
	next = line[i + 1];
	while (line[j] == '<' || line[j] == '>')
	{
		count++;
		j++;
	}
	if (count > 2)
		return (true);
	if ((this == '<' && next == '>') || (this == '>' && next == '<'))
		return (true);
	while (isspace(line[j]))											// TODO: change to ft_ version
		j++;
	if (line[j] == '<' || line[j] == '>')
		return (true);
	return (false);
}

/* Handle the redirection (<, <<, >, >>) chars: determine the redirection type,
create a new token, append it to the token list, and update *i depending on
the number of chars in the redirection type (1 or 2).
Return 0 on success, -1 on malloc failure, or -2 on syntax error. */
int	create_token_redirection(const char *line, size_t *i, t_token **head, t_token **tail)
{
	t_token	*token;

	if (is_invalid_redir_sequence(line, *i))						// NEW! check for redirection syntax errors
		return (-2);
	if (line[*i] == '<')
	{
		if (line[*i + 1] && line[*i + 1] == '<')
			token = new_token(TOKEN_HEREDOC, strdup("<<"));			// TODO: switch to the ft_ version later
		else
			token = new_token(TOKEN_REDIR_IN, strdup("<"));			// TODO: switch to the ft_ version later
	}
	else 	// if (line[*i] == '>')
	{
		if (line[*i + 1] && line[*i + 1] == '>')
			token = new_token(TOKEN_REDIR_APPEND, strdup(">>"));	// TODO: switch to the ft_ version later
		else
			token = new_token(TOKEN_REDIR_OUT, strdup(">"));		// TODO: switch to the ft_ version later
	}
	if (line[*i + 1] && (line[*i + 1] == '<' || line[*i + 1] == '>'))
		*i += 2;
	else
		*i += 1;
	if (!token || append_token(head, tail, token) != 0)
		return (-1);
	return (0);
}

/* Create a token out of an unquoted word or a quoted string. Parse the string
depending on the presence and type of quotes; create a new token and return 0
on success or -1 on failure. */
/* TODO: document new addition: Coalescere segments + use envp and shell for expansion */
int	create_token_quote_or_word(const char *line, size_t *i, t_token **head, t_token **tail, t_shell *shell)
{
	t_dynamic_buf	*buf;
	bool			had_quote;
	char			*segment;
	t_token			*token;

	buf = dynbuf_create_and_init();
	if (!buf)
		return (-1);
	had_quote = false;
	while (line[*i] != '\0' && !isspace((unsigned char)line[*i])			// TODO: turn into ft_ version; TODO: duble check the logic
		&& line[*i] != '|' && line[*i] != '<' && line[*i] != '>')
	{
		segment = NULL;
		if (line[*i] == '\'')
		{
			if (parse_single_quote(line, i, &segment) != 0)
				return (dynamic_buf_free(buf), -1);
			had_quote = true;
		}
		else if (line[*i] == '"')
		{
			if (parse_double_quote(line, i, &segment, shell) != 0)
				return (dynamic_buf_free(buf), -1);
			had_quote = true;
		}
		else
		{
			if (parse_unquoted_word(line, i, &segment, shell) != 0)
				return (dynamic_buf_free(buf), -1);
		}
		if (append_str(buf, segment) != 0)
			return (free(segment), dynamic_buf_free(buf), -1);
		free(segment);
	}
	token = new_token(TOKEN_WORD, buf->buf);
	if (!token)
		return (dynamic_buf_free(buf), -1);
	token->was_quoted = had_quote;
	free(buf);
	if (append_token(head, tail, token) != 0)
		return (free(token->text), free(token), -1);
	return (0);
}

/* Tokenize input line into linked token list */		// FIXME: too long; decide if it should deal with error handling (refactoring will heavily depend on this)
t_token	*tokenize(const char *line, char **error, t_shell *shell)
{
	t_token	*head;
	t_token	*tail;
	size_t	i;
	char	c;
	int		ret_value;

	head = NULL;
	tail = NULL;
	i = 0;
	while (line[i] != '\0')
	{
		c = line[i];
		if (isspace((unsigned char)c))									// TODO: switch to the ft_ version later
		{
			i++;
			continue ;
		}
		if (c == '\\' || c == ';')
		{
			*error = strdup("Unsupported escape or special character");	// TODO: switch to the ft_ version later
			free_tokens(head);
			return (NULL);
		}
		if (c == '|')
		{
			if (create_token_pipe(line, &i, &head, &tail) != 0)
			{
				*error = strdup("malloc failure");						// TODO: switch to the ft_ version later
				free_tokens(head);
				return (NULL);
			}
			continue ;
		}
		if (c == '<' || c == '>')
		{
			ret_value = create_token_redirection(line, &i, &head, &tail);	// NEW! differentiating between two error cases for this func
			if (ret_value != 0)
			{
				if (ret_value == -1)
					*error = strdup("malloc failure");					// TODO: switch to the ft_ version later
				else
					*error = strdup("Syntax error near unexpected redirection token");	// TODO: switch to the ft_ version later
				free_tokens(head);
				return (NULL);
			}
			continue ;
		}
		if (c == '\'' || c == '"' || is_word_char(c))
		{
			if (create_token_quote_or_word(line, &i, &head, &tail, shell) != 0)
			{
				*error = strdup("Parse error in word/quote");			// TODO: switch to the ft_ version later
				free_tokens(head);
				return (NULL);
			}
			continue ;
		}
		i++;
	}
	return (head);
}

/* ===== Deal with pipes: validate, strip trailing pipe (if applicable) ===== */

/* Validate pipe usage in the token list:
	- reject a leading pipe (e.g. "| echo");
	- reject consecutive pipes (e.g. "echo || echo" or "echo | | echo").
On success, return 0; on error, return -1 and set the *error message. */
int	validate_pipes(t_token *tokens, char **error)
{
	t_token *current;

	if (tokens == NULL)
		return (0);
	if (tokens->type == TOKEN_PIPE)		// leading pipe case
	{
		*error = strdup("Syntax error: unexpected '|'");
		return (-1);
	}
	current = tokens;
	while (current)							// consecutive pipe (anywhere) case
	{
		if (current->type == TOKEN_PIPE && current->next && current->next->type == TOKEN_PIPE)
		{
			*error = strdup("Syntax error: unexpected '||'");
			return (-1);
		}
		current = current->next;
	}
	return (0);
}

/* If the last token is a single pipe, unlink and free that token, set 
*incomplete = true and return 0. If no trailing pipe found, set *incomplete 
= false and return 0. Return -1 if either parameter was NULL. 

NOTE on removing the single trailing pipe token from the list: a single trailing
'|' means the user started a pipeline but didn't finish the right-hand command. 
The parser reports this to the execution layer (via the incomplete flag) but 
still returns the already-built command(s). Removing the trailing pipe token 
prevents parse_tokens_to_commands() from attempting to treat an empty command
after the pipe. After the execution layer prompts and gathers more input, the 
combined input can be re-parsed. */
int	strip_trailing_pipe(t_token **tokens, bool *incomplete)
{
	t_token *current;
	t_token *previous;

	*incomplete = false;
	previous = NULL;
	current = *tokens;
	while (current && current->next)	// find last node
	{
		previous = current;
		current = current->next;
	}
	if (current && current->type == TOKEN_PIPE)
	{
		*incomplete = true;		// single trailing pipe found -> unlink and free current
		if (previous)				
			previous->next = NULL;
		else
			*tokens = NULL;		// defensive; leading pipe should have been rejected earlier (TODO: double check)
		free(current->text);
		free(current);
	}
	return (0);
}

/* Check whether the token list contains any pipe tokens; if so, validate pipe
syntax to reject leading/duplicate pipes and strip trailing pipe if applicable.
Return 0 on success (validation passed or no pipes), -1 on error of failure. */
int	deal_with_pipes(t_token **tokens, char **error, bool *incomplete)
{
	t_token	*current;
	bool	has_pipe;

	if (!tokens || !error || !incomplete)
		return (-1);
	if (*tokens == NULL)
		return (0);
	has_pipe = false;
	current = *tokens;
	while (current)			// scan for any pipe token first; skip pipe-related logic if none
	{
		if (current->type == TOKEN_PIPE)
		{
			has_pipe = true;
			break ;
		}
		current = current->next;
	}
	if (!has_pipe)
		return (0);
	if (validate_pipes(*tokens, error) != 0)		// validate pipe usage (leading/double)
		return (-1);
	strip_trailing_pipe(tokens, incomplete);		// strip single trailing pipe if present and set incomplete flag
	return (0);
}

/* ===== Parser: convert tokens into commands with argv and redirections ===== */

/* Count words until a pipe or end of command segment; return count. */
size_t	count_words_in_segment(t_token *token)
{
	size_t	count;

	count = 0;
	while (token != NULL && token->type != TOKEN_PIPE)
	{
		if (token->type == TOKEN_WORD)
			count++;
		token = token->next;
	}
	return (count);
}

/* Add word to cmd argv at arg_index and update it (caller tracks arg_index);
return 0 on success, -1 on failure. */
int	add_word_to_cmd_argv(t_command *cmd, const char *word, size_t *arg_index)
{
	cmd->argv[*arg_index] = strdup(word);				// TODO: switch to the ft_ version later
	if (!cmd->argv[*arg_index])
		return (-1);
	*arg_index += 1;
	return (0);
}

/* Consume the redirection target token and attach to cmd.redirs. On success,
return 0. If the target doesn't exist or is not a TOKEN_WORD type, or if
anything goes wrong with mallocs, return -1. */
int	consume_redirection_target(t_token *token, t_command *cmd)
{
	t_token_type	redir_type;
	char			*target;
	t_redir			*redir;

	if (!token || !token->next)
		return (-1);
	redir_type = token->type;
	token = token->next;	// advance to skip the actual redirection token and get to target
	if (token->type != TOKEN_WORD)
		return (-1);
	target = strdup(token->text);								// TODO: turn into ft_ version
	if (!target)
		return (-1);
	redir = new_redir(redir_type, target, token->was_quoted);
	if (!redir)
	{
		free(target);
		return (-1);
	}
	// FIX: Append to end instead of prepend to head
	// Old: prepend created reversed list (last-typed processed first)
	// New: append maintains correct order (last-typed processed last)
	if (!cmd->redirs)
		cmd->redirs = redir;
	else
	{
		t_redir *last = cmd->redirs;						// FIXME: variable declaration should be on top
		while (last->next)
			last = last->next;
		last->next = redir;
	}
	return (0);
}

/* Initialize a t_command with NULLs and return it, or just NULL on failure. */
t_command *init_t_command(t_token *token)
{
	t_command	*cmd;
	size_t		word_count;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->redirs = NULL;
	cmd->argv = NULL;
	cmd->next = NULL;
	word_count = count_words_in_segment(token);
	cmd->argv = malloc((word_count + 1) * sizeof(char *));
	if (!cmd->argv)
	{
		free(cmd);
		return (NULL);
	}
	return (cmd);
}

/* Build a single command from tokens up to pipe; differentiate between
words and redirections with their targets; return the built command
on success, or NULL on failure. */
/* FIX ISSUE 7: Added char **error parameter for specific error messages */
t_command	*build_command_from_tokens(t_token **tp, char **error)
{
	t_token		*token;
	t_command	*cmd;
	size_t		index;

	token = *tp;
	cmd = init_t_command(token);
	index = 0;
	while (token != NULL && token->type != TOKEN_PIPE)
	{
		if (token->type == TOKEN_WORD)
		{
			if (add_word_to_cmd_argv(cmd, token->text, &index) != 0)
				return (NULL);
		}
		else // must be a redirection token
		{
			/* FIX ISSUE 7: Check if redirection target is missing */
			if (!token->next || token->next->type != TOKEN_WORD)
			{
				if (error)
					*error = strdup("syntax error near unexpected token 'newline'");
				return (NULL);
			}
			if (consume_redirection_target(token, cmd) != 0)
				return (NULL);
			token = token->next;	// skip the target of redirection
		}
		token = token->next;
	}
	cmd->argv[index] = NULL;		// null-terminate the argv array
	*tp = token;
	return (cmd);
}

// FIXME: too long
/* Parse token list into pipeline (linked commands). Set incomplete_pipe = true
if the token list ends with a single trailing pipe (the execution layer checks
the flag to get more input). */
/* FIX ISSUE 7: Added char **error parameter for specific error messages */
t_command	*parse_tokens_to_commands(t_token *t, char **error)
{
	t_command	*head;
	t_command	*tail;
	t_command	*cmd;

	head = NULL;
	tail = NULL;
	while (t != NULL)
	{
		if (t->type == TOKEN_PIPE)
		{
			t = t->next;
			continue ;
		}
		cmd = build_command_from_tokens(&t, error);
		if (!cmd)
			return (NULL);
		if (head == NULL)
		{
			head = cmd;
			tail = cmd;
		}
		else
		{
			tail->next = cmd;
			tail = cmd;
		}
		if (t != NULL && t->type == TOKEN_PIPE)
			t = t->next;
	}
	return (head);
}

/* =================== Top-level parse entrypoint =================== */

// FIXME: too long; TODO: documentation
/* FIX ISSUE 2: Added char **envp parameter */
/* FIX ISSUE 4: Added t_shell *shell parameter for $? expansion */
int	parse(const char *line, t_parse_result *result, t_shell *shell)
{
	t_token		*tokens;
	char		*tok_err;

	if (!result)
		return (-1);
	tokens = NULL;
	tok_err = NULL;
	tokens = tokenize(line, &tok_err, shell);
	if (!tokens)										// NEW! whitespace doesn't produce tok_err -> ignored like in bash
	{
		if (tok_err)
			result->error = tok_err;
		return (0);
	}
	if (deal_with_pipes(&tokens, &tok_err, &result->incomplete_pipe) != 0)
	{
		if (tok_err)
			result->error = tok_err;
		else if (!result->error)
			result->error = strdup("Syntax error in pipe usage");				// TODO: switch to the ft_ version later; check if this line gets executed ever
		free_tokens(tokens);
		return (0);
	}
	result->commands = parse_tokens_to_commands(tokens, &tok_err);
	if (!result->commands && !result->incomplete_pipe)
	{
		/* FIX ISSUE 7: Use specific error if set, otherwise generic */
		if (tok_err)
			result->error = tok_err;
		else
			result->error = strdup("Syntax error building commands");				// TODO: switch to the ft_ version later
		free_tokens(tokens);
		return (0);
	}
	free_tokens(tokens);
	return (0);
}

/* ========================= Cleanup helpers ========================= */

/* Free tokens */
void	free_tokens(t_token *token)
{
	t_token	*next_token;

	while (token)
	{
		next_token = token->next;
		free(token->text);
		free(token);
		token = next_token;
	}
}

/* Free commands and their redirs/argv */			// FIXME: too long
void	free_commands(t_command *cmd)
{
	size_t		i;				// TODO: try eliminating i altogether to cut 4-5 lines
	t_command	*next_command;
	t_redir		*redir;
	t_redir		*next_redir;

	while (cmd)
	{
		next_command = cmd->next;
		if (cmd->argv)
		{
			i = 0;
			while (cmd->argv[i])
			{
				free(cmd->argv[i]);
				i++;
			}
			free(cmd->argv);
		}
		redir = cmd->redirs;
		while (redir)
		{
			next_redir = redir->next;
			free(redir->target);
			free(redir);
			redir = next_redir;
		}
		free(cmd);
		cmd = next_command;
	}
}

/* Free the dynamic buffer struct. */
void dynamic_buf_free(t_dynamic_buf *dynamic_buf)
{
	free(dynamic_buf->buf);
	dynamic_buf->buf = NULL;
	dynamic_buf->len = 0;
	dynamic_buf->cap = 0;
}

/* Signal handler to set the global signal number */		// TODO: figure this out; where is this supposed to go?
void	sigint_handler(int sig)
{
	g_signal_received = sig;
	write(STDOUT_FILENO, "\n", 1);		// for now this only helps with ctrl+c; figure out ctrl+'\'
}

// /* ========================= TESTING AND DEBUGGING ========================= */	// TODO: delete later

// /* A helper for print_commands to display the type of redirection. */
// static const char	*redir_type_to_str(t_token_type type)
// {
// 	switch (type)
// 	{
// 		case TOKEN_REDIR_IN:
// 			return ("<");
// 		case TOKEN_REDIR_OUT:
// 			return (">");
// 		case TOKEN_REDIR_APPEND:
// 			return (">>");
// 		case TOKEN_HEREDOC:
// 			return ("<<");
// 		default:
// 			return ("unknown");
// 	}
// }

// /* Print parsed commands. */
// static void	print_commands(t_command *cmds)
// {
// 	int	index;

// 	while (cmds)
// 	{
// 		index = 0;
// 		printf("Command:\n");
// 		if (cmds->argv)
// 		{
// 			while (cmds->argv[index])
// 			{
// 				printf("  argv[%d] = '%s'\n", index, cmds->argv[index]);
// 				index++;
// 			}
// 		}
// 		t_redir *r;
// 		r = cmds->redirs;
// 		while (r)
// 		{
// 			printf("  redir: type = %s target = %s\n", redir_type_to_str(r->type), r->target);
// 			r = r->next;
// 		}
// 		cmds = cmds->next;
// 	}
// }

// /* (for now debug only; COMPILATION: cc -lreadline parser.c) */
// int	main(void)
// {
// 	char				*line;
// 	t_token				*tokens;
// 	t_command			*cmds;
// 	struct sigaction	sa;				// TODO: figure this out
// 	t_parse_result		result;			// NEW! the final parsin result is stored here (use for execution)

// 	memset(&sa, 0, sizeof(sa));
// 	sa.sa_handler = sigint_handler;
// 	sigaction(SIGINT, &sa, NULL);
// 	while (1) 
// 	{
// 		line = readline("mini$ ");
// 		if (line == NULL)
// 		{
// 			printf("exit (debug)\n");
// 			break ;
// 		}
// 		if (line[0] != '\0')
// 		{
// 			add_history(line);
// 		}
// 		result.commands = NULL;
//         result.error = NULL;
//         result.incomplete_pipe = false;
// 		if (parse(line, &result) != 0)
//         {
//             fprintf(stderr, "Internal parse error\n");
//             free(line);
//             continue ;
//         }
// 		if (result.error)
//         {
//             fprintf(stderr, "Parse error: %s\n", result.error);
//             free(result.error);
//             free(line);
//             continue ;
//         }
// 		if (result.incomplete_pipe)
//         {	// execution layer should prompt for continuation; for now just notify and cleanup:
//             fprintf(stderr, "Input ends with trailing pipe (need continuation)\n");
//             free_commands(result.commands);
//             free(line);
//             continue ;
//         }
// 		cmds = result.commands;
// 		print_commands(cmds);
// 		free_commands(cmds);
// 		free(line);
// 		g_signal_received = 0;
// 	}
// 	return (0);
// }
