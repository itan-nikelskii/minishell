/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_resolver.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antoniocossari <antoniocossari@student.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 09:51:28 by acossari          #+#    #+#             */
/*   Updated: 2025/10/30 15:11:46 by antoniocoss      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

/**
 * Get value of environment variable
 * @param key Variable name (e.g., "PATH")
 * @param envp Environment array
 * @return Pointer to value part (after '='), or NULL if not found
 */
char	*get_env_value(char *key, char **envp)
{
	int		i;
	int		key_len;

	if (!key || !envp)
		return (NULL);
	key_len = ft_strlen(key);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], key, key_len) == 0
			&& envp[i][key_len] == '=')
			return (envp[i] + key_len + 1);
		i++;
	}
	return (NULL);
}

/**
 * Extract PATH directories from environment variables
 * @param envp Environment variables array
 * @return Malloc'd array of PATH directories, or NULL if PATH not found
 */
static char	**get_env_paths(char **envp)
{
	char	*path_env;
	char	**paths;

	path_env = get_env_value("PATH", envp);
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	return (paths);
}

/**
 * Build full path by concatenating directory and command
 * @param dir Directory path
 * @param cmd Command name
 * @return Malloc'd full path string, or NULL on allocation failure
 */
static char	*build_full_path(const char *dir, const char *cmd)
{
	char	*full_path;
	size_t	path_len;
	size_t	cmd_len;
	size_t	buf_size;

	path_len = ft_strlen(dir);
	cmd_len = ft_strlen(cmd);
	buf_size = path_len + 1 + cmd_len + 1;
	full_path = malloc(buf_size);
	if (!full_path)
		return (NULL);
	ft_strlcpy(full_path, dir, buf_size);
	ft_strlcat(full_path, "/", buf_size);
	ft_strlcat(full_path, cmd, buf_size);
	return (full_path);
}

/**
 * Find command in given PATH directories
 * @param cmd Command name
 * @param env_paths Array of PATH directories
 * @return Malloc'd full path if found and executable, otherwise NULL
 */
static char	*find_in_path(const char *cmd, char **env_paths)
{
	char	*full_path;
	int		i;

	if (!env_paths || !cmd)
		return (NULL);
	i = 0;
	while (env_paths[i])
	{
		full_path = build_full_path(env_paths[i], cmd);
		if (!full_path)
			return (NULL);
		if (access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
		i++;
	}
	return (NULL);
}

/**
 * Resolve command path
 * Handles three cases:
 * 1. Command with '/' → return as-is (let execve handle errors)
 * 2. Command without '/' → search in PATH
 * 3. PATH not found or command not in PATH → return NULL
 * @param command Command to resolve
 * @param envp Environment variables
 * @return Full path to executable, with fallback to "./command"
 *         or NULL if not found
 */
char	*resolve_path(char *command, char **envp)
{
	char	**env_paths;
	char	*path;

	if (!command || !*command)
		return (NULL);
	if (ft_strchr(command, '/'))
		return (command);
	env_paths = get_env_paths(envp);
	if (!env_paths)
	{
		path = build_full_path(".", command);
		if (!path)
			return (NULL);
		if (access(path, X_OK) == 0)
			return (path);
		free(path);
		return (NULL);
	}
	path = find_in_path(command, env_paths);
	free_array(env_paths);
	return (path);
}
