#include"hash_map_fwd.h"
#include"cstdint"

using namespace martrix::hashmap;
namespace testint
{
    typedef union hash_key_type<uint32_t> UserKeyType;
    typedef hash_map<UserKeyType, int> map_type;

    bool func(map_type& a);
}
