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

//    struct isl_union_set *d = isl_union_set_read_from_str(ctx, "[M, N] -> { U[i, j] : 0 < i < N and 0 < j < M }");
	FILE * f = fopen("file.domain", "rt");
	struct isl_union_set *d = isl_union_set_read_from_file(ctx, f);
	printf("%s\n", isl_union_set_to_str(d));
	fclose(f);
//    struct isl_union_map *r = isl_union_map_read_from_str(ctx, "[N, M] -> { U[i, j] -> U[i' = i, j' = 1 + j] : 0 < i < N and 0 < j <= -2 + M }");
	FILE *file_rel = fopen("file.rel", "rt");
	struct isl_union_map *r = isl_union_map_read_from_file(ctx, file_rel);
	printf("%s\n", isl_union_map_to_str(r));    
	fclose(file_rel);
	struct isl_schedule * s=isl_union_set_compute_schedule (d, r, r);
//	printf("%p\n", s);
	printf("%s\n", isl_schedule_to_str(s));
	printf("%s\n", "begin1");
/*	isl_union_map *simple_map = isl_schedule_get_map(s);
	printf("%s\n", "begin2");
	printf("%s\n", isl_union_map_to_str(simple_map));
	printf("%s\n", "begin3");
*/
	//isl_schedule_free(s);
//    isl_union_set_free(d);
    isl_union_map_free(r);
    isl_ctx_free(ctx);
    return 0;
}
