#include "scopinfo.hh"
#include <string>
#include <sstream>
ScopInfo::ScopInfo(pet_scop* scop)
{
    isl_ctx* ctx;
    isl_union_set* domain;
    isl_union_map* schedule;
    isl_union_map* relation;
    reads  = pet_scop_get_may_reads(scop);
    writes = pet_scop_get_may_writes(scop);
    pet_scop* pet;
}

std::string ScopInfo::toString()
{
    std::stringstream s;
    s << "ScopInfo: " << std::endl;
    s << "Reads:" << isl_union_map_to_str(reads) << std::endl;
    s << "Writes:" << isl_union_map_to_str(writes) << std::endl;
    return s.str();
}
