
#include <cstddef>
#include <memory>
#include <iterator>
#include <cstdint>
#include <utility>
#include <stdexcept>
#include <type_traits>
#include <cassert>
#include <cstring>
#include <initializer_list>

template<typename T, std::size_t SZ, template<typename...> class Alloc = std::allocator>
class inline_vector
{
	static constexpr bool TPOD = std::is_pod<T>::value;

public:
	typedef Alloc<T> allocator_type;
	typedef T value_type;
	typedef T& reference_type;
	typedef T* pointer_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef T* iterator;
	typedef const T* const_iterator;

public:
	inline_vector() noexcept;
	inline_vector(const inline_vector&);
	inline_vector(inline_vector&&);
	inline_vector& operator=(const inline_vector&);
	inline_vector& operator=(inline_vector&&);
	~inline_vector() { clear(); }
	void swap(inline_vector&);

	template<typename It>
	void assign(It, It);

	void assign(std::size_t, const T&);

	void assign(std::initializer_list<T>);

	inline_vector(std::initializer_list<T>);

	void push_back(const T&);
	void push_back(T&&);

	template<typename... Args>
	T& emplace_back(Args&&...);

	T& operator[](std::size_t idx);
	const T& operator[](std::size_t idx) const;

	T& at(std::size_t idx);
	const T& at(std::size_t idx) const;

	T& back();
	const T& back() const;

	T& front();
	const T& front() const;

	T* data() { return base(); }
	const T* data() const { return base(); }

	void pop_back();
	void clear();

	void resize(std::size_t);
	void resize(std::size_t, const T&);

	void reserve(std::size_t);
	std::size_t capacity();

	template<typename = std::enable_if_t<TPOD>>
	void pod_resize(std::size_t);

	void best_resize(std::size_t n) { best_resize_helper(select_1{}, n); }

	std::size_t size() const;
	static std::size_t max_size();
	bool packed() const { return sz.packed; }
	bool empty() const { return sz.size == 0; }

	T* begin() { return base(); }
	const T* begin() const { return base(); }
	const T* cbegin() const { return base(); }

	T* end() { return base() + size(); }
	const T* end() const { return base() + size(); }
	const T* cend() const { return base() + size(); }

	typedef std::reverse_iterator<iterator> reverse_iterator;

	reverse_iterator rbegin() { return reverse_iterator{end()}; }
	reverse_iterator rend() { return reverse_iterator{begin()}; }

	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	const_reverse_iterator rbegin()  const { return const_reverse_iterator{end()}; }
	const_reverse_iterator rend()    const { return const_reverse_iterator{begin()}; }
	const_reverse_iterator crbegin() const { return const_reverse_iterator{end()}; }
	const_reverse_iterator crend()   const { return const_reverse_iterator{begin()}; }

	allocator_type get_allocator() const { return allocator_type{}; }

	void shrink_to_fit();

	template<typename... Args>
	T& emplace(iterator, Args&&...);

	iterator insert(iterator, const T&);
	iterator insert(iterator, T&&);

	iterator erase(iterator);

	iterator erase(iterator b, iterator e)
	{
		//auto n = e - b;
		while (b != e)
		{
			b = erase(b);
		}
		return b;
	}

	int compare(const inline_vector& other) const;

	bool operator == (const inline_vector& other) const;
	bool operator != (const inline_vector& other) const;
	bool operator  < (const inline_vector& other) const;
	bool operator <= (const inline_vector& other) const;
	bool operator  > (const inline_vector& other) const;
	bool operator >= (const inline_vector& other) const;


private:

	struct select_4 {};
	struct select_3 : select_4 {};
	struct select_2 : select_3 {};
	struct select_1 : select_2 {};
	static constexpr const select_1 choose{};

	template<typename = std::enable_if_t<TPOD>>
	void copychunk_helper(select_1, T* src, T* dst, std::size_t sz);
	void copychunk_helper(select_2, T* src, T* dst, std::size_t sz);
	void copychunk_helper(T* src, T* dst, std::size_t sz);

