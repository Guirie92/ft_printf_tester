/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_tester.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guillsan <guillsan@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 13:56:10 by guillsan          #+#    #+#             */
/*   Updated: 2025/11/12 14:01:02 by guillsan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_TESTER_H
# define FT_PRINTF_TESTER_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdarg.h>
# include <limits.h>

# define CLR_RST     "\x1b[0m"
# define CLR_GREEN   "\x1b[32m"
# define CLR_RED     "\x1b[31m"
# define CLR_YELLOW  "\033[0;33m"

# define CAP_SIZE 8192

# ifndef TEST_OCTAL
#  define TEST_OCTAL 1
# endif

# ifndef TEST_FLAGS
#  define TEST_FLAGS 1
# endif

# ifndef TEST_FLAGS_INVALID
#  define TEST_FLAGS_INVALID 1
# endif

typedef struct s_test_data
{
	size_t	test_ok;
	size_t	test_ko;
}	t_test_data;

#endif /* FT_PRINTF_TESTER_H */