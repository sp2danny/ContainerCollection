
// #pragma once

#include <cstddef>
#include <initializer_list>
#include <utility>
#include <memory>
#include <iterator>
#include <cassert>
#include <cstring>
#include <type_traits>
#include <iterator>

template<typename T, std::size_t N>
class inline_vector
{
	static constexpr bool cne = noexcept(T(std::declval<T>()));
	static constexpr bool triv = false; // std::is_trivially_copyable<T>::value;
public:
	// types
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef T* iterator;
	typedef const T* const_iterator;

	// construction
	inline_vector() noexcept;
	inline_vector(const inline_vector&);
	inline_vector(inline_vector&&) noexcept(cne || triv);
	explicit inline_vector(size_type, const T& = T{});
	template<typename It>
	inline_vector(It, It);
	inline_vector(std::initializer_list<T>);

	// misc
	void swap(inline_vector&);

	// destruction
	void clear();
	~inline_vector();

	// assignment
	inline_vector& operator=(const inline_vector&);
	inline_vector& operator=(inline_vector&&);
	void assign(size_type, const T&);
	template<typename It>
	void assign(It, It);
	void assign(std::initializer_list<T>);

	// size
	void reserve(size_type);
	void resize(size_type, const T& = T{});
	size_type size() const;
	size_type capacity() const;
	[[nodiscard]] bool empty() const;
	constexpr static size_type max_size() noexcept;
	void shrink_to_fit();

	// element access
	T& at(size_type);
	const T& at(size_type) const;
	T& operator[](size_type idx) { return data()[idx]; }
	const T& operator[](size_type idx) const { return data()[idx]; }
	T& front() { return data()[0]; }
	const T& front() const { return data()[0]; }
	T& back();
	const T& back() const;
	T* data();
	const T* data() const;

	// iteration
	iterator begin() { return data(); }
	const_iterator begin() const { return data(); }
	const_iterator cbegin() const { return data(); }
	iterator end() { return data()+size(); }
	const_iterator end() const { return data()+size(); }
	const_iterator cend() const { return data()+size(); }
	iterator nth(size_type idx) { return data()+idx; }
	const_iterator nth(size_type idx) const { return data()+idx; }

	// modifiers
	iterator insert(iterator, const T&);
	iterator insert(iterator, T&&);
	iterator insert(iterator, size_type, const T&);
	template<typename It>
	iterator insert(iterator, It, It);
	iterator insert(iterator, std::initializer_list<T>);

	template<typename... Args>
	iterator emplace(iterator, Args&&...);
	template<typename... Args>
	iterator emplace_back(Args&&...);

	iterator erase(iterator);
	iterator erase(iterator, iterator);

	T& push_back(const T& val) { return *insert(end(), val); }
	T& push_back(T&& val) { return *insert(end(), std::move(val)); }

	void pop_back();

private:

	template<typename It>
	void internal_assign(std::random_access_iterator_tag, It, It);
	template<typename It>
	void internal_assign(std::bidirectional_iterator_tag, It, It);

	void create_gap(size_type, size_type);

	void make_heap();

	struct heap_content
	{
		size_type size, capa;
		T* data;
	};
	struct inline_content
	{
		size_type size, capa;
		T data[N];
	};

	union 
	{
		heap_content hc;
		inline_content ic;
	};

};

// construction

template<typename T, std::size_t N>
inline_vector<T, N>::inline_vector() noexcept
{
	ic.size = 0;
	ic.capa = 0;
}

template<typename T, std::size_t N>
inline_vector<T, N>::inline_vector(const inline_vector& other)
{
	if (other.ic.size <= N)
	{
		ic.size = other.ic.size;
		ic.capa = 0;
	}
	else
	{
		hc.size = other.ic.size;
		hc.capa = ic.size;
		hc.data = std::allocator<T>{}.allocate(ic.size);
	}
	const T* src = other.data();
	T* dst = data();
	if constexpr(triv)
	{
		std::memcpy(dst, src, sizeof(T)*ic.size);
	} else {
		for (size_type i=0; i<ic.size; ++i)
		{
			new (dst+i) T(src[i]);
		}
	}
}

