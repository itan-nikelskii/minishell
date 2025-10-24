/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:09:58 by acossari          #+#    #+#             */
/*   Updated: 2025/05/13 19:44:04 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** ft_memset: fill memory area with a constant byte
** @b: pointer to the memory area to fill
** @c: byte value to set (converted to unsigned char)
** @n: number of bytes to set
** Return: pointer to the memory area b
*/
void	*ft_memset(void *b, int c, size_t n)
{
	size_t			i;
	unsigned char	*p;
	unsigned char	uc;

	p = (unsigned char *)b;
	uc = (unsigned char)c;
	i = 0;
	while (i < n)
	{
		p[i] = uc;
		i++;
	}
	return (b);
}
