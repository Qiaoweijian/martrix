#ifndef HASH_MAP_FWD_
#define HASH_MAP_FWD_

#include <type_traits>

namespace martrix
{
namespace hashmap
{
    template<typename K>
    struct hash;
    
    template<typename T, bool V = 
        std::__or_<std::is_fundamental<T>, std::is_pointer<T>>::value>
    union hash_key_type;
    
    template<typename K, typename V, typename H = hash<K>>
    class hash_map;

}
}

#endif