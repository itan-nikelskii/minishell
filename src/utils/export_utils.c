/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 12:52:08 by acossari          #+#    #+#             */
/*   Updated: 2025/11/11 22:48:41 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Check if a string is a valid export identifier
 * A valid identifier starts with a letter or underscore, followed by letters,
 * digits, or underscores, and may optionally include an '=' sign after the name.
 * Examples: valid: VAR, _VAR, VAR123, VAR=VALUE
 *          invalid: 123VAR, VAR-TEST, VAR TEST
 * @param str The string to check
 * @return true if valid, false otherwise
 */
bool	is_valid_export_identifier(char *str)
{
	int	i;

	if (!str)
		return (false);
	if (!ft_isalpha(str[0]) && str[0] != '_')
		return (false);
	i = 1;
	while (str[i] && str[i] != '=')
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (false);
		i++;
	}
	return (true);
}

/**
 * Print a single environment variable in export format
 * @param env_var The environment variable string (e.g., "KEY=VALUE" or "KEY")
 */
void	print_export_line(char *env_var)
{
	char	*equal_sign;

	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	equal_sign = ft_strchr(env_var, '=');
	if (equal_sign)
	{
		write(STDOUT_FILENO, env_var, equal_sign - env_var + 1);
		ft_putchar_fd('"', STDOUT_FILENO);
		ft_putstr_fd(equal_sign + 1, STDOUT_FILENO);
		ft_putchar_fd('"', STDOUT_FILENO);
	}
	else
		ft_putstr_fd(env_var, STDOUT_FILENO);
	ft_putchar_fd('\n', STDOUT_FILENO);
}

/**
 * Build a merged and sorted export array from envp and xenv
 * @param envp The environment variables array
 * @param xenv The exported names without values array
 * @return A newly allocated sorted array of export strings, or NULL on failure
 */
static char	**build_export_vector(char **envp, char **xenv)
{
	int		env_count;
	int		xenv_count;
	char	**merged;
	int		i;
	int		j;

	env_count = count_array(envp);
	xenv_count = count_array(xenv);
	merged = malloc(sizeof(char *) * (env_count + xenv_count + 1));
	if (!merged)
		return (NULL);
	i = 0;
	while (i < env_count)
	{
		merged[i] = envp[i];
		i++;
	}
	j = 0;
	while (j < xenv_count)
		merged[i++] = xenv[j++];
	merged[i] = NULL;
	sort_env_array(merged);
	return (merged);
}

/**
 * Merges envp and xenv, sorts them, and prints in export format
 * @param shell The shell structure containing envp and xenv
 * @return 0 on success, 1 on memory allocation failure
 */
int	print_export_merged(t_shell *shell)
{
	char	**sorted_vars;
	int		i;

	sorted_vars = build_export_vector(shell->envp, shell->xenv);
	if (!sorted_vars)
		return (1);
	i = 0;
	while (sorted_vars[i])
	{
		print_export_line(sorted_vars[i]);
		i++;
	}
	free(sorted_vars);
	return (0);
}
