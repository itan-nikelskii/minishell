/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 14:08:46 by inikelsk          #+#    #+#             */
/*   Updated: 2025/10/30 15:42:51 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.h"

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
		result = ft_strdup(value);
	else
		result = ft_strdup("");
	free(name);
	*j = index;
	return (result);
}

/* FIXME: too long; TODO: documentation (after proper refactoring for norminette) */
int	expand_dollar(const char *s, size_t *j, t_dyn_buf *buf, t_shell *shell)
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
	else if (ft_isalpha((unsigned char)s[index]) || s[index] == '_')
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
