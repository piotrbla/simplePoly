#include "scopinfo.hh"
#include "normalization.hh"
#include <sstream>
#include <cstring>
#include <isl/flow.h>
#include <iostream>


static isl_stat collect_maps_callback(__isl_take isl_map* map, void* user)
{
    isl_map_list** maps = (isl_map_list**)user;
    *maps = isl_map_list_add(*maps, map);
    return isl_stat_ok;
}

__isl_give isl_map_list* collect_maps(__isl_keep isl_union_map* umap)
{
    isl_map_list* maps = isl_map_list_alloc(isl_union_map_get_ctx(umap), isl_union_map_n_map(umap));
    isl_union_map_foreach_map(umap, &collect_maps_callback, &maps);
    return maps;
}
__isl_give isl_union_map* remove_map_with_tuple(__isl_take isl_union_map* umap, const char* name)
{
    isl_union_map* result = NULL;
    isl_map_list* maps = collect_maps(umap);
    isl_union_map_free(umap);
    for (int i = 0; i < isl_map_list_n_map(maps); ++i)
    {
        isl_map* map = isl_map_list_get_map(maps, i);
        if ((isl_map_has_tuple_name(map, isl_dim_in) && 0 == strcmp(name, isl_map_get_tuple_name(map, isl_dim_in)))
            || (isl_map_has_tuple_name(map, isl_dim_out) && 0 == strcmp(name, isl_map_get_tuple_name(map, isl_dim_out))))
        {
            isl_map_free(map);
        }
        else
        {
            if (NULL == result)
            {
                result = isl_union_map_from_map(map);
            }
            else
            {
                result = isl_union_map_add_map(result, map);
            }
        }
    }
    isl_map_list_free(maps);
    return result;
}

__isl_give isl_union_map* simplify_schedule(__isl_take isl_union_map* S)
{
    isl_map_list* maps = collect_maps(S);
    isl_union_map_free(S);
    isl_map* map_sample = isl_map_list_get_map(maps, 0);
    int n_out = isl_map_n_out(map_sample);

    for (int i = n_out - 1; i >= 0; --i)
    {
        isl_val* val = isl_map_plain_get_val_if_fixed(map_sample, isl_dim_out, i);

        if (!isl_val_is_nan(val))
        {
            isl_bool all_equal = isl_bool_true;

            for (int j = 0; j < isl_map_list_n_map(maps); ++j)
            {
                isl_map* map = isl_map_list_get_map(maps, j);

                isl_val* val_other = isl_map_plain_get_val_if_fixed(map, isl_dim_out, i);

                if (!isl_val_eq(val, val_other))
                {
                    all_equal = isl_bool_false;
                }

                isl_val_free(val_other);
                isl_map_free(map);
            }

            if (all_equal)
            {
                for (int j = 0; j < isl_map_list_n_map(maps); ++j)
                {
                    isl_map* map = isl_map_list_get_map(maps, j);

                    map = isl_map_remove_dims(map, isl_dim_out, i, 1);

                    maps = isl_map_list_set_map(maps, j, map);
                }
            }
        }

        isl_val_free(val);
    }
    isl_map_free(map_sample);
    isl_union_map* S_prim = NULL;

    for (int i = 0; i < isl_map_list_n_map(maps); ++i)
    {
        isl_map* map = isl_map_list_get_map(maps, i);

        if (NULL == S_prim)
        {
            S_prim = isl_union_map_from_map(map);
        }
        else
        {
            S_prim = isl_union_map_add_map(S_prim, map);
        }
    }
    isl_map_list_free(maps);
    return S_prim;
}

