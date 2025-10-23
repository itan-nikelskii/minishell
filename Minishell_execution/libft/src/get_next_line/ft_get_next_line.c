/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_get_next_line.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/21 10:51:49 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/10/01 19:57:41 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/libft.h"

/*
** read_to_stash: read from fd and append to stash until '\n', EOF, or error
** @fd:     file descriptor to read from
** @stash:  pointer to t_buffer holding current stash (may be init if data NULL)
** Return: 1 on successful read (stash contains data or newline),
**         0 on EOF with empty stash or on error/allocation failure
*/
static int	read_to_stash(int fd, t_buffer *stash)
{
	char	*tmp;
	ssize_t	bytes_read;

	if (stash->data && stash->len > 0
		&& find_newline(stash->data, stash->len))
		return (1);
	tmp = (char *)malloc(BUFFER_SIZE);
	if (!tmp)
		return (0);
	if (!stash->data && !init_stash(stash))
		return (free(tmp), 0);
	bytes_read = read(fd, tmp, BUFFER_SIZE);
	while (bytes_read > 0)
	{
		if (!append_to_stash(stash, tmp, (size_t)bytes_read))
			return (free(tmp), 0);
		if (find_newline(tmp, (size_t)bytes_read))
			break ;
		bytes_read = read(fd, tmp, BUFFER_SIZE);
	}
	free(tmp);
	if (bytes_read < 0 || (!stash->len && bytes_read == 0))
		return (0);
	return (1);
}

/*
** extract_line: allocate and return the next line (up to and including '\n')
** @stash:  pointer to t_buffer containing data to extract from
** Return: pointer to newly malloc’d line on success,
**         NULL on allocation failure or if stash empty
*/
static char	*extract_line(t_buffer *stash)
{
	size_t	i;
	size_t	len;
	char	*line;

	len = 0;
	while (len < stash->len && stash->data[len] != '\n')
		len++;
	if (len < stash->len)
		len++;
	line = (char *)malloc(len + 1);
	if (!line)
		return (NULL);
	i = 0;
	while (i < len)
	{
		line[i] = stash->data[i];
		i++;
	}
	line[i] = '\0';
	return (line);
}

/*
** update_stash: remove the extracted line from stash, shifting remaining bytes
** @stash:  pointer to t_buffer to update
** Return: 1 always (stash cleared or updated), 0 would indicate failure
*/
static int	update_stash(t_buffer *stash)
{
	size_t	i;
	size_t	j;
	size_t	len;

	len = 0;
	while (len < stash->len && stash->data[len] != '\n')
		len++;
	if (len < stash->len)
		len++;
	if (len == stash->len)
	{
		stash->len = 0;
		return (1);
	}
	i = len;
	j = 0;
	while (i < stash->len)
		stash->data[j++] = stash->data[i++];
	stash->len = j;
	return (1);
}

/*
** clear_stash: free and reset all fields of a t_buffer to initial state
** @stash:  pointer to t_buffer to clear
** Return: void (stash is now empty and safe for reuse)
*/
void	clear_stash(t_buffer *stash)
{
	if (stash->data)
		free(stash->data);
	stash->data = NULL;
	stash->len = 0;
	stash->cap = 0;
}

/*
** get_next_line: return next line from fd by coordinating stash routines
** @fd:     file descriptor to read from
** Return: malloc’d string ending in '\n' or EOF data on success,
**         NULL on error, EOF with no data, or invalid parameters
*/
char	*ft_get_next_line(int fd)
{
	static t_buffer	buffers[MAX_FD];
	char			*line;

	if (fd < 0 || fd >= MAX_FD || BUFFER_SIZE <= 0)
		return (NULL);
	if (!read_to_stash(fd, &buffers[fd]))
		return (clear_stash(&buffers[fd]), NULL);
	line = extract_line(&buffers[fd]);
	if (!line)
		return (clear_stash(&buffers[fd]), NULL);
	if (!update_stash(&buffers[fd]))
		return (free(line), clear_stash(&buffers[fd]), NULL);
	return (line);
}
