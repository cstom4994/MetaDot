// Copyright(c) 2022-2023, KaoruXun All rights reserved.

#ifndef ME_CORE_H
#define ME_CORE_H

#include <assert.h>
#include <stdint.h>

#include "core/basic_types.h"
#include "core/macros.hpp"

#define METADOT_INT8_MAX 0x7F
#define METADOT_UINT8_MAX 0xFF
#define METADOT_INT16_MAX 0x7FFF
#define METADOT_UINT16_MAX 0xFFFF
#define METADOT_INT32_MAX 0x7FFFFFFF
#define METADOT_UINT32_MAX 0xFFFFFFFF
#define METADOT_INT64_MAX 0x7FFFFFFFFFFFFFFF
#define METADOT_UINT64_MAX 0xFFFFFFFFFFFFFFFF

#define METADOT_OK 0
#define METADOT_FAILED 1

typedef int R_bool;
#define R_false 0
#define R_true 1
#define R_null NULL

// Num types
typedef int8_t I8;
typedef uint8_t U8;
typedef int16_t I16;
typedef uint16_t U16;
typedef int32_t I32;
typedef uint32_t U32;
typedef int64_t I64;
typedef uint64_t U64;
typedef float F32;
typedef double F64;

typedef unsigned char Byte;

typedef struct Pixel {
    U8 b;
    U8 g;
    U8 r;
    U8 a;
} Pixel;

#pragma region endian_h

// MetaDot endian
// https://github.com/mikepb/endian.h

#if (defined(_WIN16) || defined(_WIN32) || defined(_WIN64)) && !defined(__WINDOWS__)

#define __WINDOWS__

#endif

#if defined(__linux__) || defined(__CYGWIN__)

#include <endian.h>

#elif defined(__APPLE__)

#include <libkern/OSByteOrder.h>

#define htobe16(x) OSSwapHostToBigInt16(x)
#define htole16(x) OSSwapHostToLittleInt16(x)
#define be16toh(x) OSSwapBigToHostInt16(x)
#define le16toh(x) OSSwapLittleToHostInt16(x)

#define htobe32(x) OSSwapHostToBigInt32(x)
#define htole32(x) OSSwapHostToLittleInt32(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)

#define htobe64(x) OSSwapHostToBigInt64(x)
#define htole64(x) OSSwapHostToLittleInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#define le64toh(x) OSSwapLittleToHostInt64(x)

#define __BYTE_ORDER BYTE_ORDER
#define __BIG_ENDIAN BIG_ENDIAN
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#define __PDP_ENDIAN PDP_ENDIAN

#elif defined(__OpenBSD__)

#include <sys/endian.h>

#elif defined(__NetBSD__) || defined(__FreeBSD__) || defined(__DragonFly__)

#include <sys/endian.h>

#define be16toh(x) betoh16(x)
#define le16toh(x) letoh16(x)

#define be32toh(x) betoh32(x)
#define le32toh(x) letoh32(x)

#define be64toh(x) betoh64(x)
#define le64toh(x) letoh64(x)

#elif defined(__WINDOWS__)

#include <winsock2.h>

#if BYTE_ORDER == LITTLE_ENDIAN

#define htobe16(x) htons(x)
#define htole16(x) (x)
#define be16toh(x) ntohs(x)
#define le16toh(x) (x)

#define htobe32(x) htonl(x)
#define htole32(x) (x)
#define be32toh(x) ntohl(x)
#define le32toh(x) (x)

#define htobe64(x) htonll(x)
#define htole64(x) (x)
#define be64toh(x) ntohll(x)
#define le64toh(x) (x)

#elif BYTE_ORDER == BIG_ENDIAN

/* that would be xbox 360 */
#define htobe16(x) (x)
#define htole16(x) __builtin_bswap16(x)
#define be16toh(x) (x)
#define le16toh(x) __builtin_bswap16(x)

#define htobe32(x) (x)
#define htole32(x) __builtin_bswap32(x)
#define be32toh(x) (x)
#define le32toh(x) __builtin_bswap32(x)

#define htobe64(x) (x)
#define htole64(x) __builtin_bswap64(x)
#define be64toh(x) (x)
#define le64toh(x) __builtin_bswap64(x)

#else

#error byte order not supported

#endif

#define __BYTE_ORDER BYTE_ORDER
#define __BIG_ENDIAN BIG_ENDIAN
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#define __PDP_ENDIAN PDP_ENDIAN

#else

#error platform not supported

#endif

#pragma endregion endian_h

#pragma region engine_framework

#include <stdint.h>

#ifndef NOMINMAX
#define NOMINMAX WINDOWS_IS_ANNOYING_AINT_IT
#endif

