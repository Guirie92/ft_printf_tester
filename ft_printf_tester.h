/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_tester.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guillsan <guillsan@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 13:56:10 by guillsan          #+#    #+#             */
/*   Updated: 2025/11/14 02:45:36 by guillsan         ###   ########.fr       */
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
# define CLR_CYAN    "\x1b[36m"
# define CLR_GOLD    "\033[38;2;255;215;0m"

# define CAP_SIZE       8192
# define CAP_PARAM_STR  5
# define CAP_PARAM_PTR  4
# define CAP_PARAM_INT  7
# define CAP_PARAM_UINT 6

# define SEPARATOR ','
# define GROUP_SIZE 3

#ifndef TEST_SPECIFIER
# define TEST_SPECIFIER ""
#endif

# define TEST_TYPE(spec) (strcmp(TEST_SPECIFIER, "") == 0 || strcmp(TEST_SPECIFIER, spec) == 0)

# ifndef TEST_OCTAL
#  define TEST_OCTAL 1
# endif

# ifndef TEST_FLAGS
#  define TEST_FLAGS 1
# endif

# ifndef TEST_INVALID
#  define TEST_INVALID 1
# endif

# ifndef TEST_DUPLICATES
#  define TEST_DUPLICATES 1
# endif

# ifndef VERBOSE
#  define VERBOSE 0
# endif

# ifndef PRINT_OK
#  define PRINT_OK 0
# endif

typedef struct s_test_data
{
	size_t	test_ok;
	size_t	test_ko;
	size_t	b_flush;
	float	flush_seconds;

	const char *param_str_names[CAP_PARAM_STR];
	const char *param_ptr_names[CAP_PARAM_PTR];
	const char *param_int_names[CAP_PARAM_INT];
	const char *param_uint_names[CAP_PARAM_UINT];
	
	const char *params_str[CAP_PARAM_STR];
	const void *params_ptr[CAP_PARAM_PTR];
	const int *params_int[CAP_PARAM_INT];
	const unsigned int *params_uint[CAP_PARAM_UINT];
	
	int widths[6];
	int precs[6];
}	t_test_data;

typedef enum e_param_type
{
	ETYPE_CHAR,
	ETYPE_STR,
	ETYPE_VOID,
	ETYPE_INT,
	ETYPE_UINT
}	t_param_type;

#if VERBOSE && PRINT_OK

