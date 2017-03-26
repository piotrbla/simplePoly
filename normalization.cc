#include <isl/ctx.h>
#include <isl/set.h>
#include <isl/map.h>
#include <isl/union_set.h>

__isl_give isl_set *normalize_union_set(__isl_keep isl_union_set *uset, __isl_keep isl_union_map *S)
{
    if(isl_union_set_is_empty(uset))
    {
        isl_set *range_set = isl_set_from_union_set(isl_union_map_range(isl_union_map_copy(S)));
        isl_set *uset_normalized = isl_set_empty(isl_set_get_space(range_set));
        isl_set_free(range_set);
        return uset_normalized;
    }
    else
    {
        isl_union_set *uset_normalized = isl_union_set_apply(isl_union_set_copy(uset), isl_union_map_copy(S));
        return isl_set_from_union_set(uset_normalized);
    }
}

__isl_give isl_map *normalize_union_map(__isl_keep isl_union_map *umap, __isl_keep isl_union_map *S)
{
    if(isl_union_map_is_empty(umap))
    {
        isl_set *range_set = isl_set_from_union_set(isl_union_map_range(isl_union_map_copy(S)));
        isl_map *range_map = isl_map_from_domain_and_range(isl_set_copy(range_set), isl_set_copy(range_set));
        isl_set_free(range_set);
        isl_map *umap_normalized = isl_map_empty(isl_map_get_space(range_map));
        isl_map_free(range_map);
        return umap_normalized;
    }
    else
    {
        isl_union_map *umap_normalized = isl_union_map_apply_domain(isl_union_map_copy(umap), isl_union_map_copy(S));
        umap_normalized = isl_union_map_apply_range(umap_normalized, isl_union_map_copy(S));
        return isl_map_from_union_map(umap_normalized);
    }
}

__isl_give isl_set *normalize_set(__isl_keep isl_set *set, __isl_keep isl_union_map *S)
{
    isl_union_set *uset = isl_union_set_from_set(isl_set_copy(set));
    isl_set *set_normalized = normalize_union_set(uset, S);
    isl_union_set_free(uset);
    return set_normalized;
}

__isl_give isl_map *normalize_map(__isl_keep isl_map *map, __isl_keep isl_union_map *S)
{
    isl_union_map *umap = isl_union_map_from_map(isl_map_copy(map));
    isl_map *map_normalized = normalize_union_map(umap, S);
    isl_union_map_free(umap);
    return map_normalized;
}

__isl_give isl_union_set *denormalize_set(__isl_keep isl_set *set, __isl_keep isl_union_map *S)
{
    isl_union_set *uset_denormalized = isl_union_set_apply(isl_union_set_from_set(isl_set_copy(set)),
                                                           isl_union_map_reverse(isl_union_map_copy(S)));

    return uset_denormalized;
}

__isl_give isl_union_map *denormalize_map(__isl_keep isl_map *map, __isl_keep isl_union_map *S)
{
    isl_union_map *umap_denormalized = isl_union_map_apply_domain(isl_union_map_from_map(isl_map_copy(map)),
                                                                  isl_union_map_reverse(isl_union_map_copy(S)));
    umap_denormalized = isl_union_map_apply_range(umap_denormalized, isl_union_map_reverse(isl_union_map_copy(S)));
    return umap_denormalized;
}




