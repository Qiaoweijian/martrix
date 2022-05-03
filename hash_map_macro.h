#ifndef HASH_MAP_IMPL_
#define HASH_MAP_IMPL_

#include <cstdint>

namespace martrix
{
namespace hashmap
{
using std::uint32_t;
using std::uint64_t;

template<int N>
struct key_type{};

template<int N>
inline bool hash_cmp(
    const struct key_type<N>&,
    const struct key_type<N>&)
{
    return false;
}

template<int N>
inline uint32_t hash64_generic(const struct key_type<N>&, int)
{
    return 0;
}

#define KEY_8 key1
#define KEY_16 key2
#define KEY_24 key3
#define KEY_32 key4
#define KEY_40 key5
#define KEY_48 key6
#define KEY_56 key7
#define KEY_64 key8

#define HASH_KEY_ELE(N) key_impl.KEY_##N

#define KEY_DEC_8 uint64_t key1
#define KEY_DEC_16 KEY_DEC_8; uint64_t key2
#define KEY_DEC_24 KEY_DEC_16; uint64_t key3
#define KEY_DEC_32 KEY_DEC_24; uint64_t key4
#define KEY_DEC_40 KEY_DEC_32; uint64_t key5
#define KEY_DEC_48 KEY_DEC_40; uint64_t key6
#define KEY_DEC_56 KEY_DEC_48; uint64_t key7
#define KEY_DEC_64 KEY_DEC_56; uint64_t key8

#define HASH_KEY_DEC(N) \
    template<> \
    struct key_type<N> \
    { \
        KEY_DEC_##N; \
    }

HASH_KEY_DEC(8);
HASH_KEY_DEC(16);
HASH_KEY_DEC(24);
HASH_KEY_DEC(32);
HASH_KEY_DEC(40);
HASH_KEY_DEC(48);
HASH_KEY_DEC(56);
HASH_KEY_DEC(64);


#define KEY_EQ_8(l, r) (l.key1 == r.key1)
#define KEY_EQ_16(l, r) KEY_EQ_8(l, r) && (l.key2 == r.key2)
#define KEY_EQ_24(l, r) KEY_EQ_16(l, r) && (l.key3 == r.key3)
#define KEY_EQ_32(l, r) KEY_EQ_24(l, r) && (l.key4 == r.key4)
#define KEY_EQ_40(l, r) KEY_EQ_32(l, r) && (l.key5 == r.key5)
#define KEY_EQ_48(l, r) KEY_EQ_40(l, r) && (l.key6 == r.key6)
#define KEY_EQ_56(l, r) KEY_EQ_48(l, r) && (l.key7 == r.key7)
#define KEY_EQ_64(l, r) KEY_EQ_56(l, r) && (l.key8 == r.key8)

#define HASH_KEY_CMP_FUNC(N) \
    template<> \
    inline bool hash_cmp<N>( \
    const struct key_type<N>& l, const struct key_type<N>& r) \
    { \
        return KEY_EQ_##N(l, r); \
    }

HASH_KEY_CMP_FUNC(8)
HASH_KEY_CMP_FUNC(16)
HASH_KEY_CMP_FUNC(24)
HASH_KEY_CMP_FUNC(32)
HASH_KEY_CMP_FUNC(40)
HASH_KEY_CMP_FUNC(48)
HASH_KEY_CMP_FUNC(56)
HASH_KEY_CMP_FUNC(64)

#define HASH_GOLDEN_RATIO_64 0x61C8864680B583EBull
#define HASH64_GENERIC(val, right) ((val) * HASH_GOLDEN_RATIO_64 >> right)

#define KEY_SUM_8(key) key.key1
#define KEY_SUM_16(key) KEY_SUM_8(key) + key.key2
#define KEY_SUM_24(key) KEY_SUM_16(key) + key.key3
#define KEY_SUM_32(key) KEY_SUM_24(key) + key.key4
#define KEY_SUM_40(key) KEY_SUM_32(key) + key.key5
#define KEY_SUM_48(key) KEY_SUM_40(key) + key.key6
#define KEY_SUM_56(key) KEY_SUM_48(key) + key.key7
#define KEY_SUM_64(key) KEY_SUM_56(key) + key.key8

#define HASH64_CAL(key, right, N) HASH64_GENERIC(KEY_SUM_##N(key), right)

#define HASH64_FUNC(N) \
    template<> \
    inline uint32_t hash64_generic<N>( \
    const struct key_type<N>& key, int right) \
    { \
        return HASH64_CAL(key, right, N); \
    }
    
HASH64_FUNC(8)
HASH64_FUNC(16)
HASH64_FUNC(24)
HASH64_FUNC(32)
HASH64_FUNC(40)
HASH64_FUNC(48)
HASH64_FUNC(56)
HASH64_FUNC(64)
}
}

#endif