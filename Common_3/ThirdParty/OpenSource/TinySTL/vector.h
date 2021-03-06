/*-
 * Copyright 2012-2015 Matthew Endsley
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TINYSTL_VECTOR_H
#define TINYSTL_VECTOR_H

#include "allocator.h"
#include "buffer.h"
#include "new.h"
#include "stddef.h"

namespace tinystl {
	template<typename T, typename Alloc = TINYSTL_ALLOCATOR>
	class vector {
	public:
		vector();
		vector(const vector& other);
		vector(size_t size);
		vector(size_t size, const T& value);
		vector(const T* first, const T* last);
		~vector();

		vector& operator=(const vector& other);

		void assign(const T* first, const T* last);

		const T* data() const;
		T* data();
		size_t size() const;
		size_t capacity() const;
		bool empty() const;

		T& operator[](size_t idx);
		const T& operator[](size_t idx) const;

		const T& front() const;
		T& front();
		const T& back() const;
		T& back();

		void resize(size_t size);
		void resize(size_t size, const T& value);
		void clear();
		void reserve(size_t capacity);

		void push_back(const T& t);
		void pop_back();

		void emplace_back();
		template<typename Param>
		void emplace_back(const Param& param);

		void shrink_to_fit();

		void swap(vector& other);

		typedef T value_type;

		typedef T* iterator;
		iterator begin();
		iterator end();

		typedef const T* const_iterator;
		const_iterator begin() const;
		const_iterator end() const;

		void insert(iterator where);
		void insert(iterator where, const T& value);
		void insert(iterator where, const T* first, const T* last);

		void insert(size_t index, const T& src)
		{
			const size_t oldCount = size();
			resize(oldCount + 1);
			if (index < oldCount)
				memmove(m_buffer.first + (index + 1), m_buffer.first + index, (oldCount - index) * sizeof(T));
			m_buffer.first [index] = src;
		}

		template<typename Param>
		void emplace(iterator where, const Param& param);

		iterator erase(iterator where);
		iterator erase(iterator first, iterator last);

		iterator erase_unordered(iterator where);
		iterator erase_unordered(iterator first, iterator last);

		// [Confetti backwards compatibility]
		T* getArray() const { return m_buffer.first; }
		T* abandonArray();
		unsigned int getCount() const { return static_cast<unsigned int>(size()); }
		void setCount(const unsigned int newCount);
		const unsigned int add(const T& t);
		void remove(const unsigned int index);
		void orderedRemove(const unsigned int index);
		void fastRemove(const unsigned int index);
		void reset();

		inline bool contains(const T& other) const {
			for (unsigned i = 0; i < getCount(); ++i)
				if (m_buffer.first[i] == other)
					return true;

			return false;
		}

		inline iterator find(const T& other) const {
			for (unsigned i = 0; i < getCount(); ++i)
				if (m_buffer.first[i] == other)
					return &m_buffer.first[i];

			return m_buffer.last;
		}

	private:
		int partition(int(*compare)(const T &elem0, const T &elem1), int p, int r);
		void quickSort(int(*compare)(const T &elem0, const T &elem1), int p, int r);

	public:
		void sort (int (*compare)(const T &elem0, const T &elem1));
		void sort (unsigned begin, unsigned end, int (*compare)(const T &elem0, const T &elem1));
		// [Confetti backwards compatibility]

	private:
		buffer<T, Alloc> m_buffer;
	};

	template<typename T, typename Alloc>
	inline vector<T, Alloc>::vector() {
		buffer_init(&m_buffer);
	}

	template<typename T, typename Alloc>
	inline vector<T, Alloc>::vector(const vector& other) {
		buffer_init(&m_buffer);
		buffer_reserve(&m_buffer, other.size());
		buffer_insert(&m_buffer, m_buffer.last, other.m_buffer.first, other.m_buffer.last);
	}

	template<typename T, typename Alloc>
	inline vector<T, Alloc>::vector(size_t size) {
		buffer_init(&m_buffer);
		buffer_resize(&m_buffer, size);
	}

	template<typename T, typename Alloc>
	inline vector<T, Alloc>::vector(size_t size, const T& value) {
		buffer_init(&m_buffer);
		buffer_resize(&m_buffer, size, value);
	}

	template<typename T, typename Alloc>
	inline vector<T, Alloc>::vector(const T* first, const T* last) {
		buffer_init(&m_buffer);
		buffer_insert(&m_buffer, m_buffer.last, first, last);
	}

	template<typename T, typename Alloc>
	inline vector<T, Alloc>::~vector() {
		buffer_destroy(&m_buffer);
	}

	template<typename T, typename Alloc>
	inline vector<T, Alloc>& vector<T, Alloc>::operator=(const vector& other) {
		vector(other).swap(*this);
		return *this;
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::assign(const T* first, const T* last) {
		buffer_clear(&m_buffer);
		buffer_insert(&m_buffer, m_buffer.last, first, last);
	}

	template<typename T, typename Alloc>
	inline const T* vector<T, Alloc>::data() const {
		return m_buffer.first;
	}

	template<typename T, typename Alloc>
	inline T* vector<T, Alloc>::data() {
		return m_buffer.first;
	}

	template<typename T, typename Alloc>
	inline size_t vector<T, Alloc>::size() const {
		return (size_t)(m_buffer.last - m_buffer.first);
	}

	template<typename T, typename Alloc>
	inline size_t vector<T, Alloc>::capacity() const {
		return (size_t)(m_buffer.capacity - m_buffer.first);
	}

	template<typename T, typename Alloc>
	inline bool vector<T, Alloc>::empty() const {
		return m_buffer.last == m_buffer.first;
	}

	template<typename T, typename Alloc>
	inline T& vector<T, Alloc>::operator[](size_t idx) {
		return m_buffer.first[idx];
	}

	template<typename T, typename Alloc>
	inline const T& vector<T, Alloc>::operator[](size_t idx) const {
		return m_buffer.first[idx];
	}

	template<typename T, typename Alloc>
	inline const T& vector<T, Alloc>::front() const {
		return m_buffer.first[0];
	}

	template<typename T, typename Alloc>
	inline T& vector<T, Alloc>::front() {
		return m_buffer.first[0];
	}

	template<typename T, typename Alloc>
	inline const T& vector<T, Alloc>::back() const {
		return m_buffer.last[-1];
	}

	template<typename T, typename Alloc>
	inline T& vector<T, Alloc>::back() {
		return m_buffer.last[-1];
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::resize(size_t size) {
		buffer_resize(&m_buffer, size);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::resize(size_t size, const T& value) {
		buffer_resize(&m_buffer, size, value);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::clear() {
		buffer_clear(&m_buffer);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::reserve(size_t capacity) {
		buffer_reserve(&m_buffer, capacity);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::push_back(const T& t) {
		buffer_append(&m_buffer, &t);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::emplace_back() {
		buffer_append(&m_buffer);
	}

	template<typename T, typename Alloc>
	template<typename Param>
	inline void vector<T, Alloc>::emplace_back(const Param& param) {
		buffer_append(&m_buffer, &param);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::pop_back() {
		buffer_erase(&m_buffer, m_buffer.last - 1, m_buffer.last);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::shrink_to_fit() {
		buffer_shrink_to_fit(&m_buffer);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::swap(vector& other) {
		buffer_swap(&m_buffer, &other.m_buffer);
	}

	template<typename T, typename Alloc>
	inline typename vector<T, Alloc>::iterator vector<T, Alloc>::begin() {
		return m_buffer.first;
	}

	template<typename T, typename Alloc>
	inline typename vector<T, Alloc>::iterator vector<T, Alloc>::end() {
		return m_buffer.last;
	}

	template<typename T, typename Alloc>
	inline typename vector<T, Alloc>::const_iterator vector<T, Alloc>::begin() const {
		return m_buffer.first;
	}

	template<typename T, typename Alloc>
	inline typename vector<T, Alloc>::const_iterator vector<T, Alloc>::end() const {
		return m_buffer.last;
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::insert(typename vector::iterator where) {
		buffer_insert(&m_buffer, where, 1);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::insert(iterator where, const T& value) {
		buffer_insert(&m_buffer, where, &value, &value + 1);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::insert(iterator where, const T* first, const T* last) {
		buffer_insert(&m_buffer, where, first, last);
	}

	template<typename T, typename Alloc>
	inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator where) {
		return buffer_erase(&m_buffer, where, where + 1);
	}

	template<typename T, typename Alloc>
	inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last) {
		return buffer_erase(&m_buffer, first, last);
	}

	template<typename T, typename Alloc>
	inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase_unordered(iterator where) {
		return buffer_erase_unordered(&m_buffer, where, where + 1);
	}

	template<typename T, typename Alloc>
	inline typename vector<T, Alloc>::iterator vector<T, Alloc>::erase_unordered(iterator first, iterator last) {
		return buffer_erase_unordered(&m_buffer, first, last);
	}

	template<typename T, typename Alloc>
	template<typename Param>
	void vector<T, Alloc>::emplace(typename vector::iterator where, const Param& param) {
		buffer_insert(&m_buffer, where, &param, &param + 1);
	}

	template<typename T, typename Alloc>
	T* vector<T, Alloc>::abandonArray()
	{
		T* r = m_buffer.first;
		buffer_init(&m_buffer);
		return r;
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::setCount(const unsigned int newCount)
	{
		buffer_resize(&m_buffer, newCount);
	}

	template<typename T, typename Alloc>
	inline const unsigned int vector<T, Alloc>::add(const T& t)
	{
		push_back(t);
		return static_cast<unsigned int>(size() - 1);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::fastRemove(const unsigned int index)
	{
		// Fast remove used to cause a memory leak in the old stl
		// Just remove regularly instead
		erase(m_buffer.first + index);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::remove(const unsigned int index)
	{
		erase(m_buffer.first + index);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::orderedRemove(const unsigned int index)
	{
		erase(m_buffer.first + index);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::reset()
	{
		clear();
	}

	template<typename T, typename Alloc>
	inline int vector<T, Alloc>::partition(int(*compare)(const T &elem0, const T &elem1), int p, int r)
	{
		T tmp, pivot = m_buffer.first[p];
		int left = p;

		for (int i = p + 1; i <= r; i++) {
			if (compare(m_buffer.first[i], pivot) < 0) {
				left++;
				tmp = m_buffer.first[i];
				m_buffer.first[i] = m_buffer.first[left];
				m_buffer.first[left] = tmp;
			}
		}
		tmp = m_buffer.first[p];
		m_buffer.first[p] = m_buffer.first[left];
		m_buffer.first[left] = tmp;
		return left;
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::quickSort(int(*compare)(const T &elem0, const T &elem1), int p, int r)
	{
		if (p < r) {
			int q = partition(compare, p, r);
			quickSort(compare, p, q - 1);
			quickSort(compare, q + 1, r);
		}
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::sort(int(*compare)(const T &elem0, const T &elem1))
	{
		quickSort(compare, 0, (int)size() - 1);
	}

	template<typename T, typename Alloc>
	inline void vector<T, Alloc>::sort (unsigned begin, unsigned end, int (*compare)(const T &elem0, const T &elem1))
	{
		quickSort (compare, (int)begin, (int)end);
	}
}

#endif
