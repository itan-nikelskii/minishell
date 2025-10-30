/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup_parser.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 13:32:59 by inikelsk          #+#    #+#             */
/*   Updated: 2025/10/30 13:20:05 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.h"

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
void dynamic_buf_free(t_dyn_buf *dynamic_buf)
{
	free(dynamic_buf->buf);
	free(dynamic_buf);
	// ISSUE 23: Fixed memory leak - struct was not being freed
	// AddressSanitizer detected 144 bytes leaked (6 allocations × 24 bytes)
}