template<typename T, std::size_t N>
inline_vector<T, N>::inline_vector(inline_vector&& other) noexcept(cne || triv)
{
	if (other.ic.size <= N)
	{
		ic.size = other.ic.size;
		ic.capa = 0;
		const T* src = other.data();
		T* dst = data();
		if constexpr(triv)
		{
			memcpy(dst, src, sizeof(T)*ic.size);
		} else {
			for (size_type i=0; i<ic.size; ++i)
			{
				new (dst+i) T(std::move(src[i]));
			}
		}
	}
	else
	{
		hc.size = other.hc.size;
		hc.capa = other.hc.capa;
		hc.data = other.hc.data;
	}
	other.hc.size = 0;
	other.hc.capa = 0;
	other.hc.data = nullptr;
}

template<typename T, std::size_t N>
inline_vector<T, N>::inline_vector(size_type sz, const T& val)
{
	if (sz <= N)
	{
		ic.size = sz;
		ic.capa = 0;
	}
	else
	{
		hc.size = sz;
		hc.capa = sz;
		hc.data = std::allocator<T>{}.allocate(sz);
	}
	T* dst = data();
	for (size_type i=0; i<ic.size; ++i)
	{
		new (dst+i) T(val);
	}
}

template<typename T, std::size_t N>
template<typename It>
inline_vector<T, N>::inline_vector(It b, It e)
	: inline_vector()
{
	typedef typename std::iterator_traits<It>::iterator_category category;
	constexpr bool RAI = std::is_same<category, std::random_access_iterator_tag>::value;
	if constexpr(RAI)
	{
		reserve(e-b);
	}
	while (b!=e)
		push_back(*b++);
}

template<typename T, std::size_t N>
inline_vector<T, N>::inline_vector(std::initializer_list<T> il)
	: inline_vector(il.begin(), il.end())
{
}

template<typename T, std::size_t N>
void inline_vector<T, N>::make_heap()
{
	if (ic.capa) return;
	auto sz = ic.size;
	auto cp = sz+1;
	T* ptr = std::allocator<T>{}.allocate(cp);
	T* src = data();
	if constexpr(triv)
	{
		std::memcpy(ptr, src, sizeof(T)*sz);
	} else {
		for (size_type i=0; i<sz; ++i)
		{
			new (ptr+i) T(std::move(src[i]));
			src[i].~T();
		}
	}
	hc.size = sz;
	hc.capa = cp;
	hc.data = ptr;
}

// misc
template<typename T, std::size_t N>
void inline_vector<T, N>::swap(inline_vector& other)
{
	make_heap();
	other.make_heap();
	using std::swap;
	swap(hc, other.hc);
}

// destruction
template<typename T, std::size_t N>
void inline_vector<T, N>::clear()
{
	T* p = data();
	size_type i, n = size();
	for (i=0; i<n; ++i)
	{
		(p+i)->~T();
	}
	if (ic.capa)
		std::allocator<T>{}.deallocate(hc.data, hc.capa);
	
	hc.size = 0;
	hc.capa = 0;
	hc.data = nullptr;
}

template<typename T, std::size_t N>
inline_vector<T, N>::~inline_vector()
{
	clear();
}

// assignment
template<typename T, std::size_t N>
auto inline_vector<T, N>::operator=(const inline_vector& other) -> inline_vector&
{
	clear();
	if (other.ic.size <= N)
	{
		ic.size = other.ic.size;
		ic.capa = 0;
	}
	else
	{
		hc.size = other.ic.size;
		hc.capa = ic.size;
		hc.data = std::allocator<T>{}.allocate(ic.size);
	}
	const T* src = other.data();
	T* dst = data();
	for (size_type i=0; i<ic.size; ++i)
	{
		new (dst+i) T(src[i]);
	}
	return *this;
}

template<typename T, std::size_t N>
auto inline_vector<T, N>::operator=(inline_vector&& other) -> inline_vector&
{
	swap(other);
	return *this;
}

