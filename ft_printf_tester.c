/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_tester.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guillsan <guillsan@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 13:12:33 by guillsan          #+#    #+#             */
/*   Updated: 2025/11/14 02:45:16 by guillsan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf_tester.h"

static char* print_result(t_test_data *data, size_t total_ok, size_t total_ko,
	const char *spec);
static void fmt_nbr(char *buffer, size_t n);
static void	init_test_data(t_test_data *data);
static void	test_flags_exhaustive(t_test_data *data, const char *spec, 
	size_t param_size, const void *params_in, const char **param_names,
	t_param_type ptype);
static void	test_specifiers_mandatory(t_test_data *data);
static void	test_mixed(t_test_data *data);
static void	test_printf_c(t_test_data *data);
static void	flush_sleep(t_test_data *data);

int main(void)
{
	t_test_data data;
	
	init_test_data(&data);

	printf("\n===== TESTING " CLR_CYAN "[mandatory]" CLR_RST " =====\n");

	test_specifiers_mandatory(&data);

	test_mixed(&data);

	printf("\n===== TESTING " CLR_CYAN "[bonus]" CLR_RST " =====\n");

#if TEST_FLAGS

	if (TEST_TYPE("c"))
	{
		test_flags_exhaustive(&data, "c", CAP_PARAM_UINT, (void *)data.params_uint, data.param_uint_names, ETYPE_UINT);
		test_flags_exhaustive(&data, "c", CAP_PARAM_INT, (void *)data.params_int, data.param_uint_names, ETYPE_INT);
	}
	if (TEST_TYPE("s"))
		test_flags_exhaustive(&data, "s", CAP_PARAM_STR, data.params_str, data.param_str_names, ETYPE_STR);
	if (TEST_TYPE("p"))
	{
		test_percent_module(&data);
		test_flags_exhaustive(&data, "p", CAP_PARAM_PTR, (void *)data.params_ptr, data.param_ptr_names, ETYPE_VOID);
	}
	if (TEST_TYPE("d"))
		test_flags_exhaustive(&data, "d", CAP_PARAM_INT, (void *)data.params_int, data.param_int_names, ETYPE_INT);
	if (TEST_TYPE("i"))
		test_flags_exhaustive(&data, "i", CAP_PARAM_INT, (void *)data.params_int, data.param_int_names, ETYPE_INT);
	if (TEST_TYPE("u"))
		test_flags_exhaustive(&data, "u", CAP_PARAM_UINT, (void *)data.params_uint, data.param_uint_names, ETYPE_UINT);
	
	if (TEST_TYPE("x"))
		test_flags_exhaustive(&data, "x", CAP_PARAM_UINT, (void *)data.params_uint, data.param_uint_names, ETYPE_UINT);
	if (TEST_TYPE("X"))
		test_flags_exhaustive(&data, "X", CAP_PARAM_UINT, (void *)data.params_uint, data.param_uint_names, ETYPE_UINT);
		
# if TEST_OCTAL

	if (TEST_TYPE("o"))
		test_flags_exhaustive(&data, "o", CAP_PARAM_UINT, (void *)data.params_uint, data.param_uint_names, ETYPE_UINT);
# endif

	//test_duplicates_and_invalid

#endif
	
	return (0);
}

static void init_test_data(t_test_data *data)
{
	memset(data, 0, sizeof(t_test_data));

	/* --- 1. Scalar Overwrites --- */
	data->flush_seconds = 0.02;

	/* --- 2. Initialize flags arrays --- */
	const int widths[] = { 0, 1, 2, 5, 10, 50 };
	const int precs[] = { -1, 0, 1, 3, 5, 20 };
	memcpy(data->widths, widths, sizeof(widths));
	memcpy(data->precs, precs, sizeof(precs));
	
	/* --- 3. Initialize str arrays --- */
	const char *param_str_names[] = {
		"ab",
		"hello",
		"abcdefghijklmnopqrstuvwxyz",
		"",
		"(char *)NULL",
		"6th" };
	const char *params_str[] = {
		"ab",
		"hello",
		"abcdefghijklmnopqrstuvwxyz",
		"", (char *)NULL,
		"6th" };
	memcpy(data->params_str, params_str, sizeof(params_str));
	memcpy(data->param_str_names, param_str_names, sizeof(param_str_names));

	/* --- 4. Initialize ptr arrays --- */
	const char *param_ptr_names[] = {
		"(void *)&params_str[0]",
		"(void *)&params_str[1]",
		"(void *)&param_ptr_names[0]",
		"(const void *)NULL"
	};
	const void *params_ptr[] = {
		(const void *)&params_str[0],
		(const void *)&params_str[1],
		(const void *)&param_ptr_names[0],
		(const void *)NULL 
	};
	memcpy(data->params_ptr, params_ptr, sizeof(params_ptr));
	memcpy(data->param_ptr_names, param_ptr_names, sizeof(param_ptr_names));

	/* --- 5. Initialize int arrays --- */
	const char *param_int_names[] = {
		"0",
		"1",
		"-1",
		"42",
		"-42",
		"INT_MAX",
		"INT_MIN",
		//"2147483647789"
	};
	const int params_int[] = {
		0,
		1,
		-1,
		42,
		-42,
		INT_MAX,
		INT_MIN,
		//2147483647789L
	};
	memcpy(data->params_int, params_int, sizeof(params_int));
	memcpy(data->param_int_names, param_int_names, sizeof(param_int_names));

	/* --- 6. Initialize uint arrays --- */
	const char *param_uint_names[] = {
		"0u",
		"1u",
		"42u",
		"UINT_MAX",
		"4294967295u",
		"0xDEADBEEFu",
		//2147483647789L
	};
	const unsigned int params_uint[] = {
		0u,
		1u,
		42u,
		UINT_MAX,
		4294967295u,
		0xDEADBEEFu,
		//2147483647789L
	};
	memcpy(data->params_uint, params_uint, sizeof(params_uint));
	memcpy(data->param_uint_names, param_uint_names, sizeof(param_uint_names));
}


