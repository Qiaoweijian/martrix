#ifndef TABLE_
#define TABLE_

#include <malloc.h>
#include <type_traits>
#include <cstdint>
#include "mem_pool.h"
#include "iterator.h"

namespace martrix
{
namespace hashmap
{

using std::uint32_t;
using std::uint64_t;
using std::uint16_t;
using std::uint8_t;
using std::int8_t;

    inline uint32_t roundup_pow_of_two(uint32_t v)
    {
        --v;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        ++v;
        return v;
    }

    enum HASH_MAP_ATTR
    {
        kDefault = 0,
    };
    
    template<typename K, typename V, typename H>
    class table
    {
        public:
        typedef K key_type;
        typedef V mapped_type;
        typedef table<K, V, H> self_type;
        typedef bucket<K, V, H> bucket_type;
        typedef list_bucket<K, V, H> list_bucket_type;
        typedef typename bucket_type::extra_type extra_type;
        typedef typename extra_type::iterator extra_iterator_type;
        typedef hash_map_iterator<bucket_type> iterator;

        public:
        /**
         * @brief      初始化hashmap
         * 
         * @param[in]  num_buckets 初始化bucket的数量
         * @param[in]  attr 属性
         * @param[in]  num_lists list_bucket的数量
         *
         * @return     成功返回true, 失败返回false
         */
        bool init(uint32_t num_buckets, 
            HASH_MAP_ATTR attr = HASH_MAP_ATTR::kDefault, int8_t num_lists = 8);

        /**
         * @brief      插入元素
         * 
         * @param[in]  key_i 
         * @param[in]  val_i 
         *
         * @return     成功返回true, 失败返回false
         */
        bool insert(const key_type& key_i, const mapped_type& val_i);

        /**
         * @brief      插入或者赋值
         * 
         * @param[in]  key_i 
         * @param[in]  val_i 
         *
         * @return     成功返回true, 失败返回false
         */
        bool insert_or_assign(const key_type& key_i, const mapped_type& val_i);

        /**
         * @brief      查找元素
         * 
         * @param[in]  key_i 
         *
         * @return     成功返回指向当前元素的迭代器, 失败返回end()
         */
        iterator find(const K& key_i);
        
        iterator begin();

        iterator end();

        /**
         * @brief      
         * 
         * @return     返回hashmap中桶的数量
         */
        uint32_t bucket_count()
        {
            return header_.bucket_count;
        }

        /**
         * @brief      
         * 
         * @return     返回指定桶中元素的数量
         */
        uint32_t bucket_size(uint32_t n)
        {
            bucket_type* bucket = ptr_add(header_.bucket_start, n << header_.bucket_bits);
            return !bucket->is_valid() ? 0 :
                   !bucket->index() ? 1 :
                   !bucket->extra_map ? 1 + bucket->index():
                   1 + bucket->index() + bucket->extra_map->size();
        }

        public:
        struct tableHeader
        {
            //bucket info
            bucket_type* bucket_start = NULL;
            uint32_t bucket_count = 0;
            uint8_t bucket_count_right = 0;
            uint8_t bucket_bits = 0;
            uint8_t list_bucket_size = 0;
            uint8_t list_bucket_bits = 0;
            int8_t list_count = 0;
            uint8_t list_count_bits = 0;
            uint8_t attr = 0;
        };

        tableHeader header_;
        H hash_;
        general::MemoryPool<list_bucket_type*> list_bucket_pool_;

    };

