/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_get_next_line_utils.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/21 10:50:29 by antoniocoss       #+#    #+#             */
/*   Updated: 2025/10/01 19:59:40 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/libft.h"

/*
** find_newline: check if a '\n' is present in data[0..len-1]
** @data: pointer to bytes to scan
** @len:  number of bytes to scan
** Return: 1 if '\n' found,
**         0 if not found or len is zero
*/
int	find_newline(char *data, size_t len)
{
	size_t	i;

	if (len == 0)
		return (0);
	i = 0;
	while (i < len)
	{
		if (data[i] == '\n')
			return (1);
		i++;
	}
	return (0);
}

/*
** init_stash: allocate initial space for the buffer
** @stash:  pointer to t_buffer to initialize
** Return: 1 on successful allocation,
**         0 on allocation failure or invalid size
*/
int	init_stash(t_buffer *stash)
{
	if (INITIAL_CAP_SIZE == 0)
		return (0);
	stash->data = (char *)malloc(INITIAL_CAP_SIZE);
	if (!stash->data)
	{
		stash->cap = 0;
		stash->len = 0;
		return (0);
	}
	stash->cap = INITIAL_CAP_SIZE;
	stash->len = 0;
	return (1);
}

/*
** grow_stash: expand stash->data to fit at least 'required' bytes
** @stash:    pointer to t_buffer to grow
** @required: minimum capacity needed
** Return: 1 on success,
**         0 on overflow, allocation failure, or invalid stash
*/
static int	grow_stash(t_buffer *stash, size_t required)
{
	char	*new_data;
	size_t	new_cap;
	size_t	i;

	if (!stash || !stash->data)
		return (0);
	new_cap = stash->cap;
	while (new_cap < required)
	{
		if (new_cap > ((size_t) - 1) / 2)
			return (0);
		new_cap *= 2;
	}
	new_data = (char *)malloc(new_cap);
	if (!new_data)
		return (0);
	i = stash->len;
	while (i--)
		new_data[i] = stash->data[i];
	free(stash->data);
	stash->data = new_data;
	stash->cap = new_cap;
	return (1);
}

/*
** append_to_stash: append 'len' bytes from data into stash->data
** @stash:  pointer to t_buffer receiving new bytes
** @data:   source bytes to append
** @len:    number of bytes to append
** Return: 1 on success,
**         0 on invalid args, overflow, or allocation failure
*/
int	append_to_stash(t_buffer *stash, char *data, size_t len)
{
	size_t	i;

	if (!stash || !data)
		return (0);
	if (len == 0)
		return (1);
	if (stash->len > ((size_t)-1) - len)
		return (0);
	if (stash->len + len > stash->cap)
		if (!grow_stash(stash, stash->len + len))
			return (0);
	i = 0;
	while (i < len)
	{
		stash->data[stash->len + i] = data[i];
		i++;
	}
	stash->len += len;
	return (1);
}
