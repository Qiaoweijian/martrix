#include"hash_map_fwd.h"
#include"cstdint"

using namespace martrix::hashmap;
namespace testptr
{
    typedef union hash_key_type<int*> UserKeyType;
    typedef hash_map<UserKeyType, int> map_type;

    bool func(map_type& a);
}
