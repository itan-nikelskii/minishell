/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isprint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:09:29 by acossari          #+#    #+#             */
/*   Updated: 2025/05/13 13:47:40 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** ft_isprint: check if c is a printable character (including space)
** @c: the character to test
** Return: 1 if ASCII code is in range 32–126, 0 otherwise
*/
int	ft_isprint(int c)
{
	unsigned char	uc;

	uc = (unsigned char)c;
	return (uc >= 32 && uc <= 126);
}
