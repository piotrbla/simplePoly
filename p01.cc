#include <isl/set.h>
#include <isl/ctx.h>
#include <isl/options.h>
#include <pet.h>

#include <iostream>
#include <string>

void logLine(std::string s)
{
    std::cout << s << std::endl;
}

int main(int argc, char *argv[])
{
    struct pet_options* pet_defaults = pet_options_new_with_defaults();
    isl_ctx* ctx = isl_ctx_alloc_with_options(&pet_options_args, pet_defaults);  
    //isl_ctx* ctx = isl_ctx_alloc();
    pet_options_set_autodetect(ctx, 0);//TODO: find out why
    logLine("Before isl");
    isl_options_set_ast_always_print_block(ctx, 1);
    logLine("Before scop");
    const char *filename = "file.c";
    struct pet_scop* scop = 
      pet_scop_extract_from_C_source(ctx, filename, NULL);
    logLine("After scop");
    if (NULL == scop)
    {
        std::string message = "No SCoP in file: ";
        message.append(filename);
        logLine(message);
    }
    else
    {
        std::string message = "SCoP found in file: ";
        message.append(filename);
        logLine(message);
    }
    return 0;
}
