/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 00:00:00 by acossari          #+#    #+#             */
/*   Updated: 2025/11/12 10:21:16 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

/*
 * ft_strcmp - Compare two strings
 *
 * @s1: First string
 * @s2: Second string
 *
 * Returns:
 *   < 0  if s1 < s2
 *   0    if s1 == s2
 *   > 0  if s1 > s2
 *
 * Compares the entire strings (no length limit like strncmp)
 */
int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}
