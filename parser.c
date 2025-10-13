/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 09:12:43 by inikelsk          #+#    #+#             */
/*   Updated: 2025/10/13 17:11:13 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TODO:
// 0. FIXME:
// 		1. ctrl-\ is supposed to od nothing, but rn it gives a segfault
// 		2. | echo hello (leading/trailing pipe) should (?) be an error, but rn it's not
//		3. echo hello || echo world (multiple pipea should be an error, but rn it's not)
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

/* Token types */
typedef enum e_token_type 
{
	TOKEN_WORD,
	TOKEN_PIPE,			// |
	TOKEN_REDIR_IN,		// <
	TOKEN_REDIR_OUT,	// >
	TOKEN_REDIR_APPEND,	// >>
	TOKEN_HEREDOC		// <<
}	t_token_type;

/* Token structure */		// TODO: figure out if it's a good idea to store single/double quotes like in my old version
typedef struct s_token
{
	t_token_type	type;
	char			*text;
	struct s_token	*next;
}	t_token;

/* Redirection entry */		// TODO: document properly
typedef struct s_redir
{
	t_token_type	type;		// type of redirection token
	char			*target;	// filename (or heredoc delimiter) string
	struct s_redir	*next;
}	t_redir;

/* Command node (in a pipeline) */
typedef struct s_command
{
	char				**argv;
	t_redir				*redirs;
	struct s_command	*next;
}	t_command;

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
t_redir		*new_redir(t_token_type type, char *target);

/* env helpers */
char		*get_env_value(const char *name);
char		*get_exit_status_str();

/* buffer/expansion helpers */
int			ensure_buffer_capacity(t_dynamic_buf *dynamic_buf, size_t need);
int			append_char(t_dynamic_buf *dynamic_buf, char c);
int			append_str(t_dynamic_buf *dynamic_buf, const char *s);
char		*expand_variable(const char *s, size_t index, size_t *j);

/* parse small parts */
int			parse_single_quote(const char *s, size_t *i, char **out);
int			parse_double_quote(const char *s, size_t *i, char **out);
int			parse_unquoted_word(const char *s, size_t *i, char **out);
int			expand_dollar(const char *s, size_t *j, t_dynamic_buf *dynamic_buf);

/* tokenize helpers */
int			create_token_pipe(const char *line, size_t *i, t_token **head, t_token **tail);
int			create_token_redirection(const char *line, size_t *i, t_token **head, t_token **tail);
int			create_token_quote_or_word(const char *line, size_t *i, t_token **head, t_token **tail);

/* top-level tokenize & parse */
t_token		*tokenize(const char *line, char **error);
size_t		count_words_in_segment(t_token *t);
int			consume_redirection_target(t_token *token, t_command *cmd);
int			add_word_to_cmd_argv(t_command *cmd, const char *word, size_t *arg_index);
t_command	*init_t_command(t_token *token);
t_command	*build_command_from_tokens(t_token **tp);
t_command	*parse_tokens_to_commands(t_token *t);

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
t_redir	*new_redir(t_token_type type, char *target)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return NULL;
	redir->type = type;
	redir->target = target;
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

/* Get the environment variable value (caller frees the returned pointer). */
char *get_env_value(const char *name)
{
	char	*value;
	char	*out;

	value = getenv(name);
	if (value == NULL)
		out = strdup("");			// TODO: switch to the ft_ version later
	else
		out = strdup(value);
	return (out);					// TODO: when trying to free value, get a segfault; why?
}

