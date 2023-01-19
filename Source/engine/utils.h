// Copyright(c) 2022-2023, KaoruXun All rights reserved.

#ifndef _METADOT_UTILS_H_
#define _METADOT_UTILS_H_

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "core/alloc.h"
#include "core/c/list.h"
#include "core/const.h"
#include "core/core.h"
#include "engine/mathlib.h"
#include "libs/cJSON.h"
#include "libs/lua/host/lauxlib.h"
#include "libs/lua/host/lua.h"
#include "libs/lua/host/lualib.h"
#include "sdl_wrapper.h"

extern U16 frameTimes[FrameTimeNum];
extern U32 frameCount;

#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#ifndef FILENAME_MAX
#define FILENAME_MAX 256
#endif

// #define max(a, b)                                                                                  \
//     ({                                                                                             \
//         __typeof__(a) _a = (a);                                                                    \
//         __typeof__(b) _b = (b);                                                                    \
//         _a > _b ? _a : _b;                                                                         \
//     })

// #define min(a, b)                                                                                  \
//     ({                                                                                             \
//         __typeof__(a) _a = (a);                                                                    \
//         __typeof__(b) _b = (b);                                                                    \
//         _a < _b ? _a : _b;                                                                         \
//     })

/* --- PRINTF_BYTE_TO_BINARY macro's --- */
#define PRINTF_BINARY_PATTERN_INT8 "%c%c%c%c%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY_INT8(i)                                                                                                                                       \
    (((i)&0x80ll) ? '1' : '0'), (((i)&0x40ll) ? '1' : '0'), (((i)&0x20ll) ? '1' : '0'), (((i)&0x10ll) ? '1' : '0'), (((i)&0x08ll) ? '1' : '0'), (((i)&0x04ll) ? '1' : '0'), \
            (((i)&0x02ll) ? '1' : '0'), (((i)&0x01ll) ? '1' : '0')

#define PRINTF_BINARY_PATTERN_INT16 PRINTF_BINARY_PATTERN_INT8 PRINTF_BINARY_PATTERN_INT8
#define PRINTF_BYTE_TO_BINARY_INT16(i) PRINTF_BYTE_TO_BINARY_INT8((i) >> 8), PRINTF_BYTE_TO_BINARY_INT8(i)
#define PRINTF_BINARY_PATTERN_INT32 PRINTF_BINARY_PATTERN_INT16 PRINTF_BINARY_PATTERN_INT16
#define PRINTF_BYTE_TO_BINARY_INT32(i) PRINTF_BYTE_TO_BINARY_INT16((i) >> 16), PRINTF_BYTE_TO_BINARY_INT16(i)
#define PRINTF_BINARY_PATTERN_INT64 PRINTF_BINARY_PATTERN_INT32 PRINTF_BINARY_PATTERN_INT32
#define PRINTF_BYTE_TO_BINARY_INT64(i) PRINTF_BYTE_TO_BINARY_INT32((i) >> 32), PRINTF_BYTE_TO_BINARY_INT32(i)
/* --- end macros --- */

// vsnprintf replacement from Valentin Milea:
// http://stackoverflow.com/questions/2915672/snprintf-and-visual-studio-2010
#if defined(_MSC_VER) && _MSC_VER < 1900

#define snprintf c99_snprintf
#define vsnprintf c99_vsnprintf

METADOT_INLINE int c99_vsnprintf(char* outBuf, size_t size, const char* format, va_list ap) {
    int count = -1;

    if (size != 0) count = _vsnprintf_s(outBuf, size, _TRUNCATE, format, ap);
    if (count == -1) count = _vscprintf(format, ap);

    return count;
}

METADOT_INLINE int c99_snprintf(char* outBuf, size_t size, const char* format, ...) {
    int count;
    va_list ap;

    va_start(ap, format);
    count = c99_vsnprintf(outBuf, size, format, ap);
    va_end(ap);

    return count;
}

#endif

#ifndef RE_DOT_MATCHES_NEWLINE
/* Define to 0 if you DON'T want '.' to match '\r' + '\n' */
#define RE_DOT_MATCHES_NEWLINE 1
#endif