static void flush_sleep(t_test_data *data)
{
    if (!data->b_flush)
        return;
    fflush(stdout);
    usleep((data->flush_seconds * 1000) * 1000); /* 40ms by default */
}

static const char flag_chars[5] = { '#', '+', ' ', '-', '0' };

/* helper: build canonical flags string for mask into out (must be large enough) */
static void build_flags_str(int mask, char *out, size_t out_size)
{
	size_t p = 0;
	memset(out, 0, out_size);
	for (int i = 0; i < 5 && p + 1 < out_size; ++i)
	{
		if (mask & (1 << i))
			out[p++] = flag_chars[i];
	}
	out[p] = '\0';
}
static void test_flags_exhaustive(t_test_data *data, const char *spec, 
	size_t param_size, const void *params, const char **param_names,
	t_param_type ptype)
{
	size_t total_ok = data->test_ok;
	size_t total_ko = data->test_ko;
	char flags_buf[8];
	char fmt[128];

	const char * const * params_str = NULL;
    const void * const * params_ptr = NULL;
    const int  * params_int  = NULL;
    const unsigned int * params_uint = NULL;

	params_str = (const char * const *) params;
    params_ptr = (const void * const *) params;
    params_int = (const int *) params;
    params_uint = (const unsigned int *) params;
	
	data->test_ok = 0;
	data->test_ko = 0;

	for (int mask = 0; mask < (1 << 5); ++mask) /* 1 << 5 -> 2^5 -> 32 */
	{
		build_flags_str(mask, flags_buf, sizeof(flags_buf)); /* now always NUL-terminated */

		/* 1) no width/precision: e.g. "|%#s|" or "|%s|" */
		if (flags_buf[0] == '\0')
			snprintf(fmt, sizeof(fmt), "|%%%s|", spec);
		else
			/* flags_buf may contain a space as a flag; that's valid in format */
			snprintf(fmt, sizeof(fmt), "|%%%s%s|", flags_buf, spec);

		for (size_t pi = 0; pi < param_size; ++pi)
		{
			if (ptype == ETYPE_STR)
				RUN_TEST(data, param_names[pi], fmt, (char *)params_str[pi]);
			else if (ptype == ETYPE_VOID)
				RUN_TEST(data, param_names[pi], fmt, (void *)params_ptr[pi]);
			else if (ptype == ETYPE_INT)
				RUN_TEST(data, param_names[pi], fmt, (int)params_int[pi]);
			else if (ptype == ETYPE_UINT)
				RUN_TEST(data, param_names[pi], fmt, (unsigned int)params_uint[pi]);
		}

		/* 2) width-only */
		for (size_t wi = 0; wi < sizeof(data->widths)/sizeof(data->widths[0]); ++wi)
		{
			int w = data->widths[wi];
			if (w <= 0) continue; /* width 0: skip, not meaningful */
			if (flags_buf[0] == '\0')
				snprintf(fmt, sizeof(fmt), "|%%%d%s|", w, spec);
			else
				snprintf(fmt, sizeof(fmt), "|%%%s%d%s|", flags_buf, w, spec);
			
			for (size_t pi = 0; pi < param_size; ++pi)
			{
				if (ptype == ETYPE_STR)
					RUN_TEST(data, param_names[pi], fmt, (char *)params_str[pi]);
				else if (ptype == ETYPE_VOID)
					RUN_TEST(data, param_names[pi], fmt, (void *)params_ptr[pi]);
				else if (ptype == ETYPE_INT)
					RUN_TEST(data, param_names[pi], fmt, (int)params_int[pi]);
				else if (ptype == ETYPE_UINT)
					RUN_TEST(data, param_names[pi], fmt, (unsigned int)params_uint[pi]);
			}
		}

		/* 3) precision-only (no width) */
		for (size_t pj = 0; pj < sizeof(data->precs)/sizeof(data->precs[0]); ++pj)
		{
			int pr = data->precs[pj];
			if (pr < 0) continue;
			if (flags_buf[0] == '\0')
				snprintf(fmt, sizeof(fmt), "|%%.%d%s|", pr, spec);
			else
				snprintf(fmt, sizeof(fmt), "|%%%s.%d%s|", flags_buf, pr, spec);
			
			for (size_t pi = 0; pi < param_size; ++pi)
			{
				if (ptype == ETYPE_STR)
					RUN_TEST(data, param_names[pi], fmt, (char *)params_str[pi]);
				else if (ptype == ETYPE_VOID)
					RUN_TEST(data, param_names[pi], fmt, (void *)params_ptr[pi]);
				else if (ptype == ETYPE_INT)
					RUN_TEST(data, param_names[pi], fmt, (int)params_int[pi]);
				else if (ptype == ETYPE_UINT)
					RUN_TEST(data, param_names[pi], fmt, (unsigned int)params_uint[pi]);
			}
		}

		/* 4) width + precision */
		for (size_t wi = 0; wi < sizeof(data->widths)/sizeof(data->widths[0]); ++wi) 
		{
			int w = data->widths[wi];
			for (size_t pj = 0; pj < sizeof(data->precs)/sizeof(data->precs[0]); ++pj)
			{
				int pr = data->precs[pj];
				
				/* build fmt carefully — ensure w and pr are set */
				if (w <= 0 && pr < 0) continue;
				
				if (pr < 0) {
					/* width only (we already did width-only above but keep for completeness) */
					if (flags_buf[0] == '\0')
						snprintf(fmt, sizeof(fmt), "|%%%d%s|", w, spec);
					else
						snprintf(fmt, sizeof(fmt), "|%%%s%d%s|", flags_buf, w, spec);
				} 
				else
				{
					if (flags_buf[0] == '\0')
						snprintf(fmt, sizeof(fmt), "|%%%d.%d%s|", w, pr, spec);
					else
						snprintf(fmt, sizeof(fmt), "|%%%s%d.%d%s|", flags_buf, w, pr, spec);
				}

				for (size_t pi = 0; pi < param_size; ++pi)
				{
					if (ptype == ETYPE_STR)
						RUN_TEST(data, param_names[pi], fmt, (char *)params_str[pi]);
					else if (ptype == ETYPE_VOID)
						RUN_TEST(data, param_names[pi], fmt, (void *)params_ptr[pi]);
					else if (ptype == ETYPE_INT)
						RUN_TEST(data, param_names[pi], fmt, (int)params_int[pi]);
					else if (ptype == ETYPE_UINT)
						RUN_TEST(data, param_names[pi], fmt, (unsigned int)params_uint[pi]);
					}
			}
		}
	}

	print_result(data, total_ok, total_ko, spec);
	
	flush_sleep(data);
}