#define RUN_TEST(data, desc, fmt, ...)                                         \
do                                                                             \
{                                                                              \
	char out_ft[CAP_SIZE], out_libc[CAP_SIZE];                                 \
	int p[2];                                                                  \
	ssize_t n;                                                                 \
	int r_ft, r_libc;                                                          \
	int saved_stdout = dup(STDOUT_FILENO);                                     \
	if (pipe(p) == -1) { perror("pipe"); exit(1); }                            \
                                                                               \
	/* Capture ft_printf */                                                    \
	dup2(p[1], STDOUT_FILENO); close(p[1]);                                    \
	r_ft = ft_printf(fmt, ##__VA_ARGS__);                                      \
	fflush(stdout);                                                            \
	dup2(saved_stdout, STDOUT_FILENO); close(saved_stdout);                    \
	n = read(p[0], out_ft, CAP_SIZE -1 );                                      \
	if (n < 0) n = 0; out_ft[n] = '\0'; close(p[0]);                           \
	                                                                           \
	/* Capture libc printf */                                                  \
	if (pipe(p) == -1) { perror("pipe"); exit(1); }                            \
	saved_stdout = dup(STDOUT_FILENO);                                         \
	dup2(p[1], STDOUT_FILENO); close(p[1]);                                    \
	r_libc = printf(fmt, ##__VA_ARGS__);                                       \
	fflush(stdout);                                                            \
	dup2(saved_stdout, STDOUT_FILENO); close(saved_stdout);                    \
	n = read(p[0], out_libc, CAP_SIZE-1);                                      \
	if (n < 0) n = 0; out_libc[n] = '\0'; close(p[0]);                         \
	                                                                           \
	/* Compare */                                                              \
	int ok = 0;                                                                \
	if (r_ft == r_libc)                                                        \
	{                                                                          \
		if (r_ft >= 0 && (memcmp(out_ft, out_libc, (size_t)r_ft) == 0))        \
			ok = 1;                                                            \
		if (r_ft < 0 && (memcmp(out_ft, out_libc, 1) == 0))                    \
			ok = 1;                                                            \
	}                                                                          \
	if (ok)                                                                    \
	{                                                                          \
		printf(CLR_GREEN "[PASS]   fmt = %s  (%s)\n" CLR_RST, (fmt), (desc));  \
		printf("  ft_printf  : %s\n", out_ft);                                 \
		printf("  libc printf: %s\n", out_libc);                               \
		printf("  ft (%d) | lb (%d)\n", r_ft, r_libc);                         \
		fflush(stdout);                                                        \
		(data)->test_ok++;                                                     \
	}                                                                          \
	else                                                                       \
	{                                                                          \
		printf(CLR_RED "[FAIL]   fmt = %s  (%s)\n" CLR_RST, (fmt), (desc));    \
		printf("  ft_printf  : %s\n", out_ft);                                 \
		printf("  libc printf: %s\n", out_libc);                               \
		if (r_ft != r_libc)                                                    \
			printf("  ft (" CLR_RED "%d" CLR_RST") | "                         \
 				"lb (" CLR_GREEN "%d" CLR_RST ")\n", r_ft, r_libc);            \
		else                                                                   \
			printf("  ft (%d) | lb (%d)\n", r_ft, r_libc);                     \
		fflush(stdout);                                                        \
		(data)->test_ko++;                                                     \
	}                                                                          \
} while (0)

#elif VERBOSE

#define RUN_TEST(data, desc, fmt, ...)                                         \
do                                                                             \
{                                                                              \
	char out_ft[CAP_SIZE], out_libc[CAP_SIZE];                                 \
	int p[2];                                                                  \
	ssize_t n;                                                                 \
	int r_ft, r_libc;                                                          \
	int saved_stdout = dup(STDOUT_FILENO);                                     \
	if (pipe(p) == -1) { perror("pipe"); exit(1); }                            \
                                                                               \
	/* Capture ft_printf */                                                    \
	dup2(p[1], STDOUT_FILENO); close(p[1]);                                    \
	r_ft = ft_printf(fmt, ##__VA_ARGS__);                                      \
	fflush(stdout);                                                            \
	dup2(saved_stdout, STDOUT_FILENO); close(saved_stdout);                    \
	n = read(p[0], out_ft, CAP_SIZE -1 );                                      \
	if (n < 0) n = 0; out_ft[n] = '\0'; close(p[0]);                           \
	                                                                           \
	/* Capture libc printf */                                                  \
	if (pipe(p) == -1) { perror("pipe"); exit(1); }                            \
	saved_stdout = dup(STDOUT_FILENO);                                         \
	dup2(p[1], STDOUT_FILENO); close(p[1]);                                    \
	r_libc = printf(fmt, ##__VA_ARGS__);                                       \
	fflush(stdout);                                                            \
	dup2(saved_stdout, STDOUT_FILENO); close(saved_stdout);                    \
	n = read(p[0], out_libc, CAP_SIZE-1);                                      \
	if (n < 0) n = 0; out_libc[n] = '\0'; close(p[0]);                         \
	                                                                           \
	/* Compare */                                                              \
	int ok = 0;                                                                \
	if (r_ft == r_libc)                                                        \
	{                                                                          \
		if (r_ft >= 0 && (memcmp(out_ft, out_libc, (size_t)r_ft) == 0))        \
			ok = 1;                                                            \
		if (r_ft < 0 && (memcmp(out_ft, out_libc, 1) == 0))                    \
			ok = 1;                                                            \
	}                                                                          \
	if (ok)                                                                    \
		(data)->test_ok++;                                                     \
	else                                                                       \
	{                                                                          \
		printf(CLR_RED "[FAIL]   fmt = %s  (%s)\n" CLR_RST, (fmt), (desc));    \
		printf("  ft_printf  : %s\n", out_ft);                                 \
		printf("  libc printf: %s\n", out_libc);                               \
		if (r_ft != r_libc)                                                    \
			printf("  ft (" CLR_RED "%d" CLR_RST") | "                         \
 				"lb (" CLR_GREEN "%d" CLR_RST ")\n", r_ft, r_libc);            \
		else                                                                   \
			printf("  ft (%d) | lb (%d)\n", r_ft, r_libc);                     \
		fflush(stdout);                                                        \
		(data)->test_ko++;                                                     \
	}                                                                          \
} while (0)

#else

#define RUN_TEST(data, desc, fmt, ...)                                         \
do                                                                             \
{                                                                              \
	char out_ft[CAP_SIZE], out_libc[CAP_SIZE];                                 \
	int p[2];                                                                  \
	ssize_t n;                                                                 \
	int r_ft, r_libc;                                                          \
	int saved_stdout = dup(STDOUT_FILENO);                                     \
	if (pipe(p) == -1) { perror("pipe"); exit(1); }                            \
                                                                               \
	/* Capture ft_printf */                                                    \
	dup2(p[1], STDOUT_FILENO); close(p[1]);                                    \
	r_ft = ft_printf(fmt, ##__VA_ARGS__);                                      \
	fflush(stdout);                                                            \
	dup2(saved_stdout, STDOUT_FILENO); close(saved_stdout);                    \
	n = read(p[0], out_ft, CAP_SIZE -1 );                                      \
	if (n < 0) n = 0; out_ft[n] = '\0'; close(p[0]);                           \
	                                                                           \
	/* Capture libc printf */                                                  \
	if (pipe(p) == -1) { perror("pipe"); exit(1); }                            \
	saved_stdout = dup(STDOUT_FILENO);                                         \
	dup2(p[1], STDOUT_FILENO); close(p[1]);                                    \
	r_libc = printf(fmt, ##__VA_ARGS__);                                       \
	fflush(stdout);                                                            \
	dup2(saved_stdout, STDOUT_FILENO); close(saved_stdout);                    \
	n = read(p[0], out_libc, CAP_SIZE-1);                                      \
	if (n < 0) n = 0; out_libc[n] = '\0'; close(p[0]);                         \
	                                                                           \
	/* Compare */                                                              \
	int ok = 0;                                                                \
	if (r_ft == r_libc)                                                        \
	{                                                                          \
		if (r_ft >= 0 && (memcmp(out_ft, out_libc, (size_t)r_ft) == 0))        \
			ok = 1;                                                            \
		if (r_ft < 0 && (memcmp(out_ft, out_libc, 1) == 0))                    \
			ok = 1;                                                            \
	}                                                                          \
	if (ok)                                                                    \
		(data)->test_ok++;                                                     \
	else                                                                       \
		(data)->test_ko++;                                                     \
} while (0)

#endif

#endif /* FT_PRINTF_TESTER_H */