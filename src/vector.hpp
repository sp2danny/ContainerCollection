
//#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <cassert>
#include <iterator>
#include <initializer_list>
#include <limits>
#include <stdexcept>

template<typename T>
class vector
{
public:
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T* iterator;
	typedef const T* const_iterator;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	vector();
	void clear();
	~vector();
	void reserve(std::size_t);
	std::size_t size() const;
	std::size_t capacity() const;
	static std::size_t max_size() { return std::numeric_limits<std::size_t>::max(); }
	void push_back(const T&);
	void push_back(T&&);
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;
	T& front();
	const T& front() const;
	T& back();
	const T& back() const;
	void pop_back();
	iterator insert(iterator, const T&);
	void insert(iterator position, size_type n, const value_type& val);
	iterator erase(iterator);
	void swap(vector&) noexcept;
	template<typename It>
	void assign(It, It);
	void assign(std::initializer_list<T>);
	void assign(std::size_t, const T&);
	vector(const vector&);
	vector(vector&&);
	vector& operator=(const vector&);
	vector& operator=(vector&&);
	vector(std::initializer_list<T> il);
	vector(std::size_t sz, const T& val);
	template<typename It>
	vector(It, It);
	typedef std::reverse_iterator<iterator> reverse_iterator;
	reverse_iterator rbegin();
	reverse_iterator rend();
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	const_reverse_iterator rbegin() const;
	const_reverse_iterator rend() const;
	const_reverse_iterator crbegin() const;
	const_reverse_iterator crend() const;
	template<typename It>
	void insert(iterator, It, It);
	void erase(iterator, iterator);
	T& operator[](std::size_t);
	const T& operator[](std::size_t) const;
	T& at(std::size_t);
	const T& at(std::size_t) const;
	void shrink_to_fit();
	template<typename... Args>
	T& emplace_back(Args&&...);
	void resize(std::size_t, const T& = T{});
	template<typename... Args>
	T& emplace(iterator, Args&&...);

private:
	bool is_full() const;
	void make_space_for(iterator, std::size_t);
	T* m_data;
	std::size_t m_size;
	std::size_t m_capa;
};

template<typename T>
vector<T>::vector()
{
	m_data = std::allocator<T>{}.allocate(1);
	m_size = 0;
	m_capa = 1;
}

template<typename T>
void vector<T>::clear()
{
	std::size_t idx;
	for (idx=0; idx<m_size; ++idx)
	{
		m_data[idx].~T();
	}
	m_size = 0;
}

template<typename T>
vector<T>::~vector()
{
	clear();
	std::allocator<T>{}.deallocate(m_data, m_capa);
}

template<typename T>
void vector<T>::reserve(std::size_t new_sz)
{
	if (new_sz <= m_capa) return;
	T* ptr = std::allocator<T>{}.allocate(new_sz);
	std::size_t idx;
	for (idx=0; idx<m_size; ++idx)
	{
		new (ptr+idx) T{std::move(m_data[idx])};
	}
	std::allocator<T>{}.deallocate(m_data, m_capa);
	m_data = ptr;
	m_capa = new_sz;
}

template<typename T>
std::size_t vector<T>::size() const
{
	return m_size;
}

template<typename T>
std::size_t vector<T>::capacity() const
{
	return m_capa;
}

template<typename T>
bool vector<T>::is_full() const
{
	return m_size==m_capa;
}

template<typename T>
void vector<T>::push_back(const T& item)
{
	if (is_full())
		reserve(m_capa*2+1);
	new (m_data + m_size++) T{item};
}

template<typename T>
void vector<T>::push_back(T&& item)
{
	if (is_full())
		reserve(m_capa*2+1);
	new (m_data + m_size++) T{std::move(item)};
}

template<typename T>
auto vector<T>::begin() -> iterator
{
	return m_data;
}

template<typename T>
auto vector<T>::end() -> iterator
{
	return m_data + m_size;
}

template<typename T>
auto vector<T>::begin() const -> const_iterator
{
	return m_data;
}

template<typename T>
auto vector<T>::end() const -> const_iterator
{
	return m_data + m_size;
}

template<typename T>
auto vector<T>::cbegin() const -> const_iterator
{
	return m_data;
}

template<typename T>
auto vector<T>::cend() const -> const_iterator
{
	return m_data + m_size;
}