static void test_printf_c(t_test_data *data)
{
	size_t total_ok = data->test_ok;
	size_t total_ko = data->test_ko;
	
	data->test_ok = 0;
	data->test_ko = 0;

	//for (int i = -5; i <= 260; i++)
	for (int i = -5; i <= 1024; i++)
	{
		unsigned char ch = (unsigned char)i;
		fflush(stdout);
		if (isprint(ch))
			RUN_TEST(data, "%c", "|%c|", ch);
		else
		{
			
#if TEST_FLAGS
			RUN_TEST(data, "\\x%02X", "|x%02X|", ch);
#else
			RUN_TEST(data, "%c", "|%c|", ch);
#endif
		}
	}

#if TEST_FLAGS

	RUN_TEST(data, "0", "|x%02X|", 0);
	RUN_TEST(data, "-1", "|x%02X|", -1);
	RUN_TEST(data, "256", "|x%02X|", 256);
	RUN_TEST(data, "1024", "|x%02X|", 1024);
	RUN_TEST(data, "0x10FFFF", "|x%02X|", 0x10FFFF);
#else
	RUN_TEST(data, "0", "|%c|", 0);
	RUN_TEST(data, "-1", "|%c|", -1);
	RUN_TEST(data, "256", "|%c|", 256);
	RUN_TEST(data, "1024", "|%c|", 1024);
	RUN_TEST(data, "0x10FFFF", "|%c|", 0x10FFFF);
#endif

	print_result(data, total_ok, total_ko, "c (ascii-all)");

	flush_sleep(&data);
}

static void run_basic_spec_for(t_test_data *data,
    const char *spec, size_t param_size,
    const void *params, const char **param_names,
    t_param_type ptype)
{
	char fmt[128];
	size_t total_ok = data->test_ok;
	size_t total_ko = data->test_ko;
	
	data->test_ok = 0;
	data->test_ko = 0;
	
	/* simple: no flags, no width, no precision */
	snprintf(fmt, sizeof(fmt), "|%%%s|", spec);
	
	/* typed views */
	const char * const * params_str   = (const char * const *) params;
	const void * const * params_ptr   = (const void * const *) params;
	const int * params_int            = (const int *) params;
	const unsigned int * params_uint  = (const unsigned int *) params;
	
	for (size_t i = 0; i < param_size; ++i) {
		switch (ptype) {
			case ETYPE_STR:
				RUN_TEST(data, param_names[i], fmt, (char *) params_str[i]);
				break;
			case ETYPE_VOID:
				RUN_TEST(data, param_names[i], fmt, (void *) params_ptr[i]);
				break;
			case ETYPE_INT:
				RUN_TEST(data, param_names[i], fmt, (int) params_int[i]);
				break;
			case ETYPE_UINT:
				RUN_TEST(data, param_names[i], fmt, (unsigned int) params_uint[i]);
				break;
			case ETYPE_CHAR:
				/* treat %c as int (promoted), params must be int[] */
				RUN_TEST(data, param_names[i], fmt, (int) params_int[i]);
				break;
			default:
				/* fallback: print as string if something unexpected */
				RUN_TEST(data, param_names[i], fmt, (char *) params_str[i]);
		}
	}

	print_result(data, total_ok, total_ko, spec);
	flush_sleep(&data);
}