#ifdef ME_PLATFORM_WINDOWS
#define METADOT_CDECL __cdecl
#else
#define METADOT_CDECL
#endif

#define METAENGINE_UNUSED(x) (void)x
#define METAENGINE_ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define METAENGINE_KB 1024
#define METAENGINE_MB (METAENGINE_KB * METAENGINE_KB)
#define METAENGINE_GB (METAENGINE_MB * METAENGINE_MB)
#define METAENGINE_SERIALIZE_CHECK(x)                  \
    do {                                               \
        if ((x) != SERIALIZE_SUCCESS) goto cute_error; \
    } while (0)
#define METAENGINE_STATIC_ASSERT(condition, error_message_string) static_assert(condition, error_message_string)
#define METAENGINE_STRINGIZE_INTERNAL(...) #__VA_ARGS__
#define METAENGINE_STRINGIZE(...) METAENGINE_STRINGIZE_INTERNAL(__VA_ARGS__)
#define METAENGINE_OFFSET_OF(T, member) ((size_t)((uintptr_t)(&(((T *)0)->member))))
#define METAENGINE_DEBUG_PRINTF(...)
#define METAENGINE_ALIGN_TRUNCATE(v, n) ((v) & ~((n)-1))
#define METAENGINE_ALIGN_FORWARD(v, n) METAENGINE_ALIGN_TRUNCATE((v) + (n)-1, (n))
#define METAENGINE_ALIGN_TRUNCATE_PTR(p, n) ((void *)METAENGINE_ALIGN_TRUNCATE((uintptr_t)(p), n))
#define METAENGINE_ALIGN_FORWARD_PTR(p, n) ((void *)METAENGINE_ALIGN_FORWARD((uintptr_t)(p), n))

#include <stdlib.h>

// -------------------------------------------------------------------------------------------------
// Avoid including <utility> header to reduce compile times.

template <typename T>
struct metadot_remove_reference {
    using type = T;
};

template <typename T>
struct metadot_remove_reference<T &> {
    using type = T;
};

template <typename T>
struct metadot_remove_reference<T &&> {
    using type = T;
};

template <typename T>
constexpr typename metadot_remove_reference<T>::type &&metadot_move(T &&arg) noexcept {
    return (typename metadot_remove_reference<T>::type &&) arg;
}

// -------------------------------------------------------------------------------------------------
// Avoid including <initializer_list> header to reduce compile times.
// Unfortunately this class *must* be in the std:: namespace or things won't compile. So we try to
// avoid defining this class if someone already included <initializer_list> before including
// cute framework <cute.h>.

#ifdef ME_PLATFORM_WINDOWS

#if !defined(_INITIALIZER_LIST_) && !defined(_INITIALIZER_LIST) && !defined(_LIBCPP_INITIALIZER_LIST)
#define _INITIALIZER_LIST_        // MSVC
#define _INITIALIZER_LIST         // GCC
#define _LIBCPP_INITIALIZER_LIST  // Clang
// Will probably need to add more here for more compilers later.

namespace std {
template <typename T>
class initializer_list {
public:
    using value_type = T;
    using reference = const T &;
    using const_reference = const T &;
    using size_type = size_t;

    using iterator = const T *;
    using const_iterator = const T *;

    constexpr initializer_list() noexcept : m_first(0), m_last(0) {}

    constexpr initializer_list(const T *first, const T *last) noexcept : m_first(first), m_last(last) {}

    constexpr const T *begin() const noexcept { return m_first; }
    constexpr const T *end() const noexcept { return m_last; }
    constexpr size_t size() const noexcept { return (size_t)(m_last - m_first); }

private:
    const T *m_first;
    const T *m_last;
};

template <class T>
constexpr const T *begin(initializer_list<T> list) noexcept {
    return list.begin();
}
template <class T>
constexpr const T *end(initializer_list<T> list) noexcept {
    return list.end();
}
}  // namespace std

#endif

#else  // ME_PLATFORM_WINDOWS

#include <initializer_list>

#endif  // ME_PLATFORM_WINDOWS

template <typename T>
using METAENGINE_InitializerList = std::initializer_list<T>;

namespace MetaEngine {
template <typename T>
using initializer_list = METAENGINE_InitializerList<T>;

template <typename T>
using remove_reference = metadot_remove_reference<T>;
}  // namespace MetaEngine

// Not sure where to put this... Here is good I guess.
ME_INLINE uint64_t metadot_fnv1a(const void *data, int size) {
    const char *s = (const char *)data;
    uint64_t h = 14695981039346656037ULL;
    char c = 0;
    while (size--) {
        h = h ^ (uint64_t)(*s++);
        h = h * 1099511628211ULL;
    }
    return h;
}

#pragma endregion engine_framework

#endif
