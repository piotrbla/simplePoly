#ifndef NORMALIZATION_H
#define NORMALIZATION_H

#include <isl/ctx.h>
#include <isl/set.h>
#include <isl/map.h>
#include <isl/union_set.h>

__isl_give isl_set *normalize_union_set(__isl_keep isl_union_set *uset, __isl_keep isl_union_map *S);
__isl_give isl_map *normalize_union_map(__isl_keep isl_union_map *umap, __isl_keep isl_union_map *S);
__isl_give isl_set *normalize_set(__isl_keep isl_set *set, __isl_keep isl_union_map *S);
__isl_give isl_map *normalize_map(__isl_keep isl_map *map, __isl_keep isl_union_map *S);
__isl_give isl_union_set *denormalize_set(__isl_keep isl_set *set, __isl_keep isl_union_map *S);
__isl_give isl_union_map *denormalize_map(__isl_keep isl_map *map, __isl_keep isl_union_map *S);

#endif