/* Mandatory specifiers only (no flags/FWM/prec) */
static void test_specifiers_mandatory(t_test_data *data)
{
	
	if (TEST_TYPE("c"))
	{
		test_printf_c(data);
		run_basic_spec_for(data, "c", CAP_PARAM_INT, (const void *)data->params_int, data->param_int_names, ETYPE_CHAR);
	}
	
	if (TEST_TYPE("s"))
		run_basic_spec_for(data, "s", CAP_PARAM_STR, (const void *)data->params_str, data->param_str_names, ETYPE_STR);

	if (TEST_TYPE("p"))
	{
		RUN_TEST(data, "(void *)NULL", "|%%|", (void *)NULL);
		run_basic_spec_for(data, "p", CAP_PARAM_PTR, (const void *)data->params_ptr, data->param_ptr_names, ETYPE_VOID);
	}

	if (TEST_TYPE("d"))
		run_basic_spec_for(data, "d", CAP_PARAM_INT, (const void *)data->params_int, data->param_int_names, ETYPE_INT);
	if (TEST_TYPE("i"))
		run_basic_spec_for(data, "i", CAP_PARAM_INT, (const void *)data->params_int, data->param_int_names, ETYPE_INT);

	if (TEST_TYPE("u"))
		run_basic_spec_for(data, "u", CAP_PARAM_UINT, (const void *)data->params_uint, data->param_uint_names, ETYPE_UINT);
	
	if (TEST_TYPE("x"))
		run_basic_spec_for(data, "x", CAP_PARAM_UINT, (const void *)data->params_uint, data->param_uint_names, ETYPE_UINT);
	if (TEST_TYPE("X"))
		run_basic_spec_for(data, "X", CAP_PARAM_UINT, (const void *)data->params_uint, data->param_uint_names, ETYPE_UINT);
	
#if TEST_OCTAL

	if (TEST_TYPE("o"))
		run_basic_spec_for(data, "o", CAP_PARAM_UINT, (const void *)data->params_uint, data->param_uint_names, ETYPE_UINT);
#endif
	
	/* percent is handled separately because it consumes no parameter; see function below. */
}

/* Small module that tests the literal '%%' behavior and some edge cases */
void test_percent_module(t_test_data *data)
{
	/* Format: literal percent, no parameter consumed. We'll pass (void *)NULL as the param
	 * and use the exact same string as desc so output shows what we invoked.
	 */
	RUN_TEST(data, "(void *)NULL", "|%%|", (void *)NULL);
	/* Extra combos: width, flags (some flags ignored for %% but test acceptance) */
	RUN_TEST(data, "(void *)NULL", "|%5%|", (void *)NULL);     /* width */
	RUN_TEST(data, "(void *)NULL", "|%-5%|", (void *)NULL);    /* left justify */
	RUN_TEST(data, "(void *)NULL", "|%05%|", (void *)NULL);    /* zero pad - should be harmless/ignored */
}

/* Small invalid/duplicate flag tests (lightweight) */
static void test_duplicates_and_invalid(t_test_data *data)
{
	/* duplicate flags for strings */
	RUN_TEST(data, "hello", "|%--10s|", "hello");
	RUN_TEST(data, "42", "|%00d|", 42);            /* duplicate zero flag on numbers */
	RUN_TEST(data, "42", "|%++d|", 42);            /* duplicate plus */
	RUN_TEST(data, "42", "|%+-5d|", 42);           /* conflicting flags order */
	RUN_TEST(data, "42", "|%-05d|", 42);           /* - should override 0 */

	/* weird spacing/ordering */
	RUN_TEST(data, "x", "|%#  - 10.3s|", "x");
	RUN_TEST(data, "foo", "|%0+#-12.4s|", "foo");

	/* invalid flags for a specifier (should be ignored but not crash) */
	RUN_TEST(data, "abc", "|%+s|", "abc"); /* + has no meaning for %s */
	RUN_TEST(data, "42", "|%#d|", 42);         /* # has no meaning for %d */

	/* repeated percent weirdness */
	RUN_TEST(data, "(void *)NULL", "|%%%|", (void *)NULL); /* malformed but test parser robustness */
}


