#include<array>
#include<cstring>
#include<string>
#include"hash_map_fwd.h"
#include"cstdint"

using namespace martrix::hashmap;

typedef std::array<char, 6> sec_type;
typedef uint32_t mar_type;

#pragma pack(1)
struct sec_key_type
{
    sec_type sec_id;  
    mar_type market;
    sec_key_type(){}
    sec_key_type(const sec_type& sec_id, const mar_type& market)
    {
        this->sec_id = sec_id;
        this->market = market;
    }
    bool operator==(const sec_key_type& p)const
    {
        return (sec_id == p.sec_id && market == p.market);
    }

};
#pragma pack()
size_t hash_value(const sec_key_type& p);

template <size_t Size>
std::string ToString(const std::array<char, Size>& from)
{
    return std::string(&from[0], Size);
}

template <size_t N>
void ToArray(
    const std::string& from, std::array<char, N>& to)
{
    auto min = std::min(from.size(), N);
    memcpy(&to[0], from.c_str(), min);
    memset(&to[min], ' ',  N > min ? N - min : 0);
}

namespace testsec
{
    typedef union hash_key_type<sec_key_type> UserKeyType;
    typedef hash_map<UserKeyType, sec_type> map_type;

    bool func(map_type& a);
}


