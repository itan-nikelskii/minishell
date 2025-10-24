/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acossari <acossari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:10:15 by acossari          #+#    #+#             */
/*   Updated: 2025/05/14 13:29:14 by acossari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/*
** ft_strlcat: append src to end of dst, up to dstsize-1
**              NUL-terminating if space permits
** @dst: destination buffer (NUL-terminated on entry)
** @src: string to append
** @dstsize: total size of dst buffer
** Return: initial dst length + src length;
**         if return ≥ dstsize, truncation occurred
*/
size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	d_len;
	size_t	s_len;
	size_t	i;

	d_len = ft_strlen(dst);
	s_len = ft_strlen(src);
	if (d_len >= dstsize)
		return (dstsize + s_len);
	i = 0;
	while (d_len + i + 1 < dstsize && src[i])
	{
		dst[d_len + i] = src[i];
		i++;
	}
	dst[d_len + i] = '\0';
	return (d_len + s_len);
}
