/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 18:53:27 by acossari          #+#    #+#             */
/*   Updated: 2025/05/15 12:07:56 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** get_len: calculate number of characters needed for integer conversion
** @n: the integer value (as long) to measure
** Return: digits count + 1 if negative (for '-'), 1 if n == 0
*/
static size_t	get_len(long n)
{
	size_t	len;

	len = 0;
	if (n <= 0)
		len = 1;
	while (n)
	{
		n /= 10;
		len++;
	}
	return (len);
}

/*
** ft_itoa: convert an integer to its string representation
** @n: the integer to convert
** Return: newly allocated string (must be freed),
**         or NULL if allocation fails
*/
char	*ft_itoa(int n)
{
	long	nbr;
	size_t	len;
	char	*res;

	nbr = n;
	len = get_len(nbr);
	res = malloc(len + 1);
	if (!res)
		return (NULL);
	if (nbr == 0)
		res[0] = '0';
	if (nbr < 0)
	{
		res[0] = '-';
		nbr = -nbr;
	}
	res[len] = '\0';
	while (nbr)
	{
		len--;
		res[len] = '0' + (nbr % 10);
		nbr /= 10;
	}
	return (res);
}
