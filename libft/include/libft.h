/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inikelsk <inikelsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 16:08:02 by acossari          #+#    #+#             */
/*   Updated: 2025/11/12 10:31:35 by inikelsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <stddef.h>   /* size_t */
# include <stdlib.h>   /* malloc, free */
# include <unistd.h>   /* write */
# include <stdarg.h>   /* va_list */

/*
** Bonus: Linked List Structure 
*/

/* Defines a single-linked list node */
typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}				t_list;

/*
** Part 1: Core Functions
*/

/* Character checks */
int		ft_isalnum(int c);
int		ft_isalpha(int c);
int		ft_isascii(int c);
int		ft_isdigit(int c);
int		ft_isprint(int c);
int		ft_isspace(char c);

/* Memory operations */
void	ft_bzero(void *s, size_t n);
void	*ft_memset(void *b, int c, size_t len);
void	*ft_memchr(const void *s, int c, size_t n);
void	*ft_memcpy(void *dst, const void *src, size_t n);
void	*ft_memmove(void *dst, const void *src, size_t len);
int		ft_memcmp(const void *s1, const void *s2, size_t n);

/* String operations */
size_t	ft_strlen(const char *s);
size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize);
size_t	ft_strlcat(char *dst, const char *src, size_t dstsize);
char	*ft_strchr(const char *s, int c);
char	*ft_strrchr(const char *s, int c);
int		ft_strcmp(const char *s1, const char *s2);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_strnstr(const char *haystack, const char *needle, size_t len);

/* Conversion */
int		ft_atoi(const char *str);
int		ft_toupper(int c);
int		ft_tolower(int c);

/* Allocation */
void	*ft_calloc(size_t count, size_t size);
void	*ft_realloc(void *ptr, size_t size);
char	*ft_strdup(const char *s1);

/*
** Part 2: Utility Functions
*/

/* String construction */
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_strtrim(char const *s1, char const *set);
char	**ft_split(char const *s, char c);
char	*ft_itoa(int n);

/* String iteration/mapping */
char	*ft_strmapi(char const *s, char (*f)(unsigned int, char));
void	ft_striteri(char *s, void (*f)(unsigned int, char *));

/* Output to file descriptor */
void	ft_putchar_fd(char c, int fd);
void	ft_putstr_fd(char *s, int fd);
void	ft_putendl_fd(char *s, int fd);
void	ft_putnbr_fd(int n, int fd);

/*
** Part 3: Bonus
*/

t_list	*ft_lstnew(void *content);
void	ft_lstadd_front(t_list **lst, t_list *new);
int		ft_lstsize(t_list *lst);
t_list	*ft_lstlast(t_list *lst);
void	ft_lstadd_back(t_list **lst, t_list *new);
void	ft_lstdelone(t_list *lst, void (*del)(void *));
void	ft_lstclear(t_list **lst, void (*del)(void *));
void	ft_lstiter(t_list *lst, void (*f)(void *));
t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *));

/*
** ft_printf
*/

/* ft_printf structure */
typedef struct s_format
{
	char	flags[6];
	int		width;
	int		precision;
	char	specifier;
}				t_format;

/* Main ft_printf function */
int		ft_printf(const char *format, ...);

/* ft_printf internal functions */
int		parse_format(const char *format, va_list *ap);
int		dispatch(t_format *fmt, va_list *ap);

/* Handler functions */
int		handle_char(t_format *fmt, va_list *ap);
int		handle_string(t_format *fmt, va_list *ap);
int		handle_ptr(t_format *fmt, va_list *ap);
int		handle_int(t_format *fmt, va_list *ap);
int		handle_unsigned(t_format *fmt, va_list *ap);
int		handle_hex(t_format *fmt, va_list *ap);
int		handle_percent(t_format *fmt, va_list *ap);

/* Utility functions */
int		ft_numlen(long n);
int		ft_unumlen(unsigned long n, int base);
char	*ft_utoa(unsigned long n);
char	*ft_utoa_base(unsigned long n, int base, char spec);

/*
** get_next_line
*/

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

# ifndef INITIAL_CAP_SIZE
#  define INITIAL_CAP_SIZE BUFFER_SIZE
# endif

# ifndef MAX_FD
#  define MAX_FD 1024
# endif

typedef struct s_buffer
{
	char	*data;
	size_t	len;
	size_t	cap;
}	t_buffer;

/* Main get_next_line function */
char	*ft_get_next_line(int fd);
void	ft_gnl_clear(int fd);

/* get_next_line utility functions */
int		init_stash(t_buffer *buf);
int		append_to_stash(t_buffer *buf, char *data, size_t len);
int		find_newline(char *data, size_t len);
void	clear_stash(t_buffer *stash);

#endif /* LIBFT_H */