/* Typedef'd pointer to get abstract datatype. */
typedef struct regex_t* re_t;

/* Compile regex string pattern to a regex_t-array. */
re_t re_compile(const char* pattern);

/* Find matches of the compiled pattern inside text. */
int re_matchp(re_t pattern, const char* text, int* matchlength);

/* Find matches of the txt pattern inside text (will compile automatically first). */
int re_match(const char* pattern, const char* text, int* matchlength);

// Trie structures and interface

// Add all supported types here, as 'Trie_type'
typedef enum TrieType { Trie_None, Trie_Pointer, Trie_String, Trie_metadot_vec3, Trie_double, Trie_float, Trie_char, Trie_int } TrieType;

// Structure used to retrieve all the data from the trie
// Add all supported types inside the union, as 'type* typeValue'
typedef struct TrieElement {
    char* key;
    TrieType type;
    unsigned size;
    union {
        void* pointerValue;
        char* stringValue;
        metadot_vec3* vector3Value;
        F64* doubleValue;
        F32* floatValue;
        char* charValue;
        int* intValue;
    };
} TrieElement;

#define TRIE_ALPHABET_SIZE 256
typedef struct TrieCell {
    TrieType elementType;
    unsigned maxKeySize;
    union {
        unsigned numberOfElements;
        unsigned elementSize;
    };
    // In a leaf and branch, the '\0' points to the data stored, while in a trunk it points to NULL
    // In both branch and trunk, all the other characters points to other Tries
    void* branch[TRIE_ALPHABET_SIZE];
} Trie;

Trie InitTrie();
void FreeTrie(Trie* trie);

void InsertTrie(Trie* trie, const char* key, const void* value, int size, TrieType valueType);
void InsertTrieString(Trie* trie, const char* key, const char* value);

int TrieContainsKey(Trie trie, const char* key);
void* GetTrieElement(Trie trie, const char* key);
void* GetTrieElementWithProperties(Trie trie, const char* key, int* sizeOut, TrieType* typeOut);
void* GetTrieElementAsPointer(Trie trie, const char* key, void* defaultValue);
char* GetTrieElementAsString(Trie trie, const char* key, char* defaultValue);

// Macro to generate headers for the insertion and retrieval functions
// Remember to call the function template macro on utils.c and to modify the TrieType enum when adding more types
#define TRIE_TYPE_FUNCTION_HEADER_MACRO(type)                        \
    void InsertTrie_##type(Trie* trie, const char* key, type value); \
    type GetTrieElementAs_##type(Trie trie, const char* key, type defaultValue);

TRIE_TYPE_FUNCTION_HEADER_MACRO(metadot_vec3)
TRIE_TYPE_FUNCTION_HEADER_MACRO(F64)
TRIE_TYPE_FUNCTION_HEADER_MACRO(F32)
TRIE_TYPE_FUNCTION_HEADER_MACRO(char)
TRIE_TYPE_FUNCTION_HEADER_MACRO(int)

// Functions to obtain all data inside a trie
// The data returned should be used before any new replace modifications
// are made in the trie, as the data pointed can be freed when replaced
TrieElement* GetTrieElementsArray(Trie trie, int* outElementsCount);
void FreeTrieElementsArray(TrieElement* elementsArray, int elementsCount);

void InitFPS();
void ProcessFPS();
F32 GetFPS();


cJSON* OpenJSON(char path[], char name[]);
F64 JSON_GetObjectDouble(cJSON* object, char* string, F64 defaultValue);
metadot_vec3 JSON_GetObjectVector3(cJSON* object, char* string, metadot_vec3 defaultValue);
cJSON* JSON_CreateVector3(metadot_vec3 value);

void Vector3ToTable(lua_State* L, metadot_vec3 vector);

int StringCompareEqual(char* stringA, char* stringB);
int StringCompareEqualCaseInsensitive(char* stringA, char* stringB);

inline R_bool startsWith(const char* s, const char* prefix) { return strlen(prefix) <= strlen(s) && (strncmp(prefix, s, strlen(prefix)) == 0); }

#ifndef Z_DEFAULT_WINDOW_BITS
#define Z_DEFAULT_WINDOW_BITS 15
#endif