ScopInfo::ScopInfo(pet_scop* scop)
{
    domain = isl_union_set_empty(isl_set_get_space(scop->context));
    pet = scop;
    computeRelationUnion();
    ctx = isl_union_set_get_ctx(domain);
    for (int i = 0; i < pet->n_stmt; i++)
    {
        struct pet_stmt* statement = pet->stmts[i];
        if (!pet_stmt_is_kill(statement))
        {
            isl_set* statement_domain = isl_set_copy(statement->domain);
            if (statement->n_arg > 0)
                statement_domain = isl_map_domain(isl_set_unwrap(statement_domain));
            domain = isl_union_set_add_set(domain, statement_domain);
        }
        else
        {
            const char* statement_label = isl_set_get_tuple_name(statement->domain);
            schedule = remove_map_with_tuple(schedule, statement_label);
            relation = remove_map_with_tuple(relation, statement_label);
        }
    }
    schedule = simplify_schedule(schedule);//TODO: refactor to method
}
ScopInfo::~ScopInfo()
{
    isl_union_map_free(reads);
    isl_union_map_free(writes);
    isl_union_map_free(schedule);

}
std::string ScopInfo::toString()
{
    std::stringstream s;
    s << "SCoPInfo: " << std::endl;

    s << "Domain: " << isl_union_set_to_str(domain) << std::endl;
    s << "Schedule: " << isl_union_map_to_str(schedule) << std::endl;
    s << "Relation: " << isl_union_map_to_str(relation) << std::endl;
    s << "Reads:  " << isl_union_map_to_str(reads) << std::endl;
    s << "Writes: " << isl_union_map_to_str(writes) << std::endl;
    return s.str();
}

__isl_give isl_union_map* unwrap_range(__isl_take isl_union_map* umap)
{
    isl_map_list* maps = collect_maps(umap);
    isl_union_map* unwrapped_umap = isl_union_map_empty(isl_union_map_get_space(umap));
    for (int i = 0; i < isl_map_list_n_map(maps); ++i)
    {
        isl_map* map = isl_map_list_get_map(maps, i);
        if (isl_map_range_is_wrapping(map))
            map = isl_set_unwrap(isl_map_domain(isl_map_uncurry(map)));
        unwrapped_umap = isl_union_map_add_map(unwrapped_umap, map);
    }
    isl_map_list_free(maps);
    isl_union_map_free(umap);
    return unwrapped_umap;
}

void computeFlow(
        __isl_keep isl_union_map* r1, __isl_keep isl_union_map* r2,
        __isl_keep isl_union_map* r3, __isl_keep isl_union_map* r4,
        __isl_keep isl_union_map **computed_rel)
{
    isl_union_map_compute_flow(isl_union_map_copy(r1),
                               isl_union_map_copy(r2),
                               isl_union_map_copy(r3),
                               isl_union_map_copy(r4),
                               NULL, computed_rel, NULL, NULL);

}
void ScopInfo::computeRelationUnion() {
    isl_space *space = isl_set_get_space(pet->context);

    isl_union_map *empty = isl_union_map_empty(space);

    schedule = isl_schedule_get_map(pet->schedule);
    reads = pet_scop_get_may_reads(pet);
    writes = pet_scop_get_may_writes(pet);

    isl_union_map *dep_read_after_write;
    isl_union_map *dep_write_after_read;
    isl_union_map *dep_write_after_write;

    computeFlow(reads, empty, writes, schedule, &dep_read_after_write);
    computeFlow(writes, empty, reads, schedule, &dep_write_after_read);
    computeFlow(writes, empty, writes, schedule, &dep_write_after_write);

    dep_read_after_write = isl_union_map_coalesce(dep_read_after_write);
    dep_write_after_read = isl_union_map_coalesce(dep_write_after_read);
    dep_write_after_write = isl_union_map_coalesce(dep_write_after_write);

    relation = dep_read_after_write;
    relation = isl_union_map_union(relation, dep_write_after_read);
    relation = isl_union_map_union(relation, dep_write_after_write);

    relation = isl_union_map_coalesce(relation);

    isl_union_map_free(empty);
    relation = unwrap_range(relation);
}

void ScopInfo::normalize()
{
    isl_map * R_normalized = normalize_union_map(relation, schedule);
    std::cout << "Normalized: " << isl_map_to_str(R_normalized) << std::endl;
}