static void test_mixed(t_test_data *data)
{
	size_t total_ok = data->test_ok;
	size_t total_ko = data->test_ko;
	
	data->test_ok = 0;
	data->test_ko = 0;
	
	/* --- SIMPLE conversions --- */
#if TEST_FLAGS

	RUN_TEST(data, "char basic", "|%.4s|", (char *)NULL);
#endif
	RUN_TEST(data, "A", "|%c|", 'A');
	RUN_TEST(data, "hello", "|%s|", "hello");
	RUN_TEST(data, "", "|%s|", "");
	RUN_TEST(data, "(char *)NULL", "|%s|", (char *)NULL);
	RUN_TEST(data, "end", "%%%% %%%% %s", "end");

	RUN_TEST(data, "(void *)&test_mixed", "|%p|", (void *)&test_mixed);
	
	
	/* pointer NULL */
	
	RUN_TEST(data, "(void *)0", "|%p|", (void *)0);

	RUN_TEST(data, "42", "|%d|", 42);
	RUN_TEST(data, "-42", "|%d|", -42);
	RUN_TEST(data, "0", "|%d|", 0);
	RUN_TEST(data, "INT_MAX", "|%d|", INT_MAX);
	RUN_TEST(data, "INT_MIN", "|%d|", INT_MIN);

	RUN_TEST(data, "-12345", "|%i|", -12345);

	RUN_TEST(data, "42u", "|%u|", 42u);
	RUN_TEST(data, "0u", "|%u|", 0u);
	RUN_TEST(data, "UINT_MAX", "|%u|", UINT_MAX);

	RUN_TEST(data, "305441741u", "|%x|", 305441741u);
	RUN_TEST(data, "305441741u", "|%X|", 305441741u);
	RUN_TEST(data, "0u", "|%x|", 0u);

#if TEST_INVALID

	RUN_TEST(data, "UINT_MAX", "|%u|", UINT_MAX + UINT_MAX/2);

	RUN_TEST(data, "305441741u", "|%x|", 305441748741u);
	RUN_TEST(data, "305441741u", "|%X|", 3054417412478293L);
	RUN_TEST(data, "0u", "|%x|", 3054417412478293L);
#endif

#if TEST_FLAGS

	/* --- WIDTH/TEST_FLAGS/PRECISION for integers --- */
	
	RUN_TEST(data, "42", "|%5d|", 42);
	

	RUN_TEST(data, "42", "|%-5d|", 42);
	RUN_TEST(data, "42", "|%05d|", 42);
	RUN_TEST(data, "42", "|%+d|", 42);
	RUN_TEST(data, "42", "|% d|", 42);
	RUN_TEST(data, "42", "|%+05d|", 42);
	RUN_TEST(data, "42", "|%-05d|", 42); /* behaviour: '-' wins */
	RUN_TEST(data, "42", "|%.5d|", 42);
	RUN_TEST(data, "123", "|%8.4d|", 123);
	RUN_TEST(data, "123", "|%8.-4d|", 123);
	RUN_TEST(data, "0", "|%.0d|", 0);
	
	/* negatives with flags */
	
	RUN_TEST(data, "-42", "|%05d|", -42);
	RUN_TEST(data, "-42", "|%.5d|", -42);
	RUN_TEST(data, "-42", "|%+d|", -42);


	/* --- unsigned/hex with flags --- */
	
	RUN_TEST(data, "1234u", "|%8u|", 1234u);
	RUN_TEST(data, "0x1234u", "|%#x|", 0x1234u);
	RUN_TEST(data, "0x1234u", "|%#08x|", 0x1234u);
	RUN_TEST(data, "0xABu", "|%#X|", 0xABu);
	
#if TEST_OCTAL

	RUN_TEST(data, "0123u", "|%#o|", 0123u);
	RUN_TEST(data, "0u", "|%#o|", 0u);
	
#endif


	/* precision for unsigned */
	
	RUN_TEST(data, "123u", "|%.6u|", 123u);
	RUN_TEST(data, "0x1au", "|%.6x|", 0x1au);

	/* --- strings with width/precision/flags --- */
	
	RUN_TEST(data, "abc", "|%10s|", "abc");
	RUN_TEST(data, "abc", "|%-10s|", "abc");
	RUN_TEST(data, "abcdef", "|%.3s|", "abcdef");
	RUN_TEST(data, "abcdef", "|%8.3s|", "abcdef");
	RUN_TEST(data, "hello", "|%.0s|", "hello");

#endif

	/* --- chars including NUL char (return counts matter) --- */
	
	RUN_TEST(data, "\\0", "|%c|", '\0'); /* output will contain NUL: harness reads raw bytes */

		
	/* --- multiple conversions in one format --- */
	
	RUN_TEST(data, "-7, 42u, 0xfaceu, \"str\", (void *)&printf", "A:%d B:%u C:%x S:%s P:%p E:%%", -7, 42u, 0xfaceu, "str", (void *)&printf);

#if TEST_FLAGS
	
	/* --- width & flags combos for %s (0 flag ignored) --- */
	
	RUN_TEST(data, "abc", "|%010s|", "abc");
	RUN_TEST(data, "abc", "|%+10s|", "abc"); /* + ignored for strings, libc might ignore */

	/* --- Boundary/edge values --- */
	
	RUN_TEST(data, "INT_MIN", "|%020d|", INT_MIN);
	RUN_TEST(data, "UINT_MAX", "|%X|", UINT_MAX);
	RUN_TEST(data, "42", "|%100d|", 42);
	
	/* --- Some tricky combinations with #-+ space and precision --- */
	
	RUN_TEST(data, "0u", "|%#x|", 0u);
	RUN_TEST(data, "0x1u", "|%#x|", 0x1u);
	RUN_TEST(data, "123", "|%+8.5d|", 123);
	RUN_TEST(data, "123", "|% 8d|", 123);

	/* --- multiple flags order permutations --- */
	
	RUN_TEST(data, "42", "|% -05d|", 42);
	RUN_TEST(data, "0x1abu", "|%#0+8x|", 0x1abu);

	/* --- large width/precision --- */
	
	RUN_TEST(data, "x", "|%500s|", "x");
	RUN_TEST(data, "abcdefghijklmnopqrstuvwxyz", "|%.200s|", "abcdefghijklmnopqrstuvwxyz");
	RUN_TEST(data, "845", "|%.40d|", 845);

#endif

	/* ---- Edge cases: values & simple flags ---- */
	
	RUN_TEST(data, "INT_MAX", "|%d|", INT_MAX);
	RUN_TEST(data, "INT_MIN", "|%d|", INT_MIN);
	RUN_TEST(data, "UINT_MAX", "|%u|", UINT_MAX);
	RUN_TEST(data, "0", "|%d|", 0);
	RUN_TEST(data, "0u", "|%u|", 0u);
	RUN_TEST(data, "-1", "|%d|", -1);
	RUN_TEST(data, "-2147483647", "|%i|", -2147483647);
	RUN_TEST(data, "'A'", "|%c|", 'A');
	RUN_TEST(data, "\\0", "|%c|", '\0');
	RUN_TEST(data, "(char *)NULL", "|%s|", (char *)NULL);
	RUN_TEST(data, "", "|%s|", "");
	RUN_TEST(data, "(void *)0", "|%p|", (void *)0);

	RUN_TEST(data, "NULL", "|%p|", NULL);

	
#if TEST_FLAGS
	RUN_TEST(data, "(char *)NULL", "|%10.6s|", (char *)NULL);

	RUN_TEST(data, "(char *)NULL", "|%10.3s|", (char *)NULL);
	
	RUN_TEST(data, "(char *)NULL", "|%-10.3s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%-10.6s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%-10.9s|", (char *)NULL);
	
	RUN_TEST(data, "(char *)NULL", "|%020.9s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%20.9s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%-20.9s|", (char *)NULL);
	
	RUN_TEST(data, "(char *)NULL", "|%020.5s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%20.5s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%-20.5s|", (char *)NULL);

	RUN_TEST(data, "(char *)NULL", "|%020.0s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%20.0s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%-20.0s|", (char *)NULL);

	RUN_TEST(data, "(char *)NULL", "|%020.s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%20.s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%-20.s|", (char *)NULL);

	RUN_TEST(data, "(char *)NULL", "|%02.s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%2.s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%-2.s|", (char *)NULL);

	RUN_TEST(data, "(char *)NULL", "|%02.6s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%2.6s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%-2.6s|", (char *)NULL);

	RUN_TEST(data, "(char *)NULL", "|% 02.6s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|% 2.6s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|% -2.6s|", (char *)NULL);

	
	RUN_TEST(data, "NULL", "|%p|", NULL);
	
	
	RUN_TEST(data, "NULL", "|%2p|", NULL);
	RUN_TEST(data, "NULL", "|%1p|", NULL);
	RUN_TEST(data, "NULL", "|%0p|", NULL);
	
	RUN_TEST(data, "ptr NULL", "|%.9p|", NULL);
	RUN_TEST(data, "ptr NULL", "|%020p|", NULL);
	RUN_TEST(data, "ptr NULL", "|%020.9p|", NULL);
	RUN_TEST(data, "ptr NULL", "|%+020.9p|", NULL);
	RUN_TEST(data, "(char *)NULL", "|%.9s|", (char *)NULL);

#endif


#if TEST_FLAGS

	RUN_TEST(data, "(char *)NULL", "|%2s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%1s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%0s|", (char *)NULL);
	
	RUN_TEST(data, "(char *)NULL", "|%020s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%20s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%-20s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%020.9s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%20.9s|", (char *)NULL);
	RUN_TEST(data, "(char *)NULL", "|%-20.9s|", (char *)NULL);
	
	/* ---- Width & precision edge combos (integers) ---- */
	
	RUN_TEST(data, "42", "|%0d|", 42);
	RUN_TEST(data, "42", "|%1d|", 42);
	RUN_TEST(data, "42", "|%2d|", 42);
	RUN_TEST(data, "42", "|%5d|", 42);
	RUN_TEST(data, "42", "|%10d|", 42);
	RUN_TEST(data, "42", "|%100d|", 42);
	RUN_TEST(data, "0", "|%.0d|", 0);
	RUN_TEST(data, "5", "|%.0d|", 5);
	RUN_TEST(data, "123", "|%.1d|", 123);
	RUN_TEST(data, "123", "|%.5d|", 123);
	RUN_TEST(data, "7", "|%6.3d|", 7);
	RUN_TEST(data, "7", "|%3.5d|", 7);
	
	/* ---- Zero / padding / left-justify conflicts ---- */
	
	RUN_TEST(data, "42", "|%05d|", 42);
	RUN_TEST(data, "42", "|%-5d|", 42);
	RUN_TEST(data, "42", "|%-05d|", 42);
	RUN_TEST(data, "42", "|%+05d|", 42);
	RUN_TEST(data, "42", "|% 05d|", 42);

#if TEST_INVALID

	RUN_TEST(data, "42", "|%0000005d|", 42);        /* repeated zero */
	RUN_TEST(data, "42", "|%-----5d|", 42);         /* repeated minus */
	RUN_TEST(data, "42", "|% -0-0 10d|", 42);       /* weird spacing & repeats */
#endif	
	/* ---- Plus / space flags and interactions ---- */
	
	RUN_TEST(data, "42", "|%+d|", 42);
	RUN_TEST(data, "-42", "|%+d|", -42);
	RUN_TEST(data, "42", "|% d|", 42);
	RUN_TEST(data, "42", "|%+ d|", 42);

#if TEST_DUPLICATES

	RUN_TEST(data, "42", "|%++d|", 42);
#endif

	RUN_TEST(data, "42", "|%  d|", 42);

	/* ---- Hash (#) alternate form tests ---- */
	
	RUN_TEST(data, "0x1a3", "|%#x|", 0x1a3);
	RUN_TEST(data, "0u", "|%#x|", 0u);
	RUN_TEST(data, "0x1a3", "|%#X|", 0x1a3);
	
#if TEST_OCTAL

	RUN_TEST(data, "0123u", "|%#o|", 0123u);
	RUN_TEST(data, "0u", "|%#o|", 0u);
	
#endif

	RUN_TEST(data, "0x7f", "|%#08x|", 0x7f);
	RUN_TEST(data, "0x7f", "|%#.5x|", 0x7f);

	
	/* ---- Repeated/strange flags combos ---- */
	
#if TEST_DUPLICATES
	RUN_TEST(data, "123", "|%--00-8d|", 123);
	RUN_TEST(data, "123", "|%+++0008d|", 123);
	RUN_TEST(data, "0x2b", "|%#0- +10.5x|", 0x2b);
	RUN_TEST(data, "7", "|%##--++00 00#10.3d|", 7);
#endif
	/* ---- Width / precision extremes (stress) ---- */
	
	RUN_TEST(data, "1", "|%200d|", 1);
	RUN_TEST(data, "1", "|%.200d|", 1);
	RUN_TEST(data, "1", "|%200.200d|", 1);
	RUN_TEST(data, "1", "|%1000d|", 1);
	

	/* ---- unsigned / hex / octal edge cases ---- */
	
	RUN_TEST(data, "0u", "|%.0u|", 0u);
	RUN_TEST(data, "123u", "|%.2u|", 123u);
	RUN_TEST(data, "0u", "|%.0X|", 0u);
	RUN_TEST(data, "0xabc", "|%#12x|", 0xabc);
	
#if TEST_OCTAL

	RUN_TEST(data, "0777u", "|%#8o|", 0777u);
	
#endif
	
	/* ---- Strings: precision, width, NULL behavior ---- */
	
	RUN_TEST(data, "abcdef", "|%.3s|", "abcdef");
	RUN_TEST(data, "abcdef", "|%8.3s|", "abcdef");
	RUN_TEST(data, "abc", "|%010s|", "abc");
	RUN_TEST(data, "abc", "|%+10s|", "abc");
	RUN_TEST(data, "hello", "|% -+0#10.5s|", "hello");
	RUN_TEST(data, "hello", "|%.0s|", "hello");
	
	/* ---- Char (%c) special cases ---- */
	
	RUN_TEST(data, "c'Z'", "|%5c|", 'Z');
	RUN_TEST(data, "'Z'", "|%-5c|", 'Z');
	RUN_TEST(data, "'Z'", "|%05c|", 'Z');
	RUN_TEST(data, "'A', '\\0', 'B'", "|%c-%c-%c|", 'A', '\0', 'B');
	
	RUN_TEST(data, "'V'", "|%.2c|", 'V');
	RUN_TEST(data, "'a'", "|%.0c|", 'a');
	RUN_TEST(data, "'\\t'", "|%.1c|", '\t');
	RUN_TEST(data, "'b'", "|%.20c|", 'b');
	RUN_TEST(data, "';'", "|%-20c|", ';');
	RUN_TEST(data, "'='", "|%0-20.2c|", '=');
	RUN_TEST(data, "'n'", "|%#8.20c|", 'n');
	
	/* ---- Percent literal with flags (should print % or behave like libc) ---- */
	
	RUN_TEST(data, "", "|%%|");
	RUN_TEST(data, "", "|%5%|");
	RUN_TEST(data, "", "|%-5%|");
	RUN_TEST(data, "", "|%05%|");
	RUN_TEST(data, "", "|%+0#-5%|");
	RUN_TEST(data, "0x7b", "|%0#-+ 10.5x|", 0x7b);

#endif

	/* ---- Mixed sequences & argument order checks ---- */

	RUN_TEST(data, "-1, 4294967295u, 0xdead", "|%d %u %x|", -1, 4294967295u, 0xdead);
	RUN_TEST(data, "\"hola\", '!', -7, 255u, 0x2a, (void *)&printf)", "|%s|%c|%d|%u|%x|%p|", "hola", '!', -7, 255u, 0x2a, (void *)&printf);
	
#if TEST_FLAGS

	/* ---- Precision and rounding-like integer edge behaviors ---- */
	
	RUN_TEST(data, "12345", "|%.2d|", 12345);
	RUN_TEST(data, "123", "|%.3d|", 123);
	RUN_TEST(data, "123", "|%.8d|", 123);
	
	/* ---- Boundary negatives combined with flags/width/prec ---- */
	
	RUN_TEST(data, "INT_MIN", "|%20d|", INT_MIN);
	RUN_TEST(data, "INT_MIN", "|%020d|", INT_MIN);
	RUN_TEST(data, "INT_MIN", "|%.15d|", INT_MIN);
	RUN_TEST(data, "INT_MIN", "|%+ 20.10d|", INT_MIN);

	RUN_TEST(data, "INT_MIN", "|%020.20d|", INT_MIN);
	RUN_TEST(data, "INT_MIN", "|%021.20d|", INT_MIN);
	RUN_TEST(data, "INT_MIN", "|%020.21d|", INT_MIN);
	RUN_TEST(data, "INT_MIN", "|%025.20d|", INT_MIN);
	
	RUN_TEST(data, "INT_MIN", "|%030.20d|", INT_MIN);
	RUN_TEST(data, "INT_MIN", "|%30.20d|", INT_MIN);
	RUN_TEST(data, "INT_MIN", "|%30d|", INT_MIN);
	RUN_TEST(data, "INT_MIN", "|%030d|", INT_MIN);
	RUN_TEST(data, "INT_MIN", "|%030.2d|", INT_MIN);
	RUN_TEST(data, "INT_MIN", "|%030.10d|", INT_MIN);
	RUN_TEST(data, "INT_MIN", "|%030.11d|", INT_MIN);
	RUN_TEST(data, "INT_MIN", "|%-30.20d|", INT_MIN);
	
	RUN_TEST(data, "INT_MIN", "|%020.25d|", INT_MIN);
	
	
	/* ---- Some more odd but valid combos ---- */
	
	RUN_TEST(data, "42", "|%0.5d|", 42);
	RUN_TEST(data, "42", "|%-0.5d|", 42);
	RUN_TEST(data, "42", "|%+#8d|", 42);
	RUN_TEST(data, "42", "|%#d|", 42);
	
	/* ---- Stress mixed: many conversions in long format ---- */
	
	RUN_TEST(data, "123, 0x1a, \"hello\", 'Q', 7u, (void *)&test_mixed", "|%+08d|%#10x|%10s|%-5c|%.4u|%p|", 123, 0x1a, "hello", 'Q', 7u, (void *)&test_mixed);
	RUN_TEST(data, "123, 0x1a, \"hello\", 'Q', 7u, (void *)&test_mixed", "|%+08d|%#10x|%10s|%-5c|%.4u|%+p|", 123, 0x1a, "hello", 'Q', 7u, (void *)&test_mixed);
	
#if TEST_INVALID

	/* ---- Edge: malformed but syntactically allowed repetitions ---- */
	
	/* --- malformed/unsupported specifiers:
	 * behaviour might be implementation defined.
	 * In many libc implementations, however, they handle them predictably,
	 * like below:
	 */
	RUN_TEST(data, "314", "|%+++---00010.4d|", 314);
	RUN_TEST(data, "0x1", "|%###08x|", 0x1);
	RUN_TEST(data, "5", "|%    10d|", 5);   /* spaces between percent and flags (libc ignores) */
	
	RUN_TEST(data, "77", "|%+-+-+-+-0000###   20.10d|", 77);
	RUN_TEST(data, "42", "|%+-#154..d|", 42);
	RUN_TEST(data, "42", "|%+-#154.23#I'm an invalid string|", 42);
	RUN_TEST(data, "42", "|%++ -00-#   ##+-#154.23#I'm an invalid string|", 42);
	RUN_TEST(data, "42", "|%++ 23 -00-#   ##+-#154.23#I'm an invalid string|", 42);

#endif
	
	/* ---- Final safety: very long precision on string (stress) ---- */
	RUN_TEST(data, "short", "|%.1000s|", "short");

#endif

	/* ---- Null printf ---- */
	RUN_TEST(data, "null fmt", NULL, NULL);

	print_result(data, total_ok, total_ko, "mixed");

	flush_sleep(&data);
}


