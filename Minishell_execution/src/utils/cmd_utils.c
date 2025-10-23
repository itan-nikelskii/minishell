/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 15:18:29 by acossari          #+#    #+#             */
/*   Updated: 2025/10/23 13:57:39 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Counts number of commands in linked list
 * @param cmd_list First command in list
 * @return Number of commands
 */
int	count_commands(t_command *cmd_list)
{
	int			count;
	t_command	*current;

	count = 0;
	current = cmd_list;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}

/**
 * Checks if command is the last in the pipeline
 * @param cmd Command to check
 * @return 1 if last, 0 otherwise
 */
int	is_last_command(t_command *cmd)
{
	if (!cmd)
		return (1);
	return (cmd->next == NULL);
}
