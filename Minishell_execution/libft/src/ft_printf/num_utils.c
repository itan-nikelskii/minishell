/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   num_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 13:03:55 by acossari          #+#    #+#             */
/*   Updated: 2025/05/29 22:38:05 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

/*
** ft_numlen: compute the number of digits of a signed long integer
** @n: the number to measure (can be negative)
** Return: count of digits needed to represent n,
**         including space for '-' if n ≤ 0
*/
int	ft_numlen(long n)
{
	int	len;

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
** ft_unumlen: compute the number of digits of an unsigned long integer
**             in a given base
** @n: the unsigned number to measure
** @base: the numeral base (e.g., 10 for decimal, 16 for hexadecimal)
** Return: count of digits needed to represent n in the specified base
**         (at least 1)
*/
int	ft_unumlen(unsigned long n, int base)
{
	int	len;

	len = 0;
	if (n == 0)
		len = 1;
	while (n)
	{
		n /= base;
		len++;
	}
	return (len);
}

/*
** ft_utoa: convert an unsigned long integer to a decimal string
** @n: the unsigned number to convert
** Return: newly allocated null-terminated string representing n in base 10,
**         or NULL if malloc fails (caller must free)
*/
char	*ft_utoa(unsigned long n)
{
	int				len;
	char			*s;
	unsigned long	num;

	len = ft_unumlen(n, 10);
	s = malloc(len + 1);
	if (!s)
		return (NULL);
	s[len] = '\0';
	num = n;
	if (n == 0)
		s[0] = '0';
	while (num)
	{
		s[--len] = '0' + (num % 10);
		num /= 10;
	}
	return (s);
}

/*
** ft_utoa_base: convert an unsigned long integer to a string
**               in any base up to 16
** @n:    the unsigned number to convert
** @base: the target base (2 to 16)
** @spec: 'x' for lowercase hex digits, 'X' for uppercase hex digits,
**        ignored for non-hex
** Return: newly allocated null-terminated string of n in the specified base,
**         or NULL if malloc fails (caller must free)
*/
char	*ft_utoa_base(unsigned long n, int base, char spec)
{
	int				len;
	char			*s;
	unsigned long	num;
	char			*digits;

	if (spec == 'X')
		digits = "0123456789ABCDEF";
	else
		digits = "0123456789abcdef";
	len = ft_unumlen(n, base);
	s = malloc(len + 1);
	if (!s)
		return (NULL);
	s[len] = '\0';
	num = n;
	if (n == 0)
		s[0] = '0';
	while (num)
	{
		s[--len] = digits[num % base];
		num /= base;
	}
	return (s);
}