template<typename T>
T& vector<T>::front()
{
	return m_data[0];
}

template<typename T>
const T& vector<T>::front() const
{
	return m_data[0];
}

template<typename T>
T& vector<T>::back()
{
	return m_data[m_size-1];
}

template<typename T>
const T& vector<T>::back() const
{
	return m_data[m_size-1];
}

template<typename T>
void vector<T>::pop_back()
{
	m_data[--m_size].~T();
}

template<typename T>
auto vector<T>::insert(iterator pos, const T& val) -> iterator
{
	if (pos == end())
	{
		push_back(val);
		return end() - 1;
	}
	if (is_full())
	{
		std::size_t new_sz = m_capa*2 + 1;
		T* ptr = std::allocator<T>{}.allocate(new_sz);
		T* src = m_data;
		T* dst = ptr;
		std::size_t idx;
		iterator new_pos;
		for (idx=0; idx<m_size; ++idx)
		{
			if (src == pos)
			{
				new_pos = dst;
				new (dst++) T{val};
			}
			new (dst++) T{std::move(*src)};
			src->~T(); ++src;
		}
		if (src == pos)
		{
			new_pos = dst;
			new (dst++) T{val};
		}
		assert(src == end());
		std::allocator<T>{}.deallocate(m_data, m_capa);
		m_data = ptr;
		m_size += 1;
		m_capa = new_sz;
		return new_pos;
	} else {
		T* ptr = end();
		new (ptr) T{std::move(*(ptr-1))};
		--ptr;
		while (true)
		{
			if (ptr == pos) break;
			(*ptr) = std::move(*(ptr-1));
			--ptr;
		}
		(*ptr) = val;
		m_size += 1;
		return pos;
	}
}

template<typename T>
auto vector<T>::erase(iterator where) -> iterator
{
	T* ptr = where;
	T* lst = end()-1;
	while (true)
	{
		if (ptr == lst) break;
		(*ptr) = std::move(*(ptr+1));
		++ptr;
	}
	ptr->~T();
	--m_size;
	return where;
}

template<typename T>
void vector<T>::swap(vector& other) noexcept
{
	std::swap(m_data, other.m_data);
	std::swap(m_size, other.m_size);
	std::swap(m_capa, other.m_capa);
}

template<typename T>
template<typename It>
void vector<T>::assign(It b, It e)
{
	clear();
	reserve(std::distance(b,e));
	while (b != e)
		push_back(*b++);
}

template<typename T>
void vector<T>::assign(std::initializer_list<T> il)
{
	assign(il.begin(), il.end());
}

template<typename T>
vector<T>::vector(const vector& other)
	: vector()
{
	assign(other.begin(), other.end());
}

template<typename T>
vector<T>::vector(vector&& other)
	: vector()
{
	swap(other);
}

template<typename T>
auto vector<T>::operator=(const vector& other) -> vector&
{
	assign(other.begin(), other.end());
	return *this;
}

template<typename T>
auto vector<T>::operator=(vector&& other) -> vector&
{
	swap(other);
	return *this;
}

template<typename T>
void vector<T>::assign(std::size_t sz, const T& val)
{
	clear();
	reserve(sz);
	while (sz--) push_back(val);	
}

template<typename T>
vector<T>::vector(std::initializer_list<T> il)
	: vector()
{
	assign(il);
}

template<typename T>
vector<T>::vector(std::size_t sz, const T& val)
	: vector()
{
	assign(sz, val);
}

template<typename T>
template<typename It>
vector<T>::vector(It b, It e)
	: vector()
{
	assign(b, e);
}

template<typename T>
auto vector<T>::rbegin() -> reverse_iterator
{
	return reverse_iterator{end()};
}

template<typename T>
auto vector<T>::rend() -> reverse_iterator
{
	return reverse_iterator{begin()};
}

template<typename T>
auto vector<T>::rbegin() const -> const_reverse_iterator
{
	return const_reverse_iterator{cend()};
}

template<typename T>
auto vector<T>::rend() const -> const_reverse_iterator
{
	return const_reverse_iterator{rbegin()};
}

template<typename T>
auto vector<T>::crbegin() const -> const_reverse_iterator
{
	return const_reverse_iterator{cend()};
}

