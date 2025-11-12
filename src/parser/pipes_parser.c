/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:28:53 by inikelsk          #+#    #+#             */
/*   Updated: 2025/11/12 10:31:13 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser.h"

/* Validate pipe usage in the token list:
	- reject a leading pipe (e.g. "| echo"; second if);
	- reject consecutive pipes (e.g. "echo || echo" or "echo | | echo"; while).
   On success, return 0; on error, return -1 and set the *error message. */
static int	validate_pipes(t_token *tokens, char **error)
{
	t_token	*current;

	if (tokens == NULL)
		return (0);
	if (tokens->type == TOKEN_PIPE)
	{
		*error = ft_strdup("syntax error near unexpected token `|'");
		return (-1);
	}
	current = tokens;
	while (current)
	{
		if (current->type == TOKEN_PIPE && current->next && current->next->type
			== TOKEN_PIPE)
		{
			*error = ft_strdup("syntax error near unexpected token `|'");
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
static int	strip_trailing_pipe(t_token **tokens, bool *incomplete)
{
	t_token	*current;
	t_token	*previous;

	*incomplete = false;
	previous = NULL;
	current = *tokens;
	while (current && current->next)
	{
		previous = current;
		current = current->next;
	}
	if (current && current->type == TOKEN_PIPE)
	{
		*incomplete = true;
		if (previous)
			previous->next = NULL;
		else
			*tokens = NULL;
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
	while (current)
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
	if (validate_pipes(*tokens, error) != 0)
		return (-1);
	strip_trailing_pipe(tokens, incomplete);
	return (0);
}
