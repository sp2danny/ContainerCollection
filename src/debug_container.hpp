
#pragma once

#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>

template<typename T>
struct wierd_alloc
{

	template<class U>
	wierd_alloc(const wierd_alloc<U>&)
	{
	}
	wierd_alloc() = default;

	template<typename U>
	struct rebind
	{
		typedef wierd_alloc<U> other;
	};

	typedef T               value_type;
	typedef std::false_type propagate_on_container_move_assignment;
	typedef std::true_type  is_always_equal;

	T* allocate(std::size_t n)
	{
		std::cerr << typeid(T).name() << " : " << sizeof(T) << "\n";
		return (T*)malloc(n * sizeof(T));
	}

	void deallocate(T* ptr, std::size_t n) { free(ptr); }
};

template<typename T, typename Alloc = std::allocator<T>>
class debug_container
{
public:
	typedef T              value_type;
	typedef T&             reference;
	typedef const T&       const_reference;
	typedef T*             pointer;
	typedef const T*       const_pointer;
	typedef std::size_t    size_type;
	typedef std::ptrdiff_t difference_type;
	typedef Alloc          allocator_type;
	struct iterator;
	struct const_iterator;

private:
	struct core_iterator;
	struct Impl;

	typedef std::list<T, Alloc>              item_list;
	typedef std::list<core_iterator*, Alloc> iter_list;

	typedef typename Alloc::template rebind<Impl>::other ImplAlloc;

	struct core_iterator
	{
		core_iterator();
		core_iterator(const core_iterator&);
		core_iterator(core_iterator&&);
		core_iterator& operator=(const core_iterator&);
		core_iterator& operator=(core_iterator&&);
		~core_iterator();
		T*   item() const;
		void next();
		void prev();
		void move(std::ptrdiff_t off);

	private:
		Impl*                        cb;
		typename item_list::iterator iter;
		bool                         valid;
		core_iterator(debug_container* owner, typename item_list::iterator iter, bool ok = true)
			: cb(owner->impl), iter(iter), valid(ok)
		{
		}
		friend class debug_container;
	};

	core_iterator ci_begin() const;
	core_iterator ci_end() const;

	template<typename... Args>
	void ci_insert(core_iterator&, Args&&...);

	void ci_erase(core_iterator&, bool = true);

	struct Impl
	{
		item_list        items;
		iter_list        iters;
		unsigned short   usage;
		unsigned short   state;
		debug_container* owner;
		static void      test_self_delete(Impl*);
		void             addi(core_iterator*);
		bool             remi(core_iterator*);
	};

	Impl* impl;

	Impl* new_block();

public:
	debug_container();
	debug_container(const debug_container&);
	debug_container(debug_container&&);
	~debug_container();
	debug_container& operator=(const debug_container&);
	debug_container& operator=(debug_container&&) noexcept;

	void        swap(debug_container&) noexcept;
	void        clear();
	std::size_t size() const;
	bool        empty() const;

	template<typename It>
	auto assign(It b, It e) -> decltype(++b, *b, b != e, void());

	void assign(std::size_t, const T&);

	void push_back(const T&);
	void push_back(T&&);

	void push_front(const T&);
	void push_front(T&&);

	T&       back();
	T&       front();
	const T& back() const;
	const T& front() const;

	struct iterator : std::iterator<std::random_access_iterator_tag, T>, core_iterator
	{
		iterator() = default;
		iterator& operator++()
		{
			next();
			return *this;
		}
		iterator& operator--()
		{
			prev();
			return *this;
		}
		iterator operator++(int)
		{
			auto tmp = *this;
			next();
			return tmp;
		}
		iterator operator--(int)
		{
			auto tmp = *this;
			prev();
			return tmp;
		}
		T&   operator*() { return *item(); }
		T*   operator->() { return item(); }
		bool operator==(const iterator& other) const { return iter == other.iter; }
		bool operator!=(const iterator& other) const { return iter != other.iter; }

	private:
		iterator(debug_container* owner, typename item_list::iterator iter) : core_iterator(owner, iter) {}
		iterator(const core_iterator& ci) : core_iterator(ci) {}
		friend class debug_container;
	};

