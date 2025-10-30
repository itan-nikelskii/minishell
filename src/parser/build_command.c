/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_command.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:31:49 by inikelsk          #+#    #+#             */
/*   Updated: 2025/10/30 11:39:41 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.h"

/* Count words until a pipe or end of command segment; return count. */
static size_t	count_words_in_segment(t_token *token)
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
static int	add_word_to_cmd_argv(t_command *cmd, const char *word, size_t *arg_index)
{
	cmd->argv[*arg_index] = strdup(word);				// TODO: switch to the ft_ version later
	if (!cmd->argv[*arg_index])
		return (-1);
	*arg_index += 1;
	return (0);
}

/* Initialize a t_command with NULLs and return it, or just NULL on failure. */
static t_command *init_t_command(t_token *token)
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

// FIXME: too long
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
			if (!token->next || token->next->type != TOKEN_WORD)	/* FIX ISSUE 7: Check if redirection target is missing */
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
