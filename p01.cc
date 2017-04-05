#include <isl/set.h>
#include <isl/options.h>
#include <pet.h>

#include <iostream>

#include "scopinfo.hh"

void parseScopFile(isl_ctx *ctx, const char *filename);

void logLine(std::string s)
{
    std::cout << s << std::endl;
}

int main(int argc, char *argv[])
{
    using std::cout;
    struct pet_options* pet_defaults = pet_options_new_with_defaults();
    isl_ctx* ctx = isl_ctx_alloc_with_options(&pet_options_args, pet_defaults);  
    //isl_ctx* ctx = isl_ctx_alloc();
    pet_options_set_autodetect(ctx, 0);//TODO: find out why
    isl_options_set_ast_always_print_block(ctx, 1);
    const char *filename = "file.c";
    parseScopFile(ctx, filename);
    isl_ctx_free(ctx);//TODO: free/clear scop
    return 0;
}

void parseScopFile(isl_ctx *ctx, const char *filename){
    struct pet_scop* scop =
      pet_scop_extract_from_C_source(ctx, filename, NULL);

    if (NULL == scop)
    {
        std::string message = "No SCoP in file: ";
        message.append(filename);
        logLine(message);
        return;
    }
    std::string message = "SCoP found in file: ";
    message.append(filename);
    std::string count_message = "\n Statement count: " + std::to_string(scop->n_stmt);
    message.append(count_message);
    logLine(message);
    logLine("Statements bodies: ");
    //pet_scop_dump(scop);
    for (int i=0 ; i<scop->n_stmt ; ++i)
    {
        //pet_tree_dump(scop->stmts[i]);
        ;
    }

    ScopInfo scop_info(scop);
    std::cout << scop_info.toString() << std::endl;
    scop_info.normalize();//TODO: refactor
}