template<typename T, std::size_t N>
void inline_vector<T, N>::assign(size_type sz, const T& val)
{
	clear();
	if (sz <= N)
	{
		ic.size = sz;
		ic.capa = 0;
	}
	else
	{
		hc.size = sz;
		hc.capa = sz;
		hc.data = std::allocator<T>{}.allocate(ic.size);
	}
	T* dst = data();
	for (size_type i=0; i<sz; ++i)
	{
		new (dst+i) T(val);
	}
}

template<typename T, std::size_t N>
template<typename It>
void inline_vector<T, N>::assign(It b, It e)
{
	clear();

	typedef typename std::iterator_traits<It>::iterator_category category;
	constexpr bool RAI = std::is_same<category, std::random_access_iterator_tag>::value;
	if constexpr(RAI)
	{
		reserve(e-b);
	}
	while (b!=e)
		push_back(*b++);
}

template<typename T, std::size_t N>
void inline_vector<T, N>::assign(std::initializer_list<T> il)
{
	assign(il.begin(), il.end());
}

template<typename T, std::size_t N>
void inline_vector<T, N>::reserve(size_type sz)
{
	if (!sz) return;
	if (ic.capa == 0)
	{
		if (sz <= N) return;
	}
	else
	{
		if (sz <= hc.capa) return;
	}
	if (sz < ic.size) sz = ic.size;
	T* ptr = std::allocator<T>{}.allocate(sz);
	T* src = data();
	auto n = size();
	if constexpr(triv)
	{
		std::memcpy(ptr, src, n*sizeof(T));
	}
	else
	{
		for (size_type i=0; i<n; ++i)
		{
			new (ptr+i) T(std::move(src[i]));
			src[i].~T();
		}
	}
	hc.size = n;
	hc.capa = sz;
	hc.data = ptr;
}

template<typename T, std::size_t N>
void inline_vector<T, N>::resize(size_type sz, const T& val)
{
	auto n = size();
	if (sz < n)
	{
		while (sz < n--) pop_back();
	}
	else if (sz > n)
	{
		reserve(sz);
		while (sz > n++) push_back(val);
	}
}

template<typename T, std::size_t N>
auto inline_vector<T, N>::size() const -> size_type
{
	return ic.size;
}

template<typename T, std::size_t N>
auto inline_vector<T, N>::capacity() const -> size_type
{
	if (ic.capa == 0)
	{
		return N;
	}
	else
	{
		return hc.capa;
	}
}

template<typename T, std::size_t N>
bool inline_vector<T, N>::empty() const
{
	return size() == 0;
}

template<typename T, std::size_t N>
constexpr auto inline_vector<T, N>::max_size() noexcept -> size_type 
{
	return std::allocator<T>::max_size();
}

template<typename T, std::size_t N>
void inline_vector<T, N>::shrink_to_fit()
{
	if (ic.capa == 0) return;
	auto sz = hc.size;
	if (hc.capa == sz) return;

	T* src = data();
	T* dst;
	if (hc.size <= N)
	{
		ic.capa = 0;
		dst = ic.data;
	} else {
		ic.capa = sz;
		dst = hc.data = std::allocator<T>{}.allocate(sz);
	}

	if constexpr(triv)
	{
		memcpy(dst, src, sizeof(T)*sz);
	} else {
		for (auto i = 0ul; i<sz; ++i)
		{
			new (dst+i) T(std::move(src[i]));
			src[i].~T();
		}
	}
	std::allocator<T>{}.deallocate(src, sz);
}

template<typename T, std::size_t N>
T* inline_vector<T, N>::data()
{
	if (ic.capa == 0)
		return ic.data;
	else
		return hc.data;
}

template<typename T, std::size_t N>
const T* inline_vector<T, N>::data() const
{
	if (ic.capa == 0)
		return ic.data;
	else
		return hc.data;
}

