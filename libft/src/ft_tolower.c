/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:10:54 by acossari          #+#    #+#             */
/*   Updated: 2025/05/14 13:42:14 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** ft_tolower: convert uppercase letter to lowercase
** @c: the character to convert
** Return: lowercase equivalent if 'A'–'Z', else unchanged
*/
int	ft_tolower(int c)
{
	unsigned char	uc;

	uc = (unsigned char)c;
	if (uc >= 'A' && uc <= 'Z')
		return (uc - 'A' + 'a');
	return (c);
}
