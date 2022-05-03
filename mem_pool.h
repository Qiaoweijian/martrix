#ifndef MEM_POOL_
#define MEM_POOL_

#include <array>
#include <cstdint>
#include <malloc.h>
#include <unistd.h>

namespace martrix
{
namespace general
{
using std::uint32_t;

//smp x86_64
#define barrier() asm volatile("" ::: "memory")
#define smp_rmb() barrier()
#define smp_wmb() barrier()

template<typename T>
class MemoryPool
{
    public:
    inline bool push(const T& element);
    inline bool pop(T& element);
    bool init(uint32_t pool_size);

    #ifdef DEBUG
    public:
    #else
    private:
    #endif
    #pragma pack(4)
    typedef struct pool
    {
        uint32_t in;
        uint32_t out;
        uint32_t pool_size;
        T ele_start[0];
    }mem_pool_type;
    #pragma pack()
    
    mem_pool_type* mem_pool_ = NULL;
};


template<typename T>
bool MemoryPool<T>::init(uint32_t pool_size)
{
    static_assert(sizeof(mem_pool_type) == 
        sizeof(mem_pool_->in) + sizeof(mem_pool_->out) + sizeof(mem_pool_->pool_size),
        "The alignment of mem_pool_type is greater than 4");
    
    mem_pool_ = (mem_pool_type*)memalign(getpagesize(), 
        sizeof(mem_pool_type) + pool_size * sizeof(T));
    if (!mem_pool_)
    {
        return false;
    }
    mem_pool_->in = mem_pool_->out = 0;
    mem_pool_->pool_size = pool_size;
    return true;
}

template<typename T>
bool MemoryPool<T>::push(const T& element)
{
    auto len = mem_pool_->pool_size - (mem_pool_->in - mem_pool_->out);
    if (len)
   	{
		auto index = mem_pool_->in & (mem_pool_->pool_size - 1);
		mem_pool_->ele_start[index] = element;
		smp_wmb();
		++mem_pool_->in;
		return true;
	}
	return false;
}

template<typename T>
bool MemoryPool<T>::pop(T& element)
{
	auto len = mem_pool_->in - mem_pool_->out;
	if (len)
	{
		auto index = mem_pool_->out & (mem_pool_->pool_size - 1);
		element = mem_pool_->ele_start[index];
		smp_wmb();
		++mem_pool_->out;
		return true;
	}
	return false;
}

}
}

#endif
