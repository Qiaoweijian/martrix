#ifndef BUCKET_
#define BUCKET_

#include <type_traits>
#include "hash_map_macro.h"
#include <boost/unordered_map.hpp>

namespace martrix
{
namespace hashmap
{
    
    template<typename K, typename V, typename H>
    struct list_bucket
    {
        typedef typename std::remove_const<K>::type key_type;
        typedef V mapped_type;

        template<typename T>
        typename std::enable_if<std::is_void<T>::value, void>::type
        emplace(const key_type& k, const mapped_type& v)
        {
            // TODO 左值右值的使用, 完美转发?
            key.key_impl = k.key_impl;
            value = v;
        }

        template<typename T>
        typename std::enable_if<!std::is_void<T>::value, bool>::type
        emplace(const key_type& k, const mapped_type& v)
        {
            // TODO 左值右值的使用, 完美转发?
            if (hash_cmp<sizeof(key_type)>(key.key_impl, k.key_impl))
            {
                return true;
            }
            return false;
        }
        
        bool replace(const key_type& k, const mapped_type& v)
        {
            // TODO 左值右值的使用, 完美转发?
            if (!hash_cmp<sizeof(key_type)>(key.key_impl, k.key_impl))
            {
                return false;
            }
            else
            {
                value = v;
            }
            return true;
        }

        key_type key;
        mapped_type value;
    };

    template<typename K, typename V, typename H>
    struct bucket
    {
        typedef K key_type;
        typedef V mapped_type;
        typedef typename K::real_type real_type;
        typedef bucket<K, V, H> self_type;
        typedef list_bucket<K, V, H> list_type;
        typedef boost::unordered_map<K, V> extra_type;

        void init()
        {
            control = 0;
            hash_code = 0;
            list = NULL;
            extra_map = NULL;
        }
        
        inline bool is_valid()
        {
            return control & 0x80;
        }
        
        inline uint8_t index()
        {
            return control & 0x0f;
        }

        template<typename T>
        typename std::enable_if<std::is_void<T>::value, void>::type
        emplace(const key_type& k, const mapped_type& v)
        {
            node.key.key_impl = k.key_impl;
            node.value = v;
            control |= 0x80;
        }

        template<typename T>
        typename std::enable_if<!std::is_void<T>::value, bool>::type
        emplace(const key_type& k, const mapped_type& v)
        {
            // TODO 左值右值的使用, 完美转发?
            if (hash_cmp<sizeof(key_type)>(node.key.key_impl, k.key_impl))
            {
                return true;
            }
            return false;
        }

        bool replace(const key_type& k, const mapped_type& v)
        {
            // TODO 左值右值的使用, 完美转发?
            if (!hash_cmp<sizeof(key_type)>(node.key.key_impl, k.key_impl))
            {
                return false;
            }
            else
            {
                node.value = v;
            }
            return true;
        }

        //Bit 7 标识该bucket是否存在有效元素. Bit 4..6保留位.
        //Bit 0..3 标识listBucket当前最高有效的元素位.        
        uint8_t control;
        uint32_t hash_code;
        struct pick_node
        {
            key_type key;   // 内部1字节对齐，不需要清0
            mapped_type value;
        }node;
        list_type* list;
        extra_type* extra_map;
    };
}
}

#endif