	struct const_iterator : std::iterator<std::random_access_iterator_tag, const T>, core_iterator
	{
		const_iterator() = default;
		const_iterator& operator++()
		{
			next();
			return *this;
		}
		const_iterator& operator--()
		{
			prev();
			return *this;
		}
		const_iterator operator++(int)
		{
			auto tmp = *this;
			next();
			return tmp;
		}
		const_iterator operator--(int)
		{
			auto tmp = *this;
			prev();
			return tmp;
		}
		const T& operator*() { return *item(); }
		const T* operator->() { return item(); }
		bool     operator==(const const_iterator& other) const { return iter == other.iter; }
		bool     operator!=(const const_iterator& other) const { return iter != other.iter; }

	private:
		const_iterator(debug_container* owner, typename item_list::iterator iter) : core_iterator(owner, iter) {}
		friend class debug_container;
	};

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;

	iterator erase(iterator);
	iterator insert(iterator, T&);
	iterator insert(iterator, T&&);

	template<typename... Args>
	iterator emplace(iterator, Args&&...);
	template<typename... Args>
	iterator emplace_back(Args&&... args);
	template<typename... Args>
	iterator emplace_front(Args&&... args);
};

template<typename T, typename A>
auto debug_container<T, A>::new_block() -> Impl*
{
	Impl* pimpl = ImplAlloc{}.allocate(1);
	new (pimpl) Impl;
	pimpl->usage = 0;
	pimpl->state = 0;
	pimpl->owner = this;
	return pimpl;
}

template<typename T, typename A>
void debug_container<T, A>::Impl::test_self_delete(Impl* pimpl)
{
	if (pimpl->items.empty() && pimpl->iters.empty() && !pimpl->owner)
	{
		pimpl->~Impl();
		ImplAlloc{}.deallocate(pimpl, 1);
	}
}

template<typename T, typename A>
void debug_container<T, A>::Impl::addi(core_iterator* ci)
{
	iters.push_back(ci);
}

template<typename T, typename A>
bool debug_container<T, A>::Impl::remi(core_iterator* ci)
{
	auto b   = iters.begin();
	auto e   = iters.end();
	auto pos = std::find(b, e, ci);
	if (pos == e)
	{
		return false;
	}
	else
	{
		iters.erase(pos);
		return true;
	}
}

template<typename T, typename A>
debug_container<T, A>::core_iterator::core_iterator() : cb(nullptr), valid(false)
{
}

template<typename T, typename A>
debug_container<T, A>::core_iterator::core_iterator(const core_iterator& other)
{
	valid = other.valid;
	cb    = other.cb;
	iter  = other.iter;
	if (cb)
		cb->addi(this);
}

template<typename T, typename A>
debug_container<T, A>::core_iterator::core_iterator(core_iterator&& other)
{
	valid = other.valid;
	cb    = other.cb;
	iter  = other.iter;
	if (cb)
	{
		if (!cb->remi(&other))
			throw "internal library error";
		cb->addi(this);
		other.valid = false;
		other.cb    = nullptr;
	}
}

template<typename T, typename A>
auto debug_container<T, A>::core_iterator::operator=(const core_iterator& other) -> core_iterator&
{
	if (cb)
		cb->remi(this);
	valid = other.valid;
	cb    = other.cb;
	iter  = other.iter;
	if (cb)
		cb->addi(this);
	return *this;
}

template<typename T, typename A>
auto debug_container<T, A>::core_iterator::operator=(core_iterator&& other) -> core_iterator&
{
	auto old_cb = cb;
	if (cb)
		cb->remi(this);
	valid = other.valid;
	cb    = other.cb;
	iter  = other.iter;
	if (cb)
	{
		cb->remi(&other);
		cb->addi(this);
		other.valid = false;
		other.cb    = nullptr;
	}

	if (old_cb && !cb)
	{
		Impl::test_self_delete(old_cb);
	}

	return *this;
}

