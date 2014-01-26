/* vi: set sw=4 ts=4: */
/*
 * Mini sort implementation for busybox
 *
 *
 * Copyright (C) 2000 by Matt Kraai <kraai@alumni.carnegiemellon.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include "busybox.h"

int max_compare_len = 0;

static int compare_ascii(const void *x, const void *y)
{
	if (!max_compare_len)
		return strcmp(*(char **)x, *(char **)y);
	else
		return strncmp(*(char **)x, *(char **)y, max_compare_len);
}

static int compare_numeric(const void *x, const void *y)
{
	return atoi(*(char **)x) - atoi(*(char **)y);
}

int sort_main(int argc, char **argv)
{
	FILE *fp;
	char *line, **lines = NULL;
	int i, opt, nlines = 0;
	int (*compare)(const void *, const void *) = compare_ascii;
#ifdef BB_FEATURE_SORT_REVERSE
	int reverse = FALSE;
#endif

	while ((opt = getopt(argc, argv, "nrl")) != -1) {
		switch (opt) {
			case 'n':
				compare = compare_numeric;
				break;
#ifdef BB_FEATURE_SORT_REVERSE
			case 'r':
				reverse = TRUE;
				break;
#endif
			/*
			 * FIXME: This hack is used so that comparison will be limitid
			 * to LOG_COMPARE_LEN chars so as to only sort using time stamp
			 * of format:
			 * -->Mmm DD HH:MM:SS<--
			 * -->012345678901234<--
			 */
			#define LOG_COMPARE_LEN 15
			case 'l':
				max_compare_len = LOG_COMPARE_LEN;
				break;
			default:
				show_usage();
		}
	}

	/* read the input */
	for (i = optind; i == optind || i < argc; i++) {
		if (argv[i] == NULL)
			fp = stdin;
		else
			fp = xfopen(argv[i], "r");

		while ((line = get_line_from_file(fp)) != NULL) {
			lines = xrealloc(lines, sizeof(char *) * (nlines + 1));
			lines[nlines++] = line;
		}
	}

	/* sort it */
	qsort(lines, nlines, sizeof(char *), compare);

	/* print it */
#ifdef BB_FEATURE_SORT_REVERSE
	if (reverse)
		for (i = nlines - 1; 0 <= i; i--)
			fputs(lines[i], stdout);
	else
#endif
	for (i = 0; i < nlines; i++)
	{
		fputs(lines[i], stdout);
		free(lines[i]);
	}
	free(lines);
	fclose(fp);
	return EXIT_SUCCESS;
}
