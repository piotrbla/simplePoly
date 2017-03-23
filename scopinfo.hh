#ifndef SCOPINFO_H
#define SCOPINFO_H

#include <isl/ctx.h>
#include <isl/set.h>
#include <isl/union_set.h>
#include <isl/union_map.h>
#include <pet.h>

#include <string>

class ScopInfo
{
public:
    ScopInfo(pet_scop* scop);
    std::string toString();
private:
    isl_ctx* ctx;
    isl_union_set* domain;
    isl_union_map* schedule;
    isl_union_map* relation;
    isl_union_map* reads;
    isl_union_map* writes;
    pet_scop* pet;
};

#endif
