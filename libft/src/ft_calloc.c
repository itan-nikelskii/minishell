/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:08:37 by acossari          #+#    #+#             */
/*   Updated: 2025/05/14 19:46:49 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdint.h>	/* for SIZE_MAX */
/*
** ft_calloc: allocate and zero-initialize an array
** @nmemb: number of elements to allocate
** @size:  size in bytes of each element
** Return: pointer to allocated memory,
**         or NULL on overflow or allocation failure
*/
void	*ft_calloc(size_t nmemb, size_t size)
{
	size_t	total;
	void	*ptr;

	if (size != 0 && nmemb > SIZE_MAX / size)
		return (NULL);
	total = nmemb * size;
	ptr = malloc(total);
	if (!ptr)
		return (NULL);
	ft_memset(ptr, 0, total);
	return (ptr);
}
