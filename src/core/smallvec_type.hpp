/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file smallvec_type.hpp Simple vector class that allows allocating an item without the need to copy this->data needlessly. */

#ifndef SMALLVEC_TYPE_HPP
#define SMALLVEC_TYPE_HPP

#include "alloc_func.hpp"
#include "mem_func.hpp"
#include <vector>
#include <algorithm>

/**
 * Simple vector template class.
 *
 * @note There are no asserts in the class so you have
 *       to care about that you grab an item which is
 *       inside the list.
 *
 * @tparam T The type of the items stored
 * @tparam S The steps of allocation
 */
template <typename T, uint S>
class SmallVector : public std::vector<T> {
public:
	SmallVector() = default;

	/**
	 * Copy constructor.
	 * @param other The other vector to copy.
	 */
	SmallVector(const SmallVector &other) = default;

	/**
	 * Generic copy constructor.
	 * @param other The other vector to copy.
	 */
	template <uint X>
	SmallVector(const SmallVector<T, X> &other) : std::vector<T>(other)
	{
	}

	/**
	 * Assignment.
	 * @param other The other vector to assign.
	 */
	SmallVector &operator=(const SmallVector &other) = default;

	/**
	 * Generic assignment.
	 * @param other The other vector to assign.
	 */
	template <uint X>
	SmallVector &operator=(const SmallVector<T, X> &other)
	{
		std::vector<T>::operator=(other);
		return *this;
	}

	~SmallVector() = default;

	/**
	 * Insert a new item at a specific position into the vector, moving all following items.
	 * @param item Position at which the new item should be inserted
	 * @return pointer to the new item
	 */
	inline T *Insert(T *item)
	{
		assert(item >= this->Begin() && item <= this->End());

		size_t start = item - this->Begin();
		std::vector<T>::insert(std::vector<T>::begin() + start);
		return this->Begin() + start;
	}

	/**
	 * Search for the first occurrence of an item.
	 * The '!=' operator of T is used for comparison.
	 * @param item Item to search for
	 * @return The position of the item, or -1 when not present
	 */
	inline int FindIndex(const T &item) const
	{
		auto const it = std::find(std::vector<T>::begin(), std::vector<T>::end(), item);
		return it == std::vector<T>::end() ? -1 : it - std::vector<T>::begin();
	}

	/**
	 * Tests whether a item is present in the vector, and appends it to the end if not.
	 * The '!=' operator of T is used for comparison.
	 * @param item Item to test for
	 * @return true iff the item is was already present
	 */
	inline bool Include(const T &item)
	{
		bool is_member = std::find(std::vector<T>::begin(), std::vector<T>::end(), item) != std::vector<T>::end();
		if (!is_member) std::vector<T>::emplace_back(item);
		return is_member;
	}

	/**
	 * Get the pointer to the first item (const)
	 *
	 * @return the pointer to the first item
	 */
	inline const T *Begin() const
	{
		return std::vector<T>::data();
	}

	/**
	 * Get the pointer to the first item
	 *
	 * @return the pointer to the first item
	 */
	inline T *Begin()
	{
		return std::vector<T>::data();
	}

	/**
	 * Get the pointer behind the last valid item (const)
	 *
	 * @return the pointer behind the last valid item
	 */
	inline const T *End() const
	{
		return std::vector<T>::data() + std::vector<T>::size();
	}

	/**
	 * Get the pointer behind the last valid item
	 *
	 * @return the pointer behind the last valid item
	 */
	inline T *End()
	{
		return std::vector<T>::data() + std::vector<T>::size();
	}
};

/**
 * Helper function to extend a vector by more than one element
 * Consider using std::back_inserter in new code
 *
 * @param vec A reference to the vector to be extended
 * @param num The number of elements to default-construct
 *
 * @return Pointer to the first new element
 */
template <typename T>
inline T* grow(std::vector<T>& vec, std::size_t num)
{
	const std::size_t pos = vec.size();
	vec.resize(pos + num);
	return vec.data() + pos;
}


/**
 * Simple vector template class, with automatic free.
 *
 * @note There are no asserts in the class so you have
 *       to care about that you grab an item which is
 *       inside the list.
 *
 * @param T The type of the items stored, must be a pointer
 * @param S The steps of allocation
 */
template <typename T, uint S>
class AutoFreeSmallVector : public SmallVector<T, S> {
public:
	~AutoFreeSmallVector()
	{
		this->Clear();
	}

	/**
	 * Remove all items from the list.
	 */
	inline void Clear()
	{
		for (uint i = 0; i < std::vector<T>::size(); i++) {
			free(std::vector<T>::operator[](i));
		}

		std::vector<T>::clear();
	}
};

/**
 * Simple vector template class, with automatic delete.
 *
 * @note There are no asserts in the class so you have
 *       to care about that you grab an item which is
 *       inside the list.
 *
 * @param T The type of the items stored, must be a pointer
 * @param S The steps of allocation
 */
template <typename T, uint S>
class AutoDeleteSmallVector : public SmallVector<T, S> {
public:
	~AutoDeleteSmallVector()
	{
		this->Clear();
	}

	/**
	 * Remove all items from the list.
	 */
	inline void Clear()
	{
		for (uint i = 0; i < std::vector<T>::size(); i++) {
			delete std::vector<T>::operator[](i);
		}

		std::vector<T>::clear();
	}
};

typedef AutoFreeSmallVector<char*, 4> StringList; ///< Type for a list of strings.

#endif /* SMALLVEC_TYPE_HPP */