template<typename T, typename A>
debug_container<T, A>::core_iterator::~core_iterator()
{
	if (cb)
	{
		cb->remi(this);
		Impl::test_self_delete(cb);
	}
}

template<typename T, typename A>
void debug_container<T, A>::ci_erase(core_iterator& ci, bool mustexist)
{
	assert(impl == ci.cb);
	assert(ci.valid);
	bool ok = impl->remi(&ci);
	assert(ok || !mustexist);
	ci.valid = false;
	ci.cb    = nullptr;
	impl->items.erase(ci.iter);
}

template<typename T, typename A>
auto debug_container<T, A>::ci_begin() const -> core_iterator
{
	assert(impl);
	return core_iterator((debug_container*)this, impl->items.begin());
}

template<typename T, typename A>
auto debug_container<T, A>::ci_end() const -> core_iterator
{
	assert(impl);
	return core_iterator((debug_container*)this, impl->items.end());
}

template<typename T, typename A>
void debug_container<T, A>::clear()
{
	for (auto&& x : impl->iters)
		x->valid = false;

	while (!empty())
		ci_erase(ci_begin(), false);
}

template<typename T, typename A>
bool debug_container<T, A>::empty() const
{
	assert(impl);
	return impl->items.empty();
}

template<typename T, typename A>
T* debug_container<T, A>::core_iterator::item() const
{
	assert(valid);
	return &*iter;
}

template<typename T, typename A>
void debug_container<T, A>::core_iterator::next()
{
	assert(valid);
	++iter;
}

template<typename T, typename A>
void debug_container<T, A>::core_iterator::prev()
{
	assert(valid);
	--iter;
}

template<typename T, typename A>
void debug_container<T, A>::core_iterator::move(std::ptrdiff_t off)
{
	std::advance(iter, off);
}

template<typename T, typename A>
template<typename... Args>
void debug_container<T, A>::ci_insert(core_iterator& ci, Args&&... args)
{
	auto iter = ci.iter;
	assert(ci.cb == impl);
	assert(ci.valid);
	assert(impl);
	impl->items.emplace(iter, std::forward<Args>(args)...);
}

template<typename T, typename A>
void debug_container<T, A>::push_back(const T& item)
{
	ci_insert(ci_end(), item);
}

template<typename T, typename A>
void debug_container<T, A>::push_back(T&& item)
{
	ci_insert(ci_end(), std::move(item));
}

template<typename T, typename A>
auto debug_container<T, A>::begin() -> iterator
{
	return {ci_begin()};
}

template<typename T, typename A>
auto debug_container<T, A>::end() -> iterator
{
	return {ci_end()};
}

template<typename T, typename A>
debug_container<T, A>::debug_container()
{
	impl = new_block();
}

template<typename T, typename A>
debug_container<T, A>::debug_container(const debug_container& other)
{
	impl = new_block();
	clear();
	assign(other.begin(), other.end());
}

template<typename T, typename A>
debug_container<T, A>::debug_container(debug_container&& other) : debug_container()
{
	swap(other);
}

template<typename T, typename A>
debug_container<T, A>::~debug_container()
{
	clear();
	impl->owner = nullptr;
	Impl::test_self_delete(impl);
}

template<typename T, typename A>
auto debug_container<T, A>::operator=(const debug_container&) -> debug_container&
{
	return *this;
}

template<typename T, typename A>
auto debug_container<T, A>::operator=(debug_container&& other) noexcept -> debug_container&
{
	swap(other);
	return *this;
}

template<typename T, typename A>
void debug_container<T, A>::swap(debug_container& other) noexcept
{
	assert(impl && other.impl) using std::swap;
	swap(impl->items, other.impl->items);
	swap(impl->iters, other.impl->iters);
	swap(impl->usage, other.impl->usage);
	swap(impl->state, other.impl->state);

	for (auto&& x : impl->iters)
	{
		if (x.cb == other.impl)
			x.cb = impl;
	}

	for (auto&& x : other.impl->iters)
	{
		if (x.cb == impl)
			x.cb = other.impl;
	}
}

void foo()
{
	std::list<int, wierd_alloc<int>> sliwat;

	sliwat.push_back(3);
}
