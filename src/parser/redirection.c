/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:14:24 by inikelsk          #+#    #+#             */
/*   Updated: 2025/10/30 15:06:47 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.h"

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
	while (ft_isspace(line[j]))
		j++;
	if (line[j] == '<' || line[j] == '>')
		return (true);
	return (false);
}

/* Handle the redirection (<, <<, >, >>) chars: determine the redirection type,
create a new token, append it to the token list, and update *i depending on
the number of chars in the redirection type (1 or 2).
Return 0 on success, -1 on malloc failure, or -2 on syntax error. */
int	create_token_redirection(const char *line, size_t *i, t_token_list *list)
{
	t_token	*token;

	if (is_invalid_redir_sequence(line, *i))
		return (-2);
	if (line[*i] == '<')
	{
		if (line[*i + 1] && line[*i + 1] == '<')
			token = new_token(TOKEN_HEREDOC, ft_strdup("<<"));
		else
			token = new_token(TOKEN_REDIR_IN, ft_strdup("<"));
	}
	else 	// if (line[*i] == '>')
	{
		if (line[*i + 1] && line[*i + 1] == '>')
			token = new_token(TOKEN_REDIR_APPEND, ft_strdup(">>"));
		else
			token = new_token(TOKEN_REDIR_OUT, ft_strdup(">"));
	}
	if (line[*i + 1] && (line[*i + 1] == '<' || line[*i + 1] == '>'))
		*i += 2;
	else
		*i += 1;
	if (!token || append_token(list, token) != 0)
		return (-1);
	return (0);
}

// FIXME: too long
/* Consume the redirection target token and attach to cmd.redirs. On success,							// DONE
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
	target = ft_strdup(token->text);
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
