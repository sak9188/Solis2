#pragma once

#include "folly/FBString.h"
#include "folly/FBVector.h"
#include "folly/AtomicHashMap.h"

#include "glm/glm.hpp"

#ifndef offsetof
#define offsetof(s, m) (size_t) & (((s *)0)->m)
#endif

namespace solis
{
	using string = folly::fbstring;

	// template <typename T>
	// using vector = folly::fbvector<T>;

	template <typename T>
	using vector = std::vector<T>;

	template <typename K, typename V>
	using hash_map = folly::AtomicHashMap<K, V>;

	namespace math = glm;
}

namespace std
{
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
}
