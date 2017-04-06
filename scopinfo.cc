#include "scopinfo.hh"
#include "normalization.hh"
#include "ast.hh"
#include "utility.hh"
#include "options.hh"
#include "lex_scheduling.hh"
#include "debug.hh"
#include "tiling.hh"
#include <sstream>
#include <cstring>
#include <iostream>
#include <isl/flow.h>
#include <isl/set.h>
#include <isl/ctx.h>
#include <isl/id.h>
#include <isl/set.h>
#include <isl/union_map.h>
#include <isl/union_set.h>

__isl_give isl_id_list* id_sequence(__isl_keep isl_ctx* ctx, const char* gen_id, int size)
{
    std::stringstream s;
//    char buff[50];
    isl_id_list* result = isl_id_list_alloc(ctx, size);
    for (int i = 0; i < size; ++i)
    {
        s << gen_id << i;
//        sprintf(buff, "%s%d", id, i);
        result = isl_id_list_add(result, isl_id_alloc(ctx, s.str().c_str(), NULL));
    }
    return result;
}

__isl_give isl_printer* codegen_print_prologue(__isl_take isl_printer* printer)
{
    printer = isl_printer_print_str(printer, "/* ");
    printer = isl_printer_print_str(printer, " Prologue test");
    printer = isl_printer_print_str(printer, "               */\n");
    return printer;
}

__isl_give isl_printer* codegen_print_epilogue(__isl_take isl_printer* printer)
{
    //printer = isl_printer_print_str(printer, "/**/\n");
    return printer;
}
void ScopInfo::print_code_tc()
{
    struct tc_scop* scop = (struct tc_scop*)malloc(sizeof(struct tc_scop));
    scop->domain = domain;
    scop->schedule = schedule;
    scop->relation = relation;
    scop->reads = reads;

    scop->writes = writes;
    scop->ctx = ctx;
    scop->pet = pet;

    isl_ctx* ctx = scop->ctx;

    isl_union_set* LD = isl_union_set_copy(scop->domain);
    tc_debug_uset(LD, "LD");

    isl_union_map* S = isl_union_map_copy(scop->schedule);
    tc_debug_umap(S, "S");

    isl_union_map* R = isl_union_map_copy(scop->relation);
    tc_debug_umap(R, "R");

    tc_debug_umap(scop->reads, "RA");
    tc_debug_umap(scop->writes, "WA");

    isl_basic_set* sample = isl_set_sample(tc_normalize_union_set(LD, S));

    isl_space* space = isl_basic_set_get_space(sample);

    isl_id_list* I = tc_ids_sequence(ctx, "i", isl_space_dim(space, isl_dim_set));
    isl_id_list* II = tc_ids_sequence(ctx, "ii", isl_space_dim(space, isl_dim_set));

    std::map<std::string, std::vector<int> > blocks = tc_options_blocks(options);

    std::vector<std::vector<std::string> > groups = tc_options_groups(options);

    isl_set* tile;
    isl_set* ii_set;

    tc_tile_loop_nest(LD, S, II, I, &tile, &ii_set, blocks, groups);

    tc_debug_set(tile, "TILE");
    tc_debug_set(ii_set, "II_SET");

    isl_map* R_normalized = tc_normalize_union_map(R, S);
    tc_debug_map(R_normalized, "R_norm");

//    if (tc_options_is_report(options))
//    {
//        isl_set* bounds = tc_options_get_report_bounds(options, ctx);
//
//        struct tc_tile_statistics* stats = tc_compute_tile_statistics(tile, ii_set, II, bounds, LD, S, scop->reads, scop->writes, scop, options, blocks);
//
//        tc_tile_statistics_print(options->output, stats);
//
//        tc_tile_statistics_free(stats);
//        isl_set_free(bounds);
//    }

    isl_map* Rtile = tc_Rtile_map(II, tile, R_normalized);

    tc_debug_map(Rtile, "R_TILE");

    enum tc_scheduling_enum scheduling = tc_options_scheduling(options);

    tc_scheduling_lex(scop, options, LD, S, R, ii_set, tile, Rtile, II, I);
    isl_map_free(R_normalized);
    isl_basic_set_free(sample);
    isl_space_free(space);
}
void ScopInfo::print_code()
{
    bool useTcPrint = true;
    if (useTcPrint)
    {
        print_code_tc();
        return;
    }
    isl_basic_set* sample = isl_set_sample(normalize_union_set(domain, schedule));

    isl_space* space = isl_basic_set_get_space(sample);

    isl_id_list* idList1 = id_sequence(ctx, "i", isl_space_dim(space, isl_dim_set));
    isl_id_list* idList2 = id_sequence(ctx, "ii", isl_space_dim(space, isl_dim_set));


    isl_id_list* iterators = isl_id_list_concat(isl_id_list_copy(idList2), isl_id_list_copy(idList1));


    isl_ctx* ctx = isl_union_map_get_ctx(schedule);
    isl_ast_build* ast_build = isl_ast_build_from_context(isl_set_copy(pet->context));
    ast_build = isl_ast_build_set_iterators(ast_build, isl_id_list_copy(iterators));
    ast_build = isl_ast_build_set_at_each_domain(ast_build, &tc_ast_visitor_at_each_domain, pet);

    isl_printer* printer = isl_printer_to_str(ctx);
    printer = isl_printer_set_output_format(printer, ISL_FORMAT_C);
    isl_ast_print_options* ast_options = isl_ast_print_options_alloc(ctx);
    isl_ast_node* ast_tile = isl_ast_build_ast_from_schedule(ast_build, schedule);
    printer = codegen_print_prologue(printer);
    printer = isl_ast_node_print_macros(ast_tile, printer);
    printer = isl_printer_print_str(printer, "#pragma scop\n");
    //printer = isl_ast_node_print(ast_tile, printer, ast_options);
    printer = isl_printer_print_str(printer, "#pragma endscop\n");
    printer = codegen_print_epilogue(printer);
    char* code = isl_printer_get_str(printer);
    //fprintf(outputFile, "%s", code);

    std::cout << code << std::endl;
//    free(code);
//
//    isl_id_list_free(iterators);
//    isl_id_list_free(idList1);
//    isl_id_list_free(idList2);
//
//    isl_printer_free(printer);
//    isl_ast_node_free(ast_tile);
//    isl_ast_build_free(ast_build);
}


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

ScopInfo::ScopInfo(pet_scop* scop, struct tc_options* pOptions)
{
    options = pOptions;
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
    isl_union_map_free(relation);
    isl_union_set_free(domain);
    pet_scop_free(pet);
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
    isl_map_free(R_normalized);
}