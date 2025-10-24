/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:10:47 by acossari          #+#    #+#             */
/*   Updated: 2025/05/14 14:34:59 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** ft_strrchr: locate last occurrence of character c in string s
** @s:   null-terminated source string
** @c:   character to find (converted to char)
** Return: pointer to matching byte or NULL if not found
*/
char	*ft_strrchr(const char *s, int c)
{
	char		ch;
	const char	*last;

	ch = (char)c;
	last = NULL;
	while (*s)
	{
		if (*s == ch)
			last = s;
		s++;
	}
	if (ch == '\0')
		return ((char *)s);
	if (last)
		return ((char *)last);
	return (NULL);
}