// modifiers
template<typename T, std::size_t N>
auto inline_vector<T, N>::insert(iterator where, const T& val) -> iterator
{
	auto cp = capacity();
	auto nn = size();
	size_type ii = where - begin();
	auto ptr = data();
	if (cp >= (nn+1))
	{
		if (nn) for (size_type i = nn; i > ii; --i)
		{
			if (i>=nn)
				new (ptr+i) T(std::move(ptr[i-1]));
			else
				ptr[i] = std::move(ptr[i-1]);
		}
		if (ii>=nn)
			new (ptr+ii) T(val);
		else
			ptr[ii] = val;
		ic.size += 1;
		return ptr + ii;
	}
	else
	{
		cp = cp * 2 + 1;
		auto dst = std::allocator<T>{}.allocate(cp);
		for (size_type i = 0; i < ii; ++i)
		{
			new (dst+i) T(std::move(ptr[i]));
			ptr[i].~T();
		}
		new (dst+ii) T(val);
		for (size_type i = ii; i < nn; ++i)
		{
			new (dst+i+1) T(std::move(ptr[i]));
			ptr[i].~T();
		}
		if (ic.capa != 0)
			std::allocator<T>{}.deallocate(ptr, nn);
		hc.size = nn+1;
		hc.capa = cp;
		hc.data = dst;
		return dst + ii;
	}
}

template<typename T, std::size_t N>
auto inline_vector<T, N>::insert(iterator where, T&& val) -> iterator
{
	auto cp = capacity();
	auto nn = size();
	size_type ii = where - begin();
	auto ptr = data();
	if (cp >= (nn+1))
	{
		if (nn) for (size_type i = nn; i > ii; --i)
		{
			if (i>=nn)
				new (ptr+i) T(std::move(ptr[i-1]));
			else
				ptr[i] = std::move(ptr[i-1]);
		}
		if (ii>=nn)
			new (ptr+ii) T(std::move(val));
		else
			ptr[ii] = std::move(val);
		ic.size += 1;
		return ptr + ii;
	}
	else
	{
		cp = cp * 2 + 1;
		auto dst = std::allocator<T>{}.allocate(cp);
		for (size_type i = 0; i < ii; ++i)
		{
			new (dst+i) T(std::move(ptr[i]));
			ptr[i].~T();
		}
		new (dst+ii) T(std::move(val));
		for (size_type i = ii; i < nn; ++i)
		{
			new (dst+i+1) T(std::move(ptr[i]));
			ptr[i].~T();
		}
		if (ic.capa != 0)
			std::allocator<T>{}.deallocate(ptr, nn);
		hc.size = nn+1;
		hc.capa = cp;
		hc.data = dst;
		return dst + ii;
	}
}

template<typename T, std::size_t N>
auto inline_vector<T, N>::erase(iterator where) -> iterator
{
	auto e = end();
	assert(where >= begin());
	assert(where < e);
	auto itr = where;
	while (true)
	{
		auto nxt = itr + 1;
		if (nxt >= e) break;
		(*itr) = std::move(*nxt);
		itr = nxt;
	}
	itr->~T();
	ic.size -= 1;
	return where;
}

template<typename T, std::size_t N>
auto inline_vector<T, N>::erase(iterator b, iterator e) -> iterator
{
	if (b>=e) return b;
	auto itr = b;
	auto ee = end();
	while (itr != e)
	{
		itr->~T();
		++itr;
		ic.size -= 1;
	}
	itr = b;
	auto src = e;
	while (true)
	{
		if (src == ee) break;
		// move into destroyed
		new (itr) T(std::move(*src));
		src->~T();
		++itr; ++src;
	}
	return b;
}

#ifndef SUPRESS_MAIN
#include <iostream>

//template class inline_vector<int, 10>;

/*
int main()
{
	inline_vector<int,12> vi = {11,22,33};
	vi.push_back(3); vi.push_back(5); vi.push_back(7);
	vi.push_back(2); vi.push_back(4); vi.push_back(6);
	vi.insert(vi.begin()+3, 11); vi.push_back(0);
	vi.shrink_to_fit();
	for (auto&& i : vi)
		std::cout << i << " ";
	std::cout << "\n" << vi.capacity() << "\n";
}
*/

#endif



