#ifndef HASH_MAP_
#define HASH_MAP_

#include "table.h"
#include "hash_map_fwd.h"

#define HASH_KEY_ALIGN 8
#define ROUNDUP(x, y) ((x + (y - 1)) & ~(y - 1))
#define HASH_KEY_IMPL(n) struct hashmap::key_type<ROUNDUP((n), (HASH_KEY_ALIGN))>

namespace martrix
{
namespace hashmap
{

using std::int8_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using boost::hash_value;

    template<typename T>
    union hash_key_type<T, true>
    {
        //TODO 避免赋值0两次
        hash_key_type():key_impl(){}
        hash_key_type(T arg):key_impl(){key = arg;}

        typedef T real_type;
        T key;
        HASH_KEY_IMPL(sizeof(real_type)) key_impl;
    };

    template<typename T>
    union hash_key_type<T, false>
    {
        hash_key_type()
        {
            auto last = ptr_add((uint64_t*)(this + 1), -sizeof(uint64_t));
            *last = 0;
        }
        
        template<typename... Args>
        hash_key_type(Args... args):key(args...)
        {
            constexpr auto mask = sizeof(T) & (8 - 1) ?
                ((uint64_t)1 << ((sizeof(T) & (8 - 1)) * 8)) - 1 
                : ~(uint64_t)0;
            auto last = ptr_add((uint64_t*)(this + 1), -sizeof(uint64_t));
            *last = *last & mask;
        }

        // 对齐强制1，避免内部对齐导致的空洞
        static_assert(1 == alignof(T),  "Not support");
        typedef T real_type;
        T key;
        HASH_KEY_IMPL(sizeof(T)) key_impl;
    };

    template<typename T>
    size_t hash_value(const hash_key_type<T>& val)
    {
        return hash_value(val.key);
    }

    template<typename T>
    bool operator==(const hash_key_type<T>& l, const hash_key_type<T>& r)
    {
        return l.key == r.key;
    }

    template<typename K>
    struct hash
    {
        inline uint32_t operator()(const K& k, int right)
        {
            return hashmap::hash64_generic<sizeof(K)>(k.key_impl, right);
        }
    };
    
    template<typename K, typename V, typename H>
    class hash_map : public table<K, V, H>
    {
        public:
        typedef table<K, V, H> base_type;
        typedef typename base_type::mapped_type mapped_type;
        typedef typename base_type::bucket_type bucket_type;
    };
}
}

#endif

