/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 18:50:02 by acossari          #+#    #+#             */
/*   Updated: 2025/05/14 21:10:58 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** ft_strtrim: allocate and return a trimmed copy of s1
** @s1:  the source string
** @set: the set of characters to remove
** Return: pointer to new string (must be freed),
**         or NULL if allocation fails
*/
char	*ft_strtrim(char const *s1, char const *set)
{
	size_t	start;
	size_t	end;
	char	*res;

	if (!s1 || !set)
		return (NULL);
	start = 0;
	while (s1[start] && ft_strchr(set, s1[start]))
		start++;
	end = ft_strlen(s1);
	while (end > start && ft_strchr(set, s1[end - 1]))
		end--;
	res = malloc(end - start + 1);
	if (!res)
		return (NULL);
	ft_memcpy(res, s1 + start, end - start);
	res[end - start] = '\0';
	return (res);
}
