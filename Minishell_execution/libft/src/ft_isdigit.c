/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:09:23 by acossari          #+#    #+#             */
/*   Updated: 2025/05/13 13:37:23 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** ft_isdigit: check if c is a decimal digit character
** @c: the character to test
** Return: 1 if '0'–'9', 0 otherwise
*/
int	ft_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}
