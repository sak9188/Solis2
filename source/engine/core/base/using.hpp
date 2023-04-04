#pragma once

#include <unordered_map>
#include <map>

#include "folly/FBString.h"
#include "folly/FBVector.h"
#include "folly/AtomicHashMap.h"
#include "folly/AtomicLinkedList.h"

#include "glm/glm.hpp"
namespace solis {

using string = folly::fbstring;

template <typename T>
using vector = folly::fbvector<T>;

// template <typename T>
// using vector = std::vector<T>;

// 确定数量下的hashmap
template <typename K, typename V>
using hash_map = folly::AtomicHashMap<K, V>;

// 不确定数量下的hashmap
template <typename K, typename V>
using dict_map = std::unordered_map<K, V>;

template <typename K, typename V>
using sort_map = std::map<K, V>;

namespace math = glm;

} // namespace solis

namespace std {
template <>
struct hash<void *>
{
    size_t operator()(void *ptr) const
    {
        return reinterpret_cast<size_t>(ptr);
    }
};

template <>
struct equal_to<void *>
{
    bool operator()(void *lhs, void *rhs) const
    {
        return lhs == rhs;
    }
};
} // namespace std
