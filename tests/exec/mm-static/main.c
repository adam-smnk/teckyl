#include <stdio.h>
#include <string.h>
#include "../lib/memref.h"

/* Generated matrix multiplication function under test */
extern void mm(DECL_VEC2D_FUNC_IN_ARGS(a, float),
	       DECL_VEC2D_FUNC_IN_ARGS(b, float),
	       DECL_VEC2D_FUNC_OUT_ARGS(o, float));

/* Reference implementation of a matrix multiplication */
void mm_refimpl(const struct vec_f2d* a, const struct vec_f2d* b, struct vec_f2d* o)
{
	float accu;

	for(int64_t y = 0; y < o->sizes[0]; y++) {
		for(int64_t x = 0; x < o->sizes[1]; x++) {
			accu = 0;

			for(int64_t k = 0; k < a->sizes[1]; k++)
				accu += vec_f2d_get(a, k, y) * vec_f2d_get(b, x, k);

			vec_f2d_set(o, x, y, accu);
		}
	}
}

/* Initialize matrix with value x+y at position (x, y) */
void init_matrix(struct vec_f2d* m)
{
	for(int64_t y = 0; y < m->sizes[0]; y++)
		for(int64_t x = 0; x < m->sizes[1]; x++)
			vec_f2d_set(m, x, y, x+y);
}

void die_usage(const char* program_name)
{
	fprintf(stderr, "Usage: %s [-v]\n", program_name);
	exit(1);
}

int main(int argc, char** argv)
{
	struct vec_f2d a, b, o, o_ref;
	int verbose = 0;
	int n = 512;
	int k = 1024;
	int m = 256;

	if(argc > 2)
		die_usage(argv[0]);

	if(argc == 2) {
		if(strcmp(argv[1], "-v") == 0)
			verbose = 1;
		else
			die_usage(argv[0]);
	}

	if(vec_f2d_alloc(&a, n, k) ||
	   vec_f2d_alloc(&b, k, m) ||
	   vec_f2d_alloc(&o, n, m) ||
	   vec_f2d_alloc(&o_ref, n, m))
	{
		fprintf(stderr, "Allocation failed");
		return 1;
	}

	init_matrix(&a);
	init_matrix(&b);

	if(verbose) {
		puts("B:");
		vec_f2d_dump(&b);
		puts("");

		puts("O:");
		vec_f2d_dump(&o);
		puts("");
	}

	mm(VEC2D_ARGS(&a), VEC2D_ARGS(&b), VEC2D_ARGS(&o));
	mm_refimpl(&a, &b, &o_ref);

	if(verbose) {
		puts("Result O:");
		vec_f2d_dump(&o);
		puts("");

		puts("Reference O:");
		vec_f2d_dump(&o_ref);
		puts("");
	}

	if(!vec_f2d_compare(&o, &o_ref)) {
	        fputs("Result differs from reference result\n", stderr);
		exit(1);
	}

	vec_f2d_destroy(&a);
	vec_f2d_destroy(&b);
	vec_f2d_destroy(&o);
	vec_f2d_destroy(&o_ref);

	return 0;
}