	template<typename = std::enable_if_t<TPOD>>
	void movechunk_helper(select_1, T* src, T* dst, std::size_t sz);
	void movechunk_helper(select_2, T* src, T* dst, std::size_t sz);
	void movechunk_helper(T* src, T* dst, std::size_t sz);

	template<typename = std::enable_if_t<TPOD>>
	void makechunk_helper(select_1, T* src, T* dst, std::size_t sz);
	void makechunk_helper(select_2, T* src, T* dst, std::size_t sz);
	void makechunk_helper(T* src, T* dst, std::size_t sz);

	void transferchunk_helper(bool move, T* src, T* dst, std::size_t sz);

	template<typename It>
	void assign_helper(It, It, std::random_access_iterator_tag);

	template<typename It>
	void assign_helper(It, It, ...);

	template<typename = std::enable_if_t<TPOD>>
	void best_resize_helper(select_1, std::size_t n)
	{
		pod_resize(n);
	}
	void best_resize_helper(select_2, std::size_t n)
	{
		resize(n);
	}



	void make_unpacked();

	template<typename = std::enable_if_t<TPOD>>
	void swap_helper(select_1, inline_vector&);

	void swap_helper(select_2, inline_vector&);

	T* base() const;

	struct SizeStruct
	{
		std::uint32_t packed : 1;
		std::uint32_t size : 31;
	};
	struct DataStruct
	{
		unsigned char data[sizeof(T) * SZ];
	};
	struct PointerStruct
	{
		T* base;
		std::uint32_t capa;
	};

	SizeStruct sz;
	union PDU {
		DataStruct ds;
		PointerStruct ps;
	} pdu;
};

// ----------------------------------------------------------------------------

template<typename T, std::size_t SZ, template<typename...> class A>
inline_vector<T, SZ, A>::inline_vector() noexcept
{
	sz.packed = true;
	sz.size = 0;

	static_assert(
		((SZ + 1) * sizeof(T)) > sizeof(PointerStruct),
		"inline_vector used with smaller size than supported by overhead"
		);
}

