#ifndef ALIGNED_PAIR_
#define ALIGNED_PAIR_

#include <utility>
#include <type_traits>

namespace martrix
{
namespace hashmap
{

using std::enable_if;
using std::__and_;
using std::is_convertible;
using std::is_nothrow_move_assignable;
using std::__decay_and_strip;
using std::forward;

    struct alignas(sizeof(uint64_t)) 
    zone_padding 
    {
    	char t[0];
    };
    #define ZONE_PADDING(name)	struct zone_padding name

	template<class T1, class T2>
	struct alignas(sizeof(uint64_t)) aligned_pair 
	{
		typedef T1 first_type;
		typedef T2 second_type;

		T1 first;
        ZONE_PADDING(_pad1);
		T2 second;

		constexpr aligned_pair():first(), second(){}

		constexpr aligned_pair(const T1& a, const T2& b)
			:first(a), second(b){}

		constexpr aligned_pair(const aligned_pair&) = default;
		constexpr aligned_pair(aligned_pair&&) = default;

		aligned_pair& operator=(const aligned_pair& p)
		{
			first = p.first;
			second = p.second;
			return *this;
		}

		aligned_pair& operator=(aligned_pair&& p)
		noexcept(__and_<is_nothrow_move_assignable<T1>,
				is_nothrow_move_assignable<T2>>::value)
		{
			first = std::forward<first_type>(p.first);
			second = std::forward<second_type>(p.second);
			return *this;
		}
	};


	template<class T1, class T2>
	inline constexpr bool
	operator==(const aligned_pair<T1, T2>& x, const aligned_pair<T1, T2>& y)
	{
	    return x.first == y.first && x.second == y.second;
    }

	template<class T1, class T2>
	inline constexpr bool
	operator<(const aligned_pair<T1, T2>& x, const aligned_pair<T1, T2>& y)
	{ 
	    return x.first < y.first
		    || (!(y.first < x.first) && x.second < y.second); 
    }

	template<class T1, class T2>
	inline constexpr bool
	operator!=(const aligned_pair<T1, T2>& x, const aligned_pair<T1, T2>& y)
	{
	    return !(x == y);
    }

	template<class T1, class T2>
	inline constexpr bool
	operator>(const aligned_pair<T1, T2>& x, const aligned_pair<T1, T2>& y)
	{
		return y < x;
    }

	template<class T1, class T2>
	inline constexpr bool
	operator<=(const aligned_pair<T1, T2>& x, const aligned_pair<T1, T2>& y)
	{
	    return !(y < x);
    }

	template<class T1, class T2>
	inline constexpr bool
	operator>=(const aligned_pair<T1, T2>& x, const aligned_pair<T1, T2>& y)
	{
	    return !(x < y);
    }
        
	template<class T1, class T2>
	constexpr aligned_pair<typename __decay_and_strip<T1>::__type,
    typename __decay_and_strip<T2>::__type>
    make_pair(T1&& x, T2&& y)
    {
      typedef typename __decay_and_strip<T1>::__type __ds_type1;
      typedef typename __decay_and_strip<T2>::__type __ds_type2;
      typedef aligned_pair<__ds_type1, __ds_type2> __aligned_pair_type;
      return __aligned_pair_type(std::forward<T1>(x), std::forward<T2>(y));
    }
}
}

#endif