/* Get the exit status string; FIXME: actually implement this in the execution part */
char	*get_exit_status_str()
{
	char	*result;

	result = malloc(2);
	if (!result)
		return (NULL);
	result[0] = '0';
	result[1] = '\0';
	return (result);
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

/* here to make expand_dollar smaller; TODO: documentation (mention advancing j) */
char	*expand_variable(const char *s, size_t index, size_t *j)
{
	size_t	name_start; // index where the variable name starts (first char after $ that is alphabetic or underscore)
	char	*name;		// temp string holding the variable name extracted from the input (e.g., "PATH") before calling getenv(name)
	char	*result;

	result = NULL;
	name_start = index;												// TODO: figure out and double check
	while (isalnum((unsigned char)s[index]) || s[index] == '_')		// TODO: switch to the ft_ version later
		index++;
	name = strdup_range(s, name_start, index);
	if (!name)
		return (NULL);
	result = get_env_value(name);
	free(name);
	*j = index;
	return (result);
}

// FIXME: too long
/* Helper to expand $ while parsing double quotes or unquoted words. Append the
expanded result to the growing buffer; return 0 on success, -1 on failure. */
int	expand_dollar(const char *s, size_t *j, t_dynamic_buf *dynamic_buf)
{
	size_t	index; 		// current scanning index after encountering $; points at the next char to examine
	char	*expanded; 	// the string result of the expansion (value to insert into the output buffer)

	index = *j + 1;
	if (s[index] == '?')
	{
		expanded = get_exit_status_str();
		if (!expanded)
			return (-1);
		*j = index + 1;
	}
	else if (isalpha((unsigned char)s[index]) || s[index] == '_')		// TODO: switch to the ft_ version later
	{
		expanded = expand_variable(s, index, j);		// *j is incremented inside
		if (!expanded)
			return (-1);
	}
	else // literal $; TODO: double check if this is how bash does it
	{
		if (append_char(dynamic_buf, '$') != 0)
			return (-1);
		*j += 1;
		return (0);
	}
	if (append_str(dynamic_buf, expanded) != 0)
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
int	parse_double_quote(const char *s, size_t *i, char **out)
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
			if (expand_dollar(s, &j, dynamic_buf) != 0)	// TODO: if j handling is changed in expand_dollar(), redo this
				return (dynamic_buf_free(dynamic_buf), -1);
			continue ; // TODO: figure out if I want to somehow increment j here instead of inside expand_dollar
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
int	parse_unquoted_word(const char *s, size_t *i, char **out)
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
			if (expand_dollar(s, &j, dynamic_buf) != 0)	// TODO: if j handling is changed in expand_dollar(), redo this
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
int	create_token_pipe(const char *line, size_t *i, t_token **head, t_token **tail)
{
	t_token	*token;

	token = new_token(TOKEN_PIPE, strdup("|"));
	if (!token)
		return (-1);
	if (append_token(head, tail, token) != 0)
		return (free(token), -1);
	*i = *i + 1;
	return (0);
}

/* Handle the redirection (<, <<, >, >>) chars: determine the redirection type, 
create a new token, append it to the token list, and update *i depending on
the number of chars in the redirection type (1 or 2).
Return 0 on success or -1 on failure. */		
int	create_token_redirection(const char *line, size_t *i, t_token **head, t_token **tail)
{
	t_token	*token;

	if (line[*i] == '<')
	{
		if (line[*i + 1] && line[*i + 1] == '<')
			token = new_token(TOKEN_HEREDOC, strdup("<<"));			// TODO: switch to the ft_ version later
		else
			token = new_token(TOKEN_REDIR_IN, strdup("<"));			// TODO: switch to the ft_ version later
	}
	else if (line[*i] == '>')
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
	if (!token)
		return (-1);
	if (append_token(head, tail, token) != 0)
		return (-1);
	return (0);
}

/* Create a token out of an unquoted word or a quoted string. Parse the string
depending on the presence and type of quotes; create a new token and return 0
on success or -1 on failure. */
int	create_token_quote_or_word(const char *line, size_t *i, t_token **head, t_token **tail)
{
	char	*txt;
	int		status;
	t_token	*token;

	txt = NULL;
	if (line[*i] == '\'')
		status = parse_single_quote(line, i, &txt);
	else if (line[*i] == '"')
		status = parse_double_quote(line, i, &txt);
	else
		status = parse_unquoted_word(line, i, &txt);
	if (status != 0)
		return (-1);
	token = new_token(TOKEN_WORD, txt);
	if (!token)
		return (free(txt), -1);
	if (append_token(head, tail, token) != 0)
	{
		free(token->text);
		free(token);
		return (-1);
	}
	return (0);
}

/* Tokenize input line into linked token list */	// FIXME: too long; decide if it should deal with error handling (refactoring will heavily depend on this)
t_token	*tokenize(const char *line, char **error)
{
	t_token	*head;
	t_token	*tail;
	size_t	i;
	char	c;

	head = NULL;
	tail = NULL;
	i = 0;
	while (line[i] != '\0')
	{
		c = line[i];
		if (isspace((unsigned char)c))
		{
			i++;
			continue ;
		}
		if (c == '\\' || c == ';')						// TODO: double check; include any other ones?
		{
			*error = strdup("Unsupported escape or special character");
			free_tokens(head);
			return (NULL);
		}
		if (c == '|')
		{
			if (create_token_pipe(line, &i, &head, &tail) != 0)
			{
				*error = strdup("Out of memory");		// TODO: double check if this is true
				free_tokens(head);
				return (NULL);
			}
			continue ;
		}
		if (c == '<' || c == '>')
		{
			if (create_token_redirection(line, &i, &head, &tail) != 0)
			{
				*error = strdup("Out of memory");		// TODO: double check if this is true
				free_tokens(head);
				return (NULL);
			}
			continue ;
		}
		if (c == '\'' || c == '"' || is_word_char(c))
		{
			if (create_token_quote_or_word(line, &i, &head, &tail) != 0)
			{
				*error = strdup("Parse error in word/quote");
				free_tokens(head);
				return (NULL);
			}
			continue ;
		}
		// unknown char: should not happen; TODO: double check
		i++;
	}
	return (head);
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
	target = strdup(token->text);
	if (!target)
		return (-1);
	redir = new_redir(redir_type, target);
	if (!redir)
	{
		free(target);
		return (-1);
	}
	redir->next = cmd->redirs;
	cmd->redirs = redir;
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
t_command	*build_command_from_tokens(t_token **tp)
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

/* Parse token list into pipeline (linked commands) */		// FIXME: too long
t_command	*parse_tokens_to_commands(t_token *t)
{
	t_command	*head;
	t_command	*tail;
	t_command	*cmd;

	head = NULL;
	tail = NULL;
	while (t != NULL)
	{
		if (t->type == TOKEN_PIPE)			// FIXME: is this why the parser is ok with multiple pipes? then fix
		{
			t = t->next;
			continue ;
		}
		cmd = build_command_from_tokens(&t);
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
	t_command	*next_cmd;
	t_redir		*redir;
	t_redir		*next_redir;

	while (cmd)
	{
		next_cmd = cmd->next;
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
		cmd = next_cmd;
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

/* ========================= TESTING AND DEBUGGING ========================= */	// TODO: delete later

/* A helper for print_commands to display the type of redirection. */
static const char	*redir_type_to_str(t_token_type type)
{
	switch (type)
	{
		case TOKEN_REDIR_IN:
			return ("<");
		case TOKEN_REDIR_OUT:
			return (">");
		case TOKEN_REDIR_APPEND:
			return (">>");
		case TOKEN_HEREDOC:
			return ("<<");
		default:
			return ("unknown");
	}
}

/* Print parsed commands. */
static void	print_commands(t_command *cmds)
{
	int	index;

	while (cmds)
	{
		index = 0;
		printf("Command:\n");
		if (cmds->argv)
		{
			while (cmds->argv[index])
			{
				printf("  argv[%d] = '%s'\n", index, cmds->argv[index]);
				index++;
			}
		}
		t_redir *r;
		r = cmds->redirs;
		while (r)
		{
			printf("  redir: type = %s target = %s\n", redir_type_to_str(r->type), r->target);
			r = r->next;
		}
		cmds = cmds->next;
	}
}

/* (for now debug only; COMPILATION: cc -lreadline parser.c) */
int	main(void)
{
	char				*line;
	char				*error;
	t_token				*tokens;
	t_command			*cmds;
	struct sigaction	sa;				// TODO: figure this out

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigint_handler;
	sigaction(SIGINT, &sa, NULL);
	while (1) 
	{
		line = readline("mini$ ");
		if (line == NULL)
		{
			printf("exit (debug)\n");
			break ;
		}
		if (line[0] != '\0')
		{
			add_history(line);
		}
		error = NULL;
		tokens = tokenize(line, &error);
		if (tokens == NULL)
		{
			if (error)
			{
				fprintf(stderr, "Parse error: %s\n", error);
				free(error);
			}
			else
			{
				fprintf(stderr, "Unknown parse error\n");
			}
			free(line);
			continue ;
		}
		cmds = parse_tokens_to_commands(tokens);
		if (cmds == NULL)
		{
			fprintf(stderr, "Syntax error building commands\n");
			free_tokens(tokens);
			free(line);
			continue ;
		}
		print_commands(cmds);
		free_tokens(tokens);
		free_commands(cmds);
		free(line);
		g_signal_received = 0;	// TODO: figure out
	}
	return (0);
}