template<typename T, std::size_t SZ, template<typename...> class A>
template<typename>
void inline_vector<T, SZ, A>::copychunk_helper(select_1, T* src, T* dst, std::size_t n)
{
	std::memcpy(dst, src, sizeof(T)*n);
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::copychunk_helper(select_2, T* src, T* dst, std::size_t n)
{
	while (n--)
	{
		(*dst++) = (*src++);
	}
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::copychunk_helper(T* src, T* dst, std::size_t n)
{
	copychunk_helper(choose, src, dst, n);
}

template<typename T, std::size_t SZ, template<typename...> class A>
template<typename>
void inline_vector<T, SZ, A>::movechunk_helper(select_1, T* src, T* dst, std::size_t n)
{
	std::memmove(dst, src, sizeof(T)*n);
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::movechunk_helper(select_2, T* src, T* dst, std::size_t n)
{
	while (n--)
	{
		(*dst++) = std::move(*src);
		src++->~T();
	}
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::movechunk_helper(T* src, T* dst, std::size_t n)
{
	movechunk_helper(choose, src, dst, n);
}

template<typename T, std::size_t SZ, template<typename...> class A>
template<typename>
void inline_vector<T, SZ, A>::makechunk_helper(select_1, T* src, T* dst, std::size_t n)
{
	std::memmove(dst, src, sizeof(T)*n);
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::makechunk_helper(select_2, T* src, T* dst, std::size_t n)
{
	std::size_t  diff = abs(src - dst);
	if (diff > n) // no overlap
	{
		while (n--)
		{
			new (dst++) T(std::move(*src));
			src++->~T();
		}
		return;
	}

	// overlap
	if (dst > src)
	{
		// have to do backwards
		dst += n;
		src += n;
		while (n--)
		{
			--src; --dst;
			new (dst) T(std::move(*src));
			src->~T();
		}
	}
	else {
		// have to do forwards
		while (n--)
		{
			new (dst++) T(std::move(*src));
			src++->~T();
		}
	}
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::makechunk_helper(T* src, T* dst, std::size_t n)
{
	makechunk_helper(choose, src, dst, n);
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::transferchunk_helper(bool move, T* src, T* dst, std::size_t n)
{
	if (move)
		movechunk_helper(src, dst, n);
	else
		copychunk_helper(src, dst, n);
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::make_unpacked()
{
	if (!sz.packed) return;
	sz.packed = false;
	uint32_t capa = 2 * sz.size;
	T* ptr = get_allocator().allocate(capa);
	T* base = (T*)pdu.ds.data;
	makechunk_helper(base, ptr, sz.size);
	pdu.ps.base = ptr;
	pdu.ps.capa = capa;
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::reserve(std::size_t capa)
{
	if (sz.packed)
	{
		if (capa <= SZ) return;
	}
	if (!sz.packed)
	{
		if (capa <= pdu.ps.capa) return;
	}
	T* ptr = get_allocator().allocate(capa);
	T* b = base();
	makechunk_helper(b, ptr, sz.size);
	if (!sz.packed)
		get_allocator().deallocate(b, sz.size);
	sz.packed = false;
	pdu.ps.base = ptr;
	pdu.ps.capa = (uint32_t)capa;
}

template<typename T, std::size_t SZ, template<typename...> class A>
std::size_t inline_vector<T, SZ, A>::capacity()
{
	if (sz.packed)
		return SZ;
	else
		return pdu.ps.capa;
}

template<typename T, std::size_t SZ, template<typename...> class A>
template<typename... Args>
T& inline_vector<T, SZ, A>::emplace_back(Args&&... args)
{
	if (sz.packed)
	{
		if (sz.size < SZ)
		{
			T* target = (T*)(pdu.ds.data + sz.size++ * sizeof(T));
			new (target) T(std::forward<Args>(args)...);
			return *target;
		}
		else
		{
			sz.packed = false;
			uint32_t capa = 2 * ++sz.size;
			T* ptr = get_allocator().allocate(capa);
			T* base = (T*)pdu.ds.data;
			makechunk_helper(base, ptr, SZ);
			new (ptr + SZ) T(std::forward<Args>(args)...);
			pdu.ps.base = ptr;
			pdu.ps.capa = capa;
			return *(ptr + SZ);
		}
	}
	else
	{
		if (sz.size < pdu.ps.capa)
		{
			T* base = pdu.ps.base;
			new (base + sz.size) T(std::forward<Args>(args)...);
			return *(base + sz.size++);
		}
		else
		{
			uint32_t capa = 2 * (sz.size + 1);
			T* ptr = get_allocator().allocate(capa);
			T* base = pdu.ps.base;
			makechunk_helper(base, ptr, sz.size);
			get_allocator().deallocate(base, pdu.ps.capa);
			new (ptr + sz.size) T(std::forward<Args>(args)...);
			pdu.ps.base = ptr;
			pdu.ps.capa = capa;
			return *(base + sz.size++);
		}
	}
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::push_back(const T& t)
{
	emplace_back(t);
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::push_back(T&& t)
{
	emplace_back(std::move(t));
}

template<typename T, std::size_t SZ, template<typename...> class A>
auto inline_vector<T, SZ, A>::base() const -> T*
{
	if (sz.packed)
		return (T*)pdu.ds.data;
	else
		return pdu.ps.base;
}

template<typename T, std::size_t SZ, template<typename...> class A>
auto inline_vector<T, SZ, A>::operator[](std::size_t idx) -> T&
{
	return base()[idx];
}

template<typename T, std::size_t SZ, template<typename...> class A>
auto inline_vector<T, SZ, A>::operator[](std::size_t idx) const -> const T&
{
	return base()[idx];
}

template<typename T, std::size_t SZ, template<typename...> class A>
auto inline_vector<T, SZ, A>::at(std::size_t idx) -> T&
{
	if (idx >= sz.size)
		throw std::out_of_range{"idx"};
	return base()[idx];
}

template<typename T, std::size_t SZ, template<typename...> class A>
auto inline_vector<T, SZ, A>::at(std::size_t idx) const -> const T&
{
	if (idx >= sz.size)
	throw std::out_of_range{"idx"};
return base()[idx];
}

template<typename T, std::size_t SZ, template<typename...> class A>
std::size_t inline_vector<T, SZ, A>::size() const
{
	return sz.size;
}

template<typename T, std::size_t SZ, template<typename...> class A>
std::size_t inline_vector<T, SZ, A>::max_size()
{
	return (1ul << 31) - 1;
}

template<typename T, std::size_t SZ, template<typename...> class A>
auto inline_vector<T, SZ, A>::back() -> T&
{
	assert(!empty());
	return base()[size() - 1];
}

template<typename T, std::size_t SZ, template<typename...> class A>
auto inline_vector<T, SZ, A>::back() const -> const T&
{
	assert(!empty());
return base()[size() - 1];
}

template<typename T, std::size_t SZ, template<typename...> class A>
auto inline_vector<T, SZ, A>::front() -> T&
{
	assert(!empty());
	return base()[0];
}

template<typename T, std::size_t SZ, template<typename...> class A>
auto inline_vector<T, SZ, A>::front() const -> const T&
{
	assert(!empty());
return base()[0];
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::pop_back()
{
	assert(!empty());
	base()[--sz.size].~T();
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::clear()
{
	if (!sz.size) return;

	T* b = base();

	if
#ifdef __cpp_if_constexpr
		constexpr
#endif
		(!TPOD)
		for (auto i = 0ul; i < sz.size; ++i)
			b[i].~T();

	if (!sz.packed)
		get_allocator().deallocate(b, sz.size);

	sz.size = 0;
	sz.packed = 1;
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::resize(std::size_t newsz)
{
	resize(newsz, T{});
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::resize(std::size_t newsz, const T& t)
{
	if (newsz == sz.size)
		return;
	if (newsz < sz.size)
	{
		T* b = base();
		for (auto i = newsz; i < sz.size; ++i)
			b[i].~T();
		sz.size = newsz;
		return;
	}
	if (sz.packed && newsz <= SZ)
	{
		T* b = base();
		for (auto i = sz.size; i < newsz; ++i)
			new (b + i) T(t);
		sz.size = newsz;
		return;
	}

	uint32_t i, capa = (uint32_t)newsz;
	T* ptr = get_allocator().allocate(capa);
	T* b = base();
	makechunk_helper(b, ptr, sz.size);
	if (!sz.packed)
		get_allocator().deallocate(b, sz.size);
	for (i = sz.size; i < newsz; ++i)
	{
		new (ptr + i) T(t);
	}

	sz.packed = false;
	sz.size = newsz;
	pdu.ps.base = ptr;
	pdu.ps.capa = capa;
}

template<typename T, std::size_t SZ, template<typename...> class A>
template<typename>
void inline_vector<T, SZ, A>::pod_resize(std::size_t newsz)
{
	if (newsz == sz.size)
		return;
	if (newsz < sz.size)
	{
		sz.size = newsz;
		return;
	}
	if (sz.packed && newsz <= SZ)
	{
		sz.size = newsz;
		return;
	}

	uint32_t capa = (uint32_t)newsz;
	T* ptr = get_allocator().allocate(capa);
	T* b = base();
	//movechunk_helper
	std::memcpy(ptr, b, sizeof(T)*sz.size);
	if (!sz.packed)
		get_allocator().deallocate(b, pdu.ps.capa);

	sz.packed = false;
	sz.size = newsz;
	pdu.ps.base = ptr;
	pdu.ps.capa = capa;
}

template<typename T, std::size_t SZ, template<typename...> class A>
inline_vector<T, SZ, A>::inline_vector(const inline_vector& other)
	: inline_vector()
{
	for (auto&& x : other)
		push_back(x);
}

template<typename T, std::size_t SZ, template<typename...> class A>
inline_vector<T, SZ, A>::inline_vector(inline_vector&& other)
	: inline_vector()
{
	swap(other);
}

template<typename T, std::size_t SZ, template<typename...> class A>
auto inline_vector<T, SZ, A>::operator=(const inline_vector& other) -> inline_vector&
{
	clear();
	for (auto&& x : other)
		push_back(x);
	return *this;
}

template<typename T, std::size_t SZ, template<typename...> class A>
auto inline_vector<T, SZ, A>::operator=(inline_vector&& other) -> inline_vector&
{
	swap(other);
	return *this;
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::swap(inline_vector& other)
{
	swap_helper(choose, other);
}

template<typename T, std::size_t SZ, template<typename...> class A>
inline_vector<T, SZ, A>::inline_vector(std::initializer_list<T> il)
	: inline_vector()
{
	assign(il.begin(), il.end());
}


template<typename T, std::size_t SZ, template<typename...> class A>
template<typename It>
void inline_vector<T, SZ, A>::assign(It b, It e)
{
	clear();
	assign_helper(b, e, typename std::iterator_traits<It>::iterator_category{});
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::assign(std::size_t n, const T& val)
{
	clear();
	reserve(n);
	while (n--)
		push_back(val);
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::assign(std::initializer_list<T> il)
{
	assign(il.begin(), il.end());
}

template<typename T, std::size_t SZ, template<typename...> class A>
template<typename It>
void inline_vector<T, SZ, A>::assign_helper(It b, It e, std::random_access_iterator_tag)
{
	std::size_t nsz = e - b;
	nsz += sz.size;
	reserve(nsz);
	while (b != e)
	{
		push_back(*b);
		++b;
		--nsz;
	}
	assert(!nsz);
}

template<typename T, std::size_t SZ, template<typename...> class A>
template<typename It>
void inline_vector<T, SZ, A>::assign_helper(It b, It e, ...)
{
	while (b != e)
	{
		push_back(*b);
		++b;
	}
}

template<typename T, std::size_t SZ, template<typename...> class A>
template<typename>
void inline_vector<T, SZ, A>::swap_helper(select_1, inline_vector& other)
{
	// is_pod == true
	{
		PDU tmp;
		auto n = sizeof(PDU);
		std::memcpy(&tmp, &pdu, n);
		std::memcpy(&pdu, &other.pdu, n);
		std::memcpy(&other.pdu, &tmp, n);
	}
	using std::swap;
	swap(sz, other.sz);
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::swap_helper(select_2, inline_vector& other)
{
	using std::swap;
	if (packed() && other.packed())
	{
		auto asz = size();
		auto bsz = other.size();
		auto abase = base();
		auto bbase = other.base();
		for (auto i = 0ul; i < SZ; ++i)
		{
			if (i < asz && i < bsz)
			{
				swap(abase[i], bbase[i]);
			}
			else if (i < asz)
			{
				new (bbase + i) T(std::move(abase[i]));
				abase[i].~T();
			}
			else if (i < bsz)
			{
				new (abase + i) T(std::move(bbase[i]));
				bbase[i].~T();
			}
			else
			{
				break;
			}
		}
		swap(sz, other.sz);
		return;
	}

	make_unpacked();
	other.make_unpacked();

	swap(sz, other.sz);
	swap(pdu.ps, other.pdu.ps);
}

template<typename T, std::size_t SZ, template<typename...> class A>
void inline_vector<T, SZ, A>::shrink_to_fit()
{
	auto sz_ = size();
	if (sz_ <= SZ)
	{
		if (packed()) return;
		T* b = base();
		T* to = (T*)pdu.ds.data;
		auto capa = pdu.ps.capa;
		makechunk_helper(b, to, sz_);
		sz.packed = true;
		get_allocator().deallocate(b, capa);
	}
	else
	{
		auto capa = pdu.ps.capa;
		if (capa == sz_) return;
		T* ptr = get_allocator().allocate(sz_);
		T* b = base();
		makechunk_helper(b, ptr, sz_);
		get_allocator().deallocate(b, capa);
		pdu.ps.capa = (uint32_t)sz_;
		pdu.ps.base = ptr;
	}
}

template<typename T, std::size_t SZ, template<typename...> class A>
template<typename... Args>
T& inline_vector<T, SZ, A>::emplace(iterator where, Args&&... args)
{
	auto pos = std::distance(begin(), where);
	auto new_sz = size() + 1;
	if (packed() && new_sz > SZ)
		make_unpacked();
	if (new_sz > capacity())
		reserve(new_sz);
	T* b = base();
	makechunk_helper(b + pos, b + pos + 1, size() - pos);
	new (b + pos) T(std::forward<Args>(args)...);
	++sz.size;
	return *(b + pos);
}

template<typename T, std::size_t SZ, template<typename...> class A>
auto inline_vector<T, SZ, A>::insert(iterator where, const T& val) -> iterator
{
	auto pos = std::distance(begin(), where);
	emplace(where, val);
	return base() + pos;
}

template<typename T, std::size_t SZ, template<typename...> class A>
auto inline_vector<T, SZ, A>::insert(iterator where, T&& val) -> iterator
{
	auto pos = std::distance(begin(), where);
	emplace(where, std::move(val));
	return base() + pos;
}

template<typename T, std::size_t SZ, template<typename...> class A>
auto inline_vector<T, SZ, A>::erase(iterator what) -> iterator
{
	auto pos = std::distance(begin(), what);
	T* b = base();
	b[pos].~T();
	makechunk_helper(b + pos + 1, b + pos, size() - pos - 1);
	--sz.size;
	return base() + pos;
}

// ----------------------------------------------------------------------------


template<typename T, std::size_t SZ, template<typename...> class A>
int inline_vector<T, SZ, A>::compare(const inline_vector& other) const
{
	auto me_iter = begin();
	auto ot_iter = other.begin();
	while (true)
	{
		bool me_ate = (me_iter == end());
		bool ot_ate = (ot_iter == other.end());
		if (me_ate && ot_ate)
			return 0;
		if (me_ate) return -1;
		if (ot_ate) return +1;
		if ((*me_iter) < (*ot_iter)) return -1;
		if ((*ot_iter) < (*me_iter)) return +1;
		++me_iter; ++ot_iter;
	}
}

template<typename T, std::size_t SZ, template<typename...> class A>
bool inline_vector<T, SZ, A>::operator == (const inline_vector& other) const
{
	if (size() != other.size())
		return false;
	else
		return compare(other) == 0;
}

template<typename T, std::size_t SZ, template<typename...> class A>
bool inline_vector<T, SZ, A>::operator != (const inline_vector& other) const
{
	if (size() != other.size())
		return true;
	else
		return compare(other) != 0;
}

template<typename T, std::size_t SZ, template<typename...> class A>
bool inline_vector<T, SZ, A>::operator  < (const inline_vector& other) const
{
	return compare(other) < 0;
}

template<typename T, std::size_t SZ, template<typename...> class A>
bool inline_vector<T, SZ, A>::operator <= (const inline_vector& other) const
{
	return compare(other) <= 0;
}

template<typename T, std::size_t SZ, template<typename...> class A>
bool inline_vector<T, SZ, A>::operator  > (const inline_vector& other) const
{
	return compare(other) > 0;
}

template<typename T, std::size_t SZ, template<typename...> class A>
bool inline_vector<T, SZ, A>::operator >= (const inline_vector& other) const
{
	return compare(other) >= 0;
}











