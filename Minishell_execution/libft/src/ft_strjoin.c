/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 18:44:46 by acossari          #+#    #+#             */
/*   Updated: 2025/05/14 20:56:01 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** ft_strjoin: concatenate s1 and s2 into a new string
** @s1:    first null-terminated string
** @s2:    second null-terminated string
** Return: pointer to the fresh string (must be freed),
**         or NULL if allocation fails or inputs are NULL
*/
char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	total;
	char	*res;
	char	*dst;

	if (!s1 || !s2)
		return (NULL);
	total = ft_strlen(s1) + ft_strlen(s2);
	res = malloc(total + 1);
	if (!res)
		return (NULL);
	dst = res;
	while (*s1)
		*dst++ = *s1++;
	while (*s2)
		*dst++ = *s2++;
	*dst = '\0';
	return (res);
}
