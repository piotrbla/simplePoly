#include <isl/set.h>
#include <isl/union_set.h>
#include <isl/union_map.h>
#include <isl/options.h>
#include <pet.h>

#include <iostream>

void logLine(std::string s)
{
    std::cout << s << std::endl;
}

int main(int argc, char *argv[])
{
    using std::cout;
    isl_ctx* ctx = isl_ctx_alloc();

    struct isl_union_set *d = isl_union_set_read_from_str(ctx, "[M, N] -> { U[i, j] : 0 < i < N and 0 < j < M }");
	printf("%s\n", isl_union_set_to_str(d));
    struct isl_union_map *r = isl_union_map_read_from_str(ctx, "[N, M] -> { U[i, j] -> U[i' = i, j' = 1 + j] : 0 < i < N and 0 < j <= -2 + M }");
	printf("%s\n", isl_union_map_to_str(r));    
	struct isl_schedule * s=isl_union_set_compute_schedule (d, r, r);
//	printf("%p\n", s);
	printf("%s\n", isl_schedule_to_str(s));
	
//	isl_schedule_free(s);
    isl_union_set_free(d);
    isl_union_map_free(r);
    isl_ctx_free(ctx);
    return 0;
}