#ifndef NBT_BUFFER_SIZE
#define NBT_BUFFER_SIZE 32768
#endif

#define NBT_COMPRESSION_LEVEL 9

typedef enum {
    NBT_TYPE_END,
    NBT_TYPE_BYTE,
    NBT_TYPE_SHORT,
    NBT_TYPE_INT,
    NBT_TYPE_LONG,
    NBT_TYPE_FLOAT,
    NBT_TYPE_DOUBLE,
    NBT_TYPE_BYTE_ARRAY,
    NBT_TYPE_STRING,
    NBT_TYPE_LIST,
    NBT_TYPE_COMPOUND,
    NBT_TYPE_INT_ARRAY,
    NBT_TYPE_LONG_ARRAY,
    NBT_NO_OVERRIDE  // Only used internally.
} nbt_tag_type_t;

typedef struct nbt_tag_t nbt_tag_t;

struct nbt_tag_t {

    nbt_tag_type_t type;

    char* name;
    size_t name_size;

    union {
        struct {
            I8 value;
        } tag_byte;
        struct {
            I16 value;
        } tag_short;
        struct {
            I32 value;
        } tag_int;
        struct {
            I64 value;
        } tag_long;
        struct {
            F32 value;
        } tag_float;
        struct {
            F64 value;
        } tag_double;
        struct {
            I8* value;
            size_t size;
        } tag_byte_array;
        struct {
            char* value;
            size_t size;
        } tag_string;
        struct {
            nbt_tag_t** value;
            nbt_tag_type_t type;
            size_t size;
        } tag_list;
        struct {
            nbt_tag_t** value;
            size_t size;
        } tag_compound;
        struct {
            I32* value;
            size_t size;
        } tag_int_array;
        struct {
            I64* value;
            size_t size;
        } tag_long_array;
    };
};

typedef struct {
    size_t (*read)(void* userdata, U8* data, size_t size);
    void* userdata;
} nbt_reader_t;

typedef struct {
    size_t (*write)(void* userdata, U8* data, size_t size);
    void* userdata;
} nbt_writer_t;

typedef enum {
    NBT_PARSE_FLAG_USE_GZIP = 1,
    NBT_PARSE_FLAG_USE_ZLIB = 2,
    NBT_PARSE_FLAG_USE_RAW = 3,
} nbt_parse_flags_t;

typedef enum { NBT_WRITE_FLAG_USE_GZIP = 1, NBT_WRITE_FLAG_USE_ZLIB = 2, NBT_WRITE_FLAG_USE_RAW = 3 } nbt_write_flags_t;

nbt_tag_t* nbt_parse(nbt_reader_t reader, int parse_flags);
void nbt_write(nbt_writer_t writer, nbt_tag_t* tag, int write_flags);

nbt_tag_t* nbt_new_tag_byte(I8 value);
nbt_tag_t* nbt_new_tag_short(I16 value);
nbt_tag_t* nbt_new_tag_int(I32 value);
nbt_tag_t* nbt_new_tag_long(I64 value);
nbt_tag_t* nbt_new_tag_float(F32 value);
nbt_tag_t* nbt_new_tag_double(F64 value);
nbt_tag_t* nbt_new_tag_byte_array(I8* value, size_t size);
nbt_tag_t* nbt_new_tag_string(const char* value, size_t size);
nbt_tag_t* nbt_new_tag_list(nbt_tag_type_t type);
nbt_tag_t* nbt_new_tag_compound(void);
nbt_tag_t* nbt_new_tag_int_array(I32* value, size_t size);
nbt_tag_t* nbt_new_tag_long_array(I64* value, size_t size);

void nbt_set_tag_name(nbt_tag_t* tag, const char* name, size_t size);

void nbt_tag_list_append(nbt_tag_t* list, nbt_tag_t* value);
nbt_tag_t* nbt_tag_list_get(nbt_tag_t* tag, size_t index);
void nbt_tag_compound_append(nbt_tag_t* compound, nbt_tag_t* value);
nbt_tag_t* nbt_tag_compound_get(nbt_tag_t* tag, const char* key);

void nbt__free_tag(nbt_tag_t* tag);

#endif