static char* print_result(t_test_data *data, size_t total_ok, size_t total_ko,
	const char *spec)
{
	char buffer[32];

	if (data->test_ko == 0)
	{
		fmt_nbr(buffer, data->test_ok);
		printf(CLR_GREEN "\n\n========= " CLR_GOLD "%%%s" CLR_GREEN ": ALL TESTS PASSED: %s", spec, buffer);
		fmt_nbr(buffer, total_ok + data->test_ok);
		printf(" (total: %s) =========\n\n" CLR_RST, buffer);
	}
	else
	{
		fmt_nbr(buffer, data->test_ko);
		printf(CLR_RED "\n\n========= " CLR_GOLD "%%%s" CLR_RED ": TESTS FAILED (%s)", spec, buffer);
		fmt_nbr(buffer, total_ko + data->test_ko);
		printf(" (total: %s)" CLR_RST, buffer);
		fmt_nbr(buffer, data->test_ok);
		printf(" (passed: %s) " CLR_RED "=========\n\n" CLR_RST, buffer);
	}
	fflush(stdout);

	data->test_ok += total_ok;
	data->test_ko += total_ko;
}

static void fmt_nbr(char *buffer, size_t n)
{
	/* 20 digits is enough for a 64-bit size_t (~18.4 quintillion), 
	 * plus separators (max 6), plus null terminator. 
	 */
	char temp[32];

	/* Convert the size_t to a string in the temporary buffer
	 * using snprintf
	 */
	int len = sprintf(temp, "%zu", n);

	int sep_count = (len - 1) / GROUP_SIZE;
	int final_len = len + sep_count;

	/* Check if buffer is large enough */
	if (final_len >= 32)
		buffer[0] = '\0';

	buffer[final_len] = '\0';
	int temp_idx = len - 1;

	for (int i = final_len - 1; i >= 0; i--)
	{
		buffer[i] = temp[temp_idx--];
		if (temp_idx >= 0 && (len - temp_idx - 1) % GROUP_SIZE == 0)
		{
			i--;
			buffer[i] = SEPARATOR;
		}
	}
}
