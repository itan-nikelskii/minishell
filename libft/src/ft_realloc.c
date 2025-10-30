/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/30 14:30:30 by inikelsk          #+#    #+#             */
/*   Updated: 2025/10/30 14:55:20 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_realloc(void *ptr, size_t size)
{
	void	*new_ptr;
	size_t	old_size;
	size_t	i;

	new_ptr = NULL;
	old_size = 0;
	i = 0;
	if (ptr == NULL)
		return (malloc(size));
	if (size == 0)
		return (free(ptr), NULL);
	old_size = ft_strlen((char *)ptr);
	new_ptr = malloc(size);
	if (!new_ptr)
		return (NULL);
	while (i < old_size && i < size)
	{
		((char *)new_ptr)[i] = ((char *)ptr)[i];
		i++;
	}
	return (free(ptr), new_ptr);
}
