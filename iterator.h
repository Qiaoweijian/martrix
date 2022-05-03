#ifndef ITERATOR_
#define ITERATOR_

#include "bucket.h"
#include "aligned_pair.h"

namespace martrix
{
namespace hashmap
{

    template<typename T>
    inline T ptr_add(T ptr, uint64_t offset)
    {
        return (T)((char*)ptr + offset);
    }
    
    template<typename T>
    class hash_map_iterator
    {
        typedef typename T::key_type key_type;
        typedef typename T::mapped_type mapped_type;
        typedef typename T::real_type real_type;
        typedef hash_map_iterator<T> self_type;
        typedef aligned_pair<real_type const, mapped_type> value_type;
        typedef typename T::extra_type extra_type;
        typedef typename extra_type::iterator extra_iterator_type;
        typedef value_type* node_point_type;
        typedef value_type& node_ref_type;

        #ifdef DEBUG
        public:
        #else
        private:
        #endif
        
        T* bucket;
        uint8_t control; //控制当前访问Bucket or List or Extra
        uint8_t cur;
        uint8_t bucket_bits;
        uint8_t list_bucket_bits;
        extra_iterator_type extra_iter;
        T* bucket_end;

        void copy_default(T* start);
        void copy(T* start, uint8_t control_i, uint8_t cur_i, extra_iterator_type& itr_i);
        void next();

        public:
        enum Control
        {
            kDefault = 0,
            kBucket,
            kList,
            kExtra,
        };
        hash_map_iterator()
        {
            copy_default(NULL);
            bucket_end = NULL;
        }

        hash_map_iterator(T* bucket, T* end, uint8_t bits, uint8_t list_bits)
        {
            copy_default(bucket);
            bucket_end = end;
            bucket_bits = bits;
            list_bucket_bits = list_bits;
        }

        hash_map_iterator(T* bucket, T* end, uint8_t bits, uint8_t list_bits, 
            uint8_t control_i, uint8_t cur_i, extra_iterator_type& itr_i)
        {
            copy(bucket, control_i, cur_i, itr_i);
            bucket_end = end;
            bucket_bits = bits;
            list_bucket_bits = list_bits;
        }

        node_ref_type operator*() const
        {
            if (control == Control::kBucket)
            {
                return *(node_point_type)&bucket->node.key;
            }
            else if (control == Control::kList)
            {
                return *(node_point_type)(&(ptr_add(bucket->list, cur << list_bucket_bits)->key));
            }
            else
            {
                return (node_ref_type)(*extra_iter);
            }
        }

        node_point_type operator->() const
        {
            if (control == Control::kBucket)
            {
                return (node_point_type)&bucket->node.key;
            }
            else if (control == Control::kList)
            {
                return (node_point_type)(&(ptr_add(bucket->list, cur << list_bucket_bits)->key));
            }
            else
            {
                return (node_point_type)&extra_iter->first;
            }
        }

        self_type& operator++()
        {
            next();
            return *this;
        }

        self_type operator++(int)
        {
            self_type tmp(*this);
            next();
            return tmp;
        }

        friend bool operator==(const self_type& l, const self_type& r)
        {
            return l.bucket == r.bucket
                && l.control == r.control
                && l.cur == r.cur
                && l.extra_iter == r.extra_iter;
        }

        friend bool operator!=(const self_type& l, const self_type& r)
        {
            return !(l == r);
        }
    };
   
    template<typename T>
    void hash_map_iterator<T>::copy_default(T* start)
    {
        bucket = start;
        control = Control::kDefault;
        if (start && start != bucket_end && start->is_valid())
        {
            control = Control::kBucket;
        }
        cur = 0;
        extra_iter = extra_iterator_type();
    }

    template<typename T>
    void hash_map_iterator<T>::copy(T* start, 
        uint8_t control_i, uint8_t cur_i, extra_iterator_type& itr_i)
    {
        bucket = start;
        control = control_i;
        cur = cur_i;
        extra_iter = itr_i;
    }

    template<typename T>
    void hash_map_iterator<T>::next()
    {
        if (control == Control::kBucket)
        {
            if (bucket->index())
            {
                control = Control::kList;
                return;
            }
        }
        else if (control == Control::kList)
        {
            if (++cur < bucket->index())
            {
                return;
            }
            
            if (bucket->extra_map)
            {
                control = Control::kExtra;
                extra_iter = bucket->extra_map->begin();
                return;
            }
        }
        else if (control == Control::kExtra)
        {
            if (++extra_iter != bucket->extra_map->end())
            {
                return;
            }
        }

        auto next_bucket = ptr_add(bucket, 1 << bucket_bits);
        while(next_bucket != bucket_end)
        {
            if (next_bucket->is_valid())
            {
                break;
            }
            next_bucket = ptr_add(next_bucket,  1 << bucket_bits);
        }
        copy_default(next_bucket);
        return;
    }
}
}

#endif
