/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_tester.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guillsan <guillsan@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 13:12:33 by guillsan          #+#    #+#             */
/*   Updated: 2025/11/12 14:02:24 by guillsan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf_tester.h"

/* Macro for running tests with variadic arguments:
 * TEST("desc","fmt", args...)
 */
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
		printf(CLR_GREEN "[PASS] | fmt = %s  (%s)\n" CLR_RST, (fmt), (desc));  \
		printf("  ft_printf  : %s\n", out_ft);                                 \
		printf("  libc printf: %s\n", out_libc);                               \
		printf("  ft (%d) | lb (%d)\n", r_ft, r_libc);                         \
		fflush(stdout);                                                        \
		(data)->test_ok++;                                                     \
	}                                                                          \
	else                                                                       \
	{                                                                          \
		printf(CLR_RED "[FAIL] | fmt = %s  (%s)\n" CLR_RST, (fmt), (desc));    \
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

int test_ft_printf(void)
{
	t_test_data data;
	
	data.test_ok = 0;
	data.test_ko = 0;
	
	printf("\n========= ft_printf automated tests =========\n");

	/* --- SIMPLE conversions --- */
	printf(CLR_YELLOW "\n========= SIMPLE CONVERSIONS =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "char basic", "|%c|", 'A');
	RUN_TEST(&data, "string basic", "|%s|", "hello");
	RUN_TEST(&data, "string empty", "|%s|", "");
	RUN_TEST(&data, "string NULL", "|%s|", (char*)NULL);
	RUN_TEST(&data, "percent literal", "%%%% %%%% %s", "end");

	RUN_TEST(&data, "pointer stack var", "|%p|", (void*)&test_ft_printf);
	
	
	/* pointer NULL */
	printf(CLR_YELLOW "\n========= POINTER NULL =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "pointer NULL", "|%p|", (void*)0);

	RUN_TEST(&data, "int basic", "|%d|", 42);
	RUN_TEST(&data, "int negative", "|%d|", -42);
	RUN_TEST(&data, "int zero", "|%d|", 0);
	RUN_TEST(&data, "int INT_MAX", "|%d|", INT_MAX);
	RUN_TEST(&data, "int INT_MIN", "|%d|", INT_MIN);

	RUN_TEST(&data, "i specifier (same as d)", "|%i|", -12345);

	RUN_TEST(&data, "unsigned basic", "|%u|", 42u);
	RUN_TEST(&data, "unsigned 0", "|%u|", 0u);
	RUN_TEST(&data, "unsigned UINT_MAX", "|%u|", UINT_MAX);

	RUN_TEST(&data, "hex lower basic", "|%x|", 305441741u);
	RUN_TEST(&data, "hex upper basic", "|%X|", 305441741u);
	RUN_TEST(&data, "oct/hex zero", "|%x|", 0u);


	/* --- WIDTH/TEST_FLAGS/PRECISION for integers --- */
	printf(CLR_YELLOW "\n====== WIDTH/TEST_FLAGS/PRECISION int ======\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "width 5 int", "|%5d|", 42);
	
#if TEST_FLAGS

	RUN_TEST(&data, "width 5 left-justify", "|%-5d|", 42);
	RUN_TEST(&data, "zero-pad width 5", "|%05d|", 42);
	RUN_TEST(&data, "plus flag", "|%+d|", 42);
	RUN_TEST(&data, "space flag", "|% d|", 42);
	RUN_TEST(&data, "plus and zero pad", "|%+05d|", 42);
	RUN_TEST(&data, "left-justify and zero (conflict)", "|%-05d|", 42); /* behaviour: '-' wins */
	RUN_TEST(&data, "precision .5", "|%.5d|", 42);
	RUN_TEST(&data, "width and precision", "|%8.4d|", 123);
	RUN_TEST(&data, "width and precision", "|%8.-4d|", 123);
	RUN_TEST(&data, "precision zero and value zero", "|%.0d|", 0);
	
	/* negatives with flags */
	printf(CLR_YELLOW "\n========= NEGATIVES (TEST_FLAGS) =========\n\n" CLR_RST);
	
	RUN_TEST(&data, "negative with zero-pad", "|%05d|", -42);
	RUN_TEST(&data, "negative with precision", "|%.5d|", -42);
	RUN_TEST(&data, "negative with plus", "|%+d|", -42);


	/* --- unsigned/hex with flags --- */
	printf(CLR_YELLOW "\n========= UINT/HEX (TEST_FLAGS) =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "unsigned width", "|%8u|", 1234u);
	RUN_TEST(&data, "hex with # (alt form)", "|%#x|", 0x1234u);
	RUN_TEST(&data, "hex with # and zero-pad", "|%#08x|", 0x1234u);
	RUN_TEST(&data, "hex uppercase #", "|%#X|", 0xABu);
	
#if TEST_OCTAL

	RUN_TEST(&data, "octal with #", "|%#o|", 0123u);
	RUN_TEST(&data, "octal # with zero", "|%#o|", 0u);
	
#endif


	/* precision for unsigned */
	printf(CLR_YELLOW "\n========= PRECISION UINT =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "unsigned precision", "|%.6u|", 123u);
	RUN_TEST(&data, "hex precision", "|%.6x|", 0x1au);

	/* --- strings with width/precision/flags --- */
	printf(CLR_YELLOW "\n======== S (WIDTH/PREC/TEST_FLAGS) ========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "string width", "|%10s|", "abc");
	RUN_TEST(&data, "string left-justify", "|%-10s|", "abc");
	RUN_TEST(&data, "string precision", "|%.3s|", "abcdef");
	RUN_TEST(&data, "string width+precision", "|%8.3s|", "abcdef");
	RUN_TEST(&data, "string precision zero", "|%.0s|", "hello");

#endif

	/* --- chars including NUL char (return counts matter) --- */
	printf(CLR_YELLOW "\n========= Cs (NULL) =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "char NUL inside output (printed as raw byte)", "|%c|", '\0'); /* output will contain NUL: harness reads raw bytes */

		
	/* --- multiple conversions in one format --- */
	printf(CLR_YELLOW "\n========= MULTIPLE CONVERSIONS =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "mixed conversions", "A:%d B:%u C:%x S:%s P:%p E:%%", -7, 42u, 0xfaceu, "str", (void*)&printf);

#if TEST_FLAGS
	
	/* --- width & flags combos for %s (0 flag ignored) --- */
	printf(CLR_YELLOW "\n==== WIDTH/TEST_FLAGS (%%s, '0' ignored) ====\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "string zero flag ignored", "|%010s|", "abc");
	RUN_TEST(&data, "string plus flag ignored", "|%+10s|", "abc"); /* + ignored for strings, libc might ignore */

	/* --- Boundary/edge values --- */
	printf(CLR_YELLOW "\n======== BOUNDARY EDGE VALUES ========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "INT_MIN with width and 0", "|%020d|", INT_MIN);
	RUN_TEST(&data, "UINT_MAX hex upper", "|%X|", UINT_MAX);
	RUN_TEST(&data, "large width 100", "|%100d|", 42);
	
	/* --- Some tricky combinations with #-+ space and precision --- */
	printf(CLR_YELLOW "\n======== TRICKY COMBINATIONS ========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "# with zero value hex", "|%#x|", 0u);
	RUN_TEST(&data, "# with nonzero hex", "|%#x|", 0x1u);
	RUN_TEST(&data, "+ with width and precision", "|%+8.5d|", 123);
	RUN_TEST(&data, "space with width", "|% 8d|", 123);

	/* --- multiple flags order permutations --- */
	printf(CLR_YELLOW "\n======== MULTIPLE TEST_FLAGS ORDER ========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "flags order -0", "|% -05d|", 42);
	RUN_TEST(&data, "flags combination '#0+' for hex", "|%#0+8x|", 0x1abu);

	/* --- extremely large width/precision to detect crashes (use moderately large to avoid huge allocations) --- */
	printf(CLR_YELLOW "\n========= EXTREME PRECISION =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "width 500 (stress)", "|%500s|", "x");
	RUN_TEST(&data, "precision 200 (stress)", "|%.200s|", "abcdefghijklmnopqrstuvwxyz");
	RUN_TEST(&data, "precision 40 (stress)", "|%.40d|", 845);

#endif

	/* ---- Edge cases: values & simple flags ---- */
	printf(CLR_YELLOW "\n========= EDGE CASES (SIMPLE TEST_FLAGS) =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "int INT_MAX", "|%d|", INT_MAX);
	RUN_TEST(&data, "int INT_MIN", "|%d|", INT_MIN);
	RUN_TEST(&data, "unsigned UINT_MAX", "|%u|", UINT_MAX);
	RUN_TEST(&data, "zero int", "|%d|", 0);
	RUN_TEST(&data, "zero unsigned", "|%u|", 0u);
	RUN_TEST(&data, "negative small", "|%d|", -1);
	RUN_TEST(&data, "negative large", "|%i|", -2147483647);
	RUN_TEST(&data, "char normal", "|%c|", 'A');
	RUN_TEST(&data, "char NUL", "|%c|", '\0');
	RUN_TEST(&data, "string NULL", "|%s|", (char*)NULL);
	RUN_TEST(&data, "string empty", "|%s|", "");
	RUN_TEST(&data, "pointer NULL", "|%p|", (void*)0);

	printf(CLR_YELLOW "\n========= EDGE CASES (NULL/NIL & TEST_FLAGS) =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "ptr NULL padding 0", "|%p|", NULL);
	
#if TEST_FLAGS
	
	RUN_TEST(&data, "ptr NULL padding 0", "|%2p|", NULL);
	RUN_TEST(&data, "ptr NULL padding 0", "|%1p|", NULL);
	RUN_TEST(&data, "ptr NULL padding 0", "|%0p|", NULL);
	
	RUN_TEST(&data, "ptr NULL", "|%.9p|", NULL);
	RUN_TEST(&data, "ptr NULL padding 0", "|%020p|", NULL);
	RUN_TEST(&data, "ptr NULL padding 0 + precision", "|%020.9p|", NULL);
	RUN_TEST(&data, "ptr NULL padding 0 + precision + '+'", "|%+020.9p|", NULL);
	RUN_TEST(&data, "string NULL", "|%.9s|", (char*)NULL);

#endif

	RUN_TEST(&data, "string NULL padding 0", "|%s|", (char*)NULL);

#if TEST_FLAGS

	RUN_TEST(&data, "string NULL padding 0", "|%2s|", (char*)NULL);
	RUN_TEST(&data, "string NULL padding 0", "|%1s|", (char*)NULL);
	RUN_TEST(&data, "string NULL padding 0", "|%0s|", (char*)NULL);
	
	RUN_TEST(&data, "string NULL padding 0", "|%020s|", (char*)NULL);
	RUN_TEST(&data, "string NULL padding ' '", "|%20s|", (char*)NULL);
	RUN_TEST(&data, "string NULL padding '-'", "|%-20s|", (char*)NULL);
	RUN_TEST(&data, "string NULL padding 0 + precision", "|%020.9s|", (char*)NULL);
	RUN_TEST(&data, "string NULL padding ' ' + precision", "|%20.9s|", (char*)NULL);
	RUN_TEST(&data, "string NULL padding '-' + precision", "|%-20.9s|", (char*)NULL);
	
	/* ---- Width & precision edge combos (integers) ---- */
	printf(CLR_YELLOW "\n========= WIDTH & PREC (INT) =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "width 0", "|%0d|", 42);
	RUN_TEST(&data, "width 1", "|%1d|", 42);
	RUN_TEST(&data, "width 2", "|%2d|", 42);
	RUN_TEST(&data, "width 5", "|%5d|", 42);
	RUN_TEST(&data, "width 10", "|%10d|", 42);
	RUN_TEST(&data, "width large 100", "|%100d|", 42);
	RUN_TEST(&data, "precision .0 with zero", "|%.0d|", 0);
	RUN_TEST(&data, "precision .0 nonzero", "|%.0d|", 5);
	RUN_TEST(&data, "precision .1", "|%.1d|", 123);
	RUN_TEST(&data, "precision .5", "|%.5d|", 123);
	RUN_TEST(&data, "width+precision small", "|%6.3d|", 7);
	RUN_TEST(&data, "width smaller than prec", "|%3.5d|", 7);
	
	/* ---- Zero / padding / left-justify conflicts ---- */
	printf(CLR_YELLOW "\n========= '0'/PAD/'-' CONFLICTS =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "zero pad simple", "|%05d|", 42);
	RUN_TEST(&data, "left justify simple", "|%-5d|", 42);
	RUN_TEST(&data, "zero and left conflict", "|%-05d|", 42);
	RUN_TEST(&data, "zero and plus", "|%+05d|", 42);
	RUN_TEST(&data, "zero and space", "|% 05d|", 42);
	RUN_TEST(&data, "many zeros", "|%0000005d|", 42);        /* repeated zero */
	RUN_TEST(&data, "many minus", "|%-----5d|", 42);        /* repeated minus */
	RUN_TEST(&data, "minus then zero", "|% -0-0 10d|", 42); /* weird spacing & repeats */
	
	/* ---- Plus / space flags and interactions ---- */
	printf(CLR_YELLOW "\n========= '+'/' ' FLAG INTERACTIONS =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "plus flag pos", "|%+d|", 42);
	RUN_TEST(&data, "plus flag neg", "|%+d|", -42);
	RUN_TEST(&data, "space flag pos", "|% d|", 42);
	RUN_TEST(&data, "plus and space together", "|%+ d|", 42);
	RUN_TEST(&data, "plus repeated", "|%++d|", 42);
	RUN_TEST(&data, "space repeated", "|%  d|", 42);
	
	/* ---- Hash (#) alternate form tests ---- */
	printf(CLR_YELLOW "\n========= '#' ALTERNATE FORM =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "hash hex nonzero lower", "|%#x|", 0x1a3);
	RUN_TEST(&data, "hash hex zero lower", "|%#x|", 0u);
	RUN_TEST(&data, "hash HEX nonzero upper", "|%#X|", 0x1a3);
	
#if TEST_OCTAL

	RUN_TEST(&data, "hash oct nonzero", "|%#o|", 0123u);
	RUN_TEST(&data, "hash oct zero", "|%#o|", 0u);
	
#endif

	RUN_TEST(&data, "hash with width and zero", "|%#08x|", 0x7f);
	RUN_TEST(&data, "hash with precision", "|%#.5x|", 0x7f);

	
	/* ---- Repeated/strange flags combos ---- */
	printf(CLR_YELLOW "\n========= REPEATED TEST_FLAGS COMBOS =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "repeated flags -0-0", "|%--00-8d|", 123);
	RUN_TEST(&data, "repeated flags +++000", "|%+++0008d|", 123);
	RUN_TEST(&data, "all flags combined", "|%#0- +10.5x|", 0x2b);
	RUN_TEST(&data, "all flags repeated", "|%##--++00 00#10.3d|", 7);
	
	/* ---- Width / precision extremes (stress) ---- */
	printf(CLR_YELLOW "\n========= WIDTH & PREC EXTREMES =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "width 200", "|%200d|", 1);
	RUN_TEST(&data, "precision 200", "|%.200d|", 1);
	RUN_TEST(&data, "width 200 + precision 200", "|%200.200d|", 1);
	RUN_TEST(&data, "huge width but small value", "|%1000d|", 1); /* may be slow but tests allocation/behavior */
	

	/* ---- unsigned / hex / octal edge cases ---- */
	printf(CLR_YELLOW "\n========= OCTAL EDGE CASES =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "unsigned zero with prec0", "|%.0u|", 0u);
	RUN_TEST(&data, "unsigned precision less", "|%.2u|", 123u);
	RUN_TEST(&data, "hex uppercase zero prec", "|%.0X|", 0u);
	RUN_TEST(&data, "hex lower width+hash", "|%#12x|", 0xabc);
	
#if TEST_OCTAL

	RUN_TEST(&data, "octal width+hash", "|%#8o|", 0777u);
	
#endif
	
	/* ---- Strings: precision, width, NULL behavior ---- */
	printf(CLR_YELLOW "\n========= PREC WIDTH NULL BEHAVIOR =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "string precision cut", "|%.3s|", "abcdef");
	RUN_TEST(&data, "string width and precision", "|%8.3s|", "abcdef");
	RUN_TEST(&data, "string left pad zeros (0 ignored)", "|%010s|", "abc");
	RUN_TEST(&data, "string plus ignored", "|%+10s|", "abc");
	RUN_TEST(&data, "string many flags", "|% -+0#10.5s|", "hello");
	RUN_TEST(&data, "string precision zero", "|%.0s|", "hello");
	
	/* ---- Char (%c) special cases ---- */
	printf(CLR_YELLOW "\n========= %%c SPECIAL CASES =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "char normal with width", "|%5c|", 'Z');
	RUN_TEST(&data, "char left justify", "|%-5c|", 'Z');
	RUN_TEST(&data, "char zero pad (ignored)", "|%05c|", 'Z');
	RUN_TEST(&data, "multiple char and nul", "|%c-%c-%c|", 'A', '\0', 'B');
	
	/* ---- Percent literal with flags (should print % or behave like libc) ---- */
	printf(CLR_YELLOW "\n========= %% LITERAL WITH TEST_FLAGS =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "literal percent simple", "|%%|");
	RUN_TEST(&data, "percent with width", "|%5%|");
	RUN_TEST(&data, "percent with left", "|%-5%|");
	RUN_TEST(&data, "percent with zero", "|%05%|");
	RUN_TEST(&data, "percent weird flags", "|%+0#-5%|");
	RUN_TEST(&data, "flags in weird order", "|%0#-+ 10.5x|", 0x7b);

#endif

	/* ---- Mixed sequences & argument order checks ---- */
	printf(CLR_YELLOW "\n========= MIXED ARGS ORDER =========\n\n" CLR_RST);
	fflush(stdout);

	RUN_TEST(&data, "mix int unsigned hex", "|%d %u %x|", -1, 4294967295u, 0xdead);
	RUN_TEST(&data, "mix many types", "|%s|%c|%d|%u|%x|%p|", "hola", '!', -7, 255u, 0x2a, (void*)&printf);
	
#if TEST_FLAGS

	/* ---- Precision and rounding-like integer edge behaviors ---- */
	printf(CLR_YELLOW "\n========= PREC & INT EDGE =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "precision smaller than digits", "|%.2d|", 12345);
	RUN_TEST(&data, "precision equals digits", "|%.3d|", 123);
	RUN_TEST(&data, "precision greater than digits", "|%.8d|", 123);
	
	/* ---- Boundary negatives combined with flags/width/prec ---- */
	printf(CLR_YELLOW "\n========= BOUND NEGATIVES (TEST_FLAGS/WIDTH/PREC) =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "INT_MIN with width", "|%20d|", INT_MIN);
	RUN_TEST(&data, "INT_MIN with zero pad", "|%020d|", INT_MIN);
	RUN_TEST(&data, "INT_MIN with precision", "|%.15d|", INT_MIN);
	RUN_TEST(&data, "INT_MIN with plus/space", "|%+ 20.10d|", INT_MIN);
	
	
	/* ---- Some more odd but valid combos ---- */
	printf(CLR_YELLOW "\n========= ODD VALID COMB =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "zero + precision", "|%0.5d|", 42);
	RUN_TEST(&data, "minus + precision", "|%-0.5d|", 42);
	RUN_TEST(&data, "plus + hash with decimal (hash ignored)", "|%+#8d|", 42);
	RUN_TEST(&data, "hash with decimal (ignored)", "|%#d|", 42);
	
	/* ---- Stress mixed: many conversions in long format ---- */
	printf(CLR_YELLOW "\n========= MANY CONV =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "long mixed stress", "|%+08d|%#10x|%10s|%-5c|%.4u|%p|", 123, 0x1a, "hello", 'Q', 7u, (void*)&test_ft_printf);
	RUN_TEST(&data, "long mixed stress", "|%+08d|%#10x|%10s|%-5c|%.4u|%+p|", 123, 0x1a, "hello", 'Q', 7u, (void*)&test_ft_printf);
	
#if TEST_FLAGS_INVALID

	/* ---- Edge: malformed but syntactically allowed repetitions ---- */
	printf(CLR_YELLOW "\n========= MALFORMED =========\n\n" CLR_RST);
	fflush(stdout);
	/* --- malformed/unsupported specifiers: should be tested carefully, 
	 * behaviour might be implementation defined.
	 * In many libc, however, they handle multiple of them predictably:
	 * '%%q' prints '%q' in many libc, but it's not portable.
	 * or clutter of them, like below.
	 */

	RUN_TEST(&data, "repeat +++---000", "|%+++---00010.4d|", 314);
	RUN_TEST(&data, "repeat # # #", "|%###08x|", 0x1);
	RUN_TEST(&data, "repeat spaces", "|%    10d|", 5);   /* spaces between percent and flags (libc ignores) */
	
	RUN_TEST(&data, "many flag clutter", "|%+-+-+-+-0000###   20.10d|", 77);
	RUN_TEST(&data, "many flag clutter", "|%+-#154..d|", 42);
	RUN_TEST(&data, "many flag clutter", "|%+-#154.23#I'm an invalid string|", 42);
	RUN_TEST(&data, "many flag clutter", "|%++ -00-#   ##+-#154.23#I'm an invalid string|", 42);
	RUN_TEST(&data, "many flag clutter", "|%++ 23 -00-#   ##+-#154.23#I'm an invalid string|", 42);

#endif
	
	/* ---- Final safety: very long precision on string (stress) ---- */
	printf(CLR_YELLOW "\n========= LONG PREC in %%s =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "string precision 1000", "|%.1000s|", "short");

#endif

	/* ---- Null printf ---- */
	printf(CLR_YELLOW "\n========= NULL FMT =========\n\n" CLR_RST);
	fflush(stdout);
	
	RUN_TEST(&data, "null fmt", NULL, NULL);
	

	printf("\n\n");
	if (data.test_ko == 0)
		printf(CLR_GREEN "\n========= ALL TESTS PASSED (%d) =========\n" CLR_RST, data.test_ok);
	else
		printf(CLR_RED "\n========= TESTS FAILED (%d) (passed: %d) =========\n" CLR_RST, data.test_ko, data.test_ok);
	fflush(stdout);

	return (0);
}

int main(void)
{
	test_ft_printf();
	return (0);
}