template<typename T>
auto vector<T>::crend() const -> const_reverse_iterator
{
	return const_reverse_iterator{cbegin()};
}

template<typename T>
void vector<T>::make_space_for(iterator where, std::size_t n)
{
	(void)where;
	(void)n;
}

template<typename T>
template<typename It>
void vector<T>::insert(iterator pos, It b, It e)
{
	while (b != e)
	{
		pos = insert(pos, *b);
		++pos; ++b;
	}
}

template<typename T>
void vector<T>::insert(iterator pos, std::size_t n, const T& val)
{
	(void)pos;
	(void)n;
	(void)val;
}

template<typename T>
void vector<T>::erase(iterator b, iterator e)
{
	if (b == e) return;
	T* p = b;
	while (p != e)
	{
		p->~T();
		++p;
		--m_size;
	}
	p = b;
	while (e != end())
	{
		new (p) T{std::move(*e)};
		e->~T();
		++p; ++e;
	}
}

template<typename T>
T& vector<T>::operator[](std::size_t idx)
{
	return m_data[idx];
}

template<typename T>
const T& vector<T>::operator[](std::size_t idx) const
{
	return m_data[idx];
}

template<typename T>
T& vector<T>::at(std::size_t idx)
{
	if (idx >= m_size)
		throw std::out_of_range("out of range");
	return m_data[idx];
}

template<typename T>
const T& vector<T>::at(std::size_t idx) const
{
	if (idx >= m_size)
		throw std::out_of_range("out of range");
	return m_data[idx];
}

template<typename T>
void vector<T>::shrink_to_fit()
{
	if (m_size == m_capa) return;
	T* ptr = std::allocator<T>{}.allocate(m_size);
	T* data = ptr;
	T* b = begin();
	T* e = end();
	while (b != e)
	{
		new (ptr) T{std::move(*b)};
		++ptr; ++b;
	}
	std::allocator<T>{}.deallocate(m_data, m_capa);
	m_data = data;
	m_capa = m_size;
}

template<typename T>
template<typename... Args>
T& vector<T>::emplace_back(Args&&... args)
{
	if (is_full())
		reserve(m_capa*2+1);
	new (m_data + m_size++) T{std::forward<Args>(args)...};
	return back();
}

template<typename T>
void vector<T>::resize(std::size_t sz, const T& val)
{
	reserve(sz);
	while (sz > size())
		push_back(val);
	while (sz < size())
		pop_back();
}

template<typename T>
template<typename... Args>
T& vector<T>::emplace(iterator pos, Args&&... args)
{
	if (pos == end())
	{
		emplace_back(std::forward<Args>(args)...);
		return back();
	}
	if (is_full())
	{
		std::size_t new_sz = m_capa*2+1;
		T* ptr = std::allocator<T>{}.allocate(new_sz);
		T* src = m_data;
		T* dst = ptr;
		std::size_t idx;
		iterator new_pos;
		for (idx=0; idx<m_size; ++idx)
		{
			if (src == pos)
			{
				new_pos = dst;
				new (dst++) T{std::forward<Args>(args)...};
			}
			new (dst++) T{std::move(*src)};
			src->~T(); ++src;
		}
		if (src == pos)
		{
			new_pos = dst;
			new (dst++) T{std::forward<Args>(args)...};
		}
		assert(src == end());
		std::allocator<T>{}.deallocate(m_data, m_capa);
		m_data = ptr;
		m_size += 1;
		m_capa = new_sz;
		return *new_pos;
	} else {
		T* ptr = end();
		new (ptr) T{std::move(*(ptr-1))};
		--ptr;
		while (true)
		{
			if (ptr == pos) break;
			(*ptr) = std::move(*(ptr-1));
			--ptr;
		}
		(*ptr) = T{std::forward<Args>(args)...};
		m_size += 1;
		return *pos;
	}
}

#include <iostream>

template class vector<int>;

int main()
{
    vector<int> vi = {11,22,33};
    vi.push_back(3); vi.push_back(5); vi.push_back(7);
    vi.push_back(2); vi.push_back(4); vi.push_back(6);
    vi.insert(vi.begin()+3, 11); vi.push_back(0);
    for (auto&& i : vi)
        std::cout << i << " ";
    std::cout << "\n" << vi.capacity() << "\n";
}

