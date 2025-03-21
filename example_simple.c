/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2018-2022 Jason A. Donenfeld <Jason@zx2c4.com>. All Rights Reserved.
 */

#define _GNU_SOURCE

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <errno.h>

enum { PASCALS_TRIANGLE_MAX_ROWS_FOR_UINT64 = 68 };

static inline size_t cord(size_t a, size_t b)
{
	return a * (a + 1) / 2 + b;
}

static uint64_t *create_pascals_triangle(size_t num_rows)
{
	uint64_t *triangle;
	size_t i, j, size;

	if (!num_rows || num_rows > PASCALS_TRIANGLE_MAX_ROWS_FOR_UINT64) {
		errno = EINVAL;
		return NULL;
	}

	size = cord(num_rows, 0);
	triangle = calloc(size, sizeof(*triangle));
	if (!triangle)
		return NULL;

	for (i = 0; i < num_rows; ++i) {
		triangle[cord(i, 0)] = triangle[cord(i, i)] = 1;
		for (j = 1; j < i; ++j)
			triangle[cord(i, j)] = triangle[cord(i - 1, j - 1)] + triangle[cord(i - 1, j)];
	}

	return triangle;
}

static void print_pascals_triangle(FILE *file, const uint64_t *triangle, size_t num_rows)
{
	size_t i, j;

	for (i = 0; i < num_rows; ++i) {
		for (j = 0; j < i + 1; ++j)
			fprintf(file, "%" PRIu64 " ", triangle[cord(i, j)]);
		fputc('\n', file);
	}
}

int main(int argc, char *argv[])
{
	uint64_t *triangle;
	size_t num_rows;
	char *endptr;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s ROWS\n", argv[0]);
		return 1;
	}
	
	num_rows = strtoul(argv[1], &endptr, 10);
	if (!*argv[1] || *endptr) {
		fprintf(stderr, "ERROR: `%s' is not a valid number\n", argv[1]);
		return 1;
	}

	triangle = create_pascals_triangle(num_rows);
	if (!triangle) {
		perror("ERROR: unable to create triangle");
		return 1;
	}
	print_pascals_triangle(stdout, triangle, num_rows);
	free(triangle);
	
	return 0;
}
