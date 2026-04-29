#pragma once

#include <cstddef>


inline constexpr size_t MEMORY_SIZE = 262'144; // дано 256Кб
inline constexpr size_t ADDRESS_LEN = 18; // log2(MEMORY_SIZE)
inline constexpr size_t CACHE_TAG_LEN = 8; // дано 8 бит
inline constexpr size_t CACHE_INDEX_LEN = 5; // log2(CACHE_SET_COUNT)
inline constexpr size_t CACHE_OFFSET_LEN = 5; // ADDRESS_LEN - TAG_LEN - INDEX_LEN
inline constexpr size_t CACHE_SIZE = 4096; // CACHE_LINE_COUNT * CACHE_LINE_SIZE
inline constexpr size_t CACHE_LINE_SIZE = 32; // 2 ^ OFFSET_LEN
inline constexpr size_t CACHE_LINE_COUNT = 128; // CACHE_SET_COUNT * CACHE_WAY
inline constexpr size_t CACHE_SET_COUNT = 32; // дано 
inline constexpr size_t CACHE_WAY = 4; // дано

inline constexpr size_t kRegistersCount = 32;
inline constexpr size_t kRegisterSize = 32;
inline constexpr size_t kInstrSizeByte = 4;
inline constexpr size_t kInstrSizeBit = 32;