    template<typename K, typename V, typename H>
    bool table<K, V, H>::init(
        uint32_t num_buckets, HASH_MAP_ATTR attr, int8_t num_lists)
    {
        uint32_t bucket_count = num_buckets == 1 ? 2 : roundup_pow_of_two(num_buckets);
        uint16_t bucket_size = roundup_pow_of_two(sizeof(bucket_type));
        uint16_t list_bucket_size = roundup_pow_of_two(sizeof(list_bucket_type));
        uint32_t bucket_mem_size = bucket_count * bucket_size;
        auto bucket_ptr = (bucket_type*)memalign(getpagesize(), bucket_mem_size);
        if (!bucket_ptr)
        {
            return false;
        }

        auto list_bucket_num = bucket_count >> 3 > 0 ? bucket_count >> 3 : 1;
        int8_t list_count = roundup_pow_of_two(num_lists);
        list_count = list_count > 15 ? 15 : list_count; // bit 0..3
        uint16_t list_count_bits = __builtin_ctz((uint32_t)list_count);
        
        header_.bucket_start = bucket_ptr;
        header_.bucket_count = bucket_count;
        header_.bucket_count_right = __builtin_clzll((uint64_t)bucket_count) + 1;
        header_.bucket_bits = __builtin_ctz((uint32_t)bucket_size);
        header_.list_bucket_size = list_bucket_size;
        header_.list_bucket_bits = __builtin_ctz((uint32_t)list_bucket_size);
        header_.list_count = list_count;
        header_.list_count_bits = list_count_bits;
        header_.attr = attr;

        if (!list_bucket_pool_.init(list_bucket_num))
        {
            free(bucket_ptr);
            return false;
        }

        for (int i = 0; i < bucket_count; ++i)
        {
            bucket_ptr->init();
            bucket_ptr = ptr_add(bucket_ptr, bucket_size);
        }
        
        int32_t section_size = list_bucket_size << header_.list_bucket_bits;
        int32_t list_bucket_mem_size = section_size * list_bucket_num;
        auto list_bucket_mem = (list_bucket_type*)malloc(list_bucket_mem_size);
        memset(list_bucket_mem, 0, list_bucket_mem_size);
        
        for (int i = 0; i < list_bucket_num; ++i)
        {
            list_bucket_pool_.push(list_bucket_mem);
            list_bucket_mem = ptr_add(list_bucket_mem, section_size);
        }
        return true;
    }

    template<typename K, typename V, typename H>
    bool table<K, V, H>::insert(
        const key_type& key_i, const mapped_type& val_i)
    {
        auto pos = hash_(key_i, header_.bucket_count_right);
        auto bucket = ptr_add(header_.bucket_start, pos << header_.bucket_bits);
        uint8_t list_count = header_.list_count;

        if (bucket->is_valid())
        {
            if (bucket->template emplace<bool>(key_i, val_i))
            {
                return false;
            }

            if (!bucket->list)
            {
                list_bucket_type* list_bucket = NULL;
                if (!list_bucket_pool_.pop(list_bucket))
                {
                    auto size = header_.list_bucket_size << header_.list_count_bits;
                    list_bucket = (list_bucket_type*)malloc(size);
                    if (list_bucket)
                    {
                        memset(list_bucket, 0, size);
                    }
                }

                if (!list_bucket)
                {
                    return false;
                }
                ++bucket->control;
                bucket->list = list_bucket;
                list_bucket->template emplace<void>(key_i, val_i);
                return true;
            }
            else
            {
                int i;
                for (i = 0; i < bucket->index(); ++i)
                {
                    auto ptr = ptr_add(bucket->list, i << header_.list_bucket_bits);
                    if (ptr->template emplace<bool>(key_i, val_i))
                    {
                        return false;
                    }
                }

                if (i < list_count)
                {
                    ++bucket->control;
                    auto ptr = ptr_add(bucket->list, i << header_.list_bucket_bits);
                    ptr->template emplace<void>(key_i, val_i);
                    return true;
                }

                if (!bucket->extra_map)
                {
                    bucket->extra_map = new extra_type();
                }
                if (!bucket->extra_map)
                {
                    return false;
                }
                
                auto ret = bucket->extra_map->emplace(key_i, val_i);
                return ret.second;
            }
        }
        else
        {
            bucket->template emplace<void>(key_i, val_i);
            return true;
        }
    }

    template<typename K, typename V, typename H>
    bool table<K, V, H>::insert_or_assign(
        const key_type& key_i, const mapped_type& val_i)
    {
        auto pos = hash_(key_i, header_.bucket_count_right);
        auto bucket = ptr_add(header_.bucket_start, pos << header_.bucket_bits);
        uint8_t list_count = header_.list_count;

        if (bucket->is_valid())
        {
            if (bucket->replace(key_i, val_i))
            {
                return true;
            }

            if (!bucket->list)
            {
                list_bucket_type* list_bucket = NULL;
                if (!list_bucket_pool_.pop(list_bucket))
                {
                    auto size = header_.list_bucket_size << header_.list_count_bits;
                    list_bucket = (list_bucket_type*)malloc(size);
                    if (list_bucket)
                    {
                        memset(list_bucket, 0, size);
                    }
                }

                if (!list_bucket)
                {
                    return false;
                }
                ++bucket->control;
                bucket->list = list_bucket;
                list_bucket->template emplace<void>(key_i, val_i);
                return true;
            }
            else
            {
                int i;
                for (i = 0; i < bucket->index(); ++i)
                {
                    auto ptr = ptr_add(bucket->list, i << header_.list_bucket_bits);
                    if (ptr->replace(key_i, val_i))
                    {
                        return true;
                    }
                }

                if (i < list_count)
                {
                    ++bucket->control;
                    auto ptr = ptr_add(bucket->list, i << header_.list_bucket_bits);
                    ptr->template emplace<void>(key_i, val_i);
                    return true;
                }

                if (!bucket->extra_map)
                {
                    bucket->extra_map = new extra_type();
                }
                if (!bucket->extra_map)
                {
                    return false;
                }
                
                (*bucket->extra_map)[key_i] = val_i;
                return true;
            }
        }
        else
        {
            bucket->template emplace<void>(key_i, val_i);
            return true;
        }
    }

    template<typename K, typename V, typename H>
    typename table<K, V, H>::iterator
    table<K, V, H>::find(const K& key_i)
    {
        typename iterator::Control control;
        uint8_t cur;
        extra_iterator_type extra_iter;
        
        const auto pos = hash_(key_i, header_.bucket_count_right);
        bucket_type* bucket = ptr_add(header_.bucket_start, pos << header_.bucket_bits);
        
        if (bucket->is_valid())
        {
            if (hash_cmp<sizeof(K)>(bucket->node.key.key_impl, key_i.key_impl))
            {
                control = iterator::Control::kBucket;
                cur = 0;
                extra_iter = extra_iterator_type();
                goto ret;
            }

            if (bucket->list)
            {
                for (int i = 0; i < bucket->index(); ++i)
                {
                    auto ptr = ptr_add(bucket->list, i << header_.list_bucket_bits);
                    if (hash_cmp<sizeof(K)>(ptr->key.key_impl, key_i.key_impl))
                    {
                        control = iterator::Control::kList;
                        cur = i;
                        extra_iter = extra_iterator_type();
                        goto ret;
                    }
                }

                if (NULL == bucket->extra_map)
                {
                    return end();
                }
                auto itr = bucket->extra_map->find(key_i);
                if (itr == bucket->extra_map->end())
                {
                    return end();
                }
                control = iterator::Control::kExtra;
                cur = bucket->index();
                extra_iter = itr;
            }
        }
        else
        {
            return end();
        }
        
    ret:
        auto bucket_end = ptr_add(header_.bucket_start, 
            header_.bucket_count << header_.bucket_bits);
        return iterator(bucket, bucket_end, 
                    header_.bucket_bits, header_.list_bucket_bits, 
                    control, cur, extra_iter);
    }

    template<typename K, typename V, typename H>
    typename table<K, V, H>::iterator 
    table<K, V, H>::begin()
    {
        bucket_type* bucket = NULL;
        auto bucket_end = ptr_add(header_.bucket_start, 
            header_.bucket_count << header_.bucket_bits);
        for (int i = 0; i < header_.bucket_count; ++i)
        {
            bucket = ptr_add(header_.bucket_start, i << header_.bucket_bits);
            if (bucket && bucket->is_valid())
            {
                return iterator(bucket, bucket_end, 
                    header_.bucket_bits, header_.list_bucket_bits);
            }
        }
        return end();
    }

    template<typename K, typename V, typename H>
    typename table<K, V, H>::iterator 
    table<K, V, H>::end()
    {
        auto bucket_end = ptr_add(header_.bucket_start, 
            header_.bucket_count << header_.bucket_bits);
        return iterator(bucket_end, bucket_end, 
            header_.bucket_bits, header_.list_bucket_bits);
    }
}
}

#endif
