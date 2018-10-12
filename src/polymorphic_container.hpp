
#include <cstddef>
#include <type_traits>
#include <iterator>
#include <vector>
#include <cassert>
#include <typeinfo>
#include <functional>
#include <exception>
#include <stdexcept>

// ----------------------------------------------------------------------------------------------

namespace ext {

template< typename, template<typename...> class, template<typename...> class >
class polymorphic_container;

namespace detail
{

    template<typename T>
    using clean = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

	template<typename,typename,typename>
	struct iterator;

	template<typename T,typename underlying_iterator>
	struct iterator_base
	{
		iterator_base() = default;
		iterator_base& operator++() { ++iter; return *this; }
		iterator_base& operator--() { --iter; return *this; }
		iterator_base operator++(int) { iterator_base tmp = *this; ++iter; return tmp; }
		iterator_base operator--(int) { iterator_base tmp = *this; --iter; return tmp; }
		bool operator==(const iterator_base& other) const { return iter==other.iter; }
		bool operator!=(const iterator_base& other) const { return iter!=other.iter; }
		T& operator*() const { return *ptr(); }
		T* operator->() const { return ptr(); };

	protected:
		underlying_iterator iter;	
		iterator_base(underlying_iterator i) : iter(i) {}
	private:
		T* ptr() const { return (T*)iter->value; }

		template<typename,typename,typename>
		friend struct iterator;

	};

	template<typename T,typename underlying_iterator,typename category>
	struct iterator : std::iterator< category, T > , iterator_base<T,underlying_iterator>
	{
		iterator() = default;
		iterator( const iterator_base<T,underlying_iterator>& ib )
			: iterator_base<T,underlying_iterator>(ib.iter) {}

		using iterator_base<T,underlying_iterator>::iterator_base;

		template< typename, template<typename...> class, template<typename...> class >
		friend class ext::polymorphic_container;

		// if the underlying iterator can convert, support it
		template<typename T2,typename UI2,typename C2>
		operator iterator<T2,UI2,C2>() { return {this->iter}; }

		template<typename U,typename UI2,typename C2>
		friend struct iterator;
	};

	template<typename T,typename underlying_iterator>
	struct iterator< T, underlying_iterator, std::random_access_iterator_tag > : std::iterator< std::random_access_iterator_tag, T > , iterator_base<T,underlying_iterator>
	{
		iterator() = default;
		iterator( const iterator_base<T,underlying_iterator>& ib )
			: iterator_base<T,underlying_iterator>(ib.iter) {}

		using iterator_base<T,underlying_iterator>::iterator_base;

		iterator& operator+=(std::ptrdiff_t diff) { this->iter+=diff; return *this; }
		iterator& operator-=(std::ptrdiff_t diff) { this->iter-=diff; return *this; }
		iterator operator+(std::ptrdiff_t diff) { auto tmp = *this; tmp+=diff; return tmp; }
		iterator operator-(std::ptrdiff_t diff) { auto tmp = *this; tmp-=diff; return tmp; }

		bool operator <  (const iterator& other) const { return this->iter <  other.iter; }
		bool operator <= (const iterator& other) const { return this->iter <= other.iter; }
		bool operator >  (const iterator& other) const { return this->iter >  other.iter; }
		bool operator >= (const iterator& other) const { return this->iter >= other.iter; }

		T& operator[](std::size_t idx) const { return *this->iter[idx].value; }

		std::ptrdiff_t operator-(const iterator& other) const { return this->iter - other.iter; }

		template< typename, template<typename...> class, template<typename...> class >
		friend class ext::polymorphic_container;

		// if the underlying iterator can convert, support it
		template<typename T2,typename UI2,typename C2>
		operator iterator<T2,UI2,C2>() { return {this->iter}; }

		template<typename U,typename UI2,typename C2>
		friend struct iterator;
	};

}

// ----------------------------------------------------------------------------------------------

template<
	typename T,
	template<typename...> class Underlying = std::vector,
	template<typename...> class Allocator = std::allocator
>
class polymorphic_container
{
private:
	typedef std::function<void(T*)> deleter_t;
	struct Item
	{
		T* value = nullptr;
		deleter_t deleter = nullptr;

		Item() = default;
		Item( const Item& other ) = delete;
		Item( Item&& other ) noexcept : Item() { swap(other); }
		Item& operator=( const Item& other ) = delete;
		Item& operator=( Item&& other ) noexcept { clear(); swap(other); return *this; }
		~Item() { clear(); }
		T& operator*() const;
		void swap(Item&) noexcept;
		void clear();

		bool operator <  (const Item& i) { return (*value) <  *i; }
		bool operator <= (const Item& i) { return (*value) <= *i; }
		bool operator >  (const Item& i) { return (*value) >  *i; }
		bool operator >= (const Item& i) { return (*value) >= *i; }
		bool operator == (const Item& i) { return (*value) == *i; }
		bool operator != (const Item& i) { return (*value) != *i; }

	};

	template<typename U>
	deleter_t make_deleter();

	template<typename U>
	static Allocator<U>& allocator()
	{
		static Allocator<U> a;
		return a;
	}

	typedef Underlying<Item,Allocator<Item>> underlying_container;
	typedef typename underlying_container::iterator underlying_iterator;
	typedef typename underlying_container::const_iterator underlying_const_iterator;
	typedef typename std::iterator_traits< underlying_iterator >::iterator_category underlying_iterator_category;

	template<typename U>
	struct sub_or_same
	{
		using CT = detail::clean<T>;
		using CU = detail::clean<U>;
		static const bool value =
			std::is_base_of < CT, CU >::value ||
			std::is_same    < CT, CU >::value ;
	};

	static const bool underlying_swap_noexcept =
		noexcept( std::declval<underlying_container&>() .swap( std::declval<underlying_container&>() ) );
public:

	typedef std::size_t size_type;
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;

	polymorphic_container() = default;
	polymorphic_container(const polymorphic_container&) = default;
	polymorphic_container(polymorphic_container&&) = default;

	polymorphic_container& operator=(const polymorphic_container&) = default;
	polymorphic_container& operator=(polymorphic_container&&) = default;

	~polymorphic_container() = default;

	void assign( const polymorphic_container& other ) { data.assign(other.data); }
	void assign( polymorphic_container&& other ) noexcept(underlying_swap_noexcept) { data.swap(other.data); }

	void clear() { data.clear(); }
	void swap( polymorphic_container&  other ) noexcept(underlying_swap_noexcept) { data.swap(other.data); }
	void swap( polymorphic_container&& other ) noexcept(underlying_swap_noexcept) { data.swap(other.data); }

	size_type size() { return data.size(); }
	bool empty() { return data.empty(); }

	typedef detail::iterator<T,underlying_iterator,underlying_iterator_category> iterator;
	typedef detail::iterator<const T,underlying_const_iterator,underlying_iterator_category> const_iterator;

	iterator begin() { return { data.begin() }; }
	iterator end()   { return { data.end()   }; }
	const_iterator begin()  const { return { data.begin() }; }
	const_iterator end()    const { return { data.end()   }; }
	const_iterator cbegin() const { return { data.begin() }; }
	const_iterator cend()   const { return { data.end()   }; }

	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	reverse_iterator rbegin() { return reverse_iterator{ end()   }; }
	reverse_iterator rend()   { return reverse_iterator{ begin() }; }
	const_reverse_iterator rbegin()  const { return const_reverse_iterator{ end()   }; }
	const_reverse_iterator rend()    const { return const_reverse_iterator{ begin() }; }
	const_reverse_iterator crbegin() const { return const_reverse_iterator{ end()   }; }
	const_reverse_iterator crend()   const { return const_reverse_iterator{ begin() }; }

private:
	template<typename U>
	iterator explicit_insert( const_iterator, const U& );

	template<typename U>
	iterator explicit_insert( const_iterator, U&& );

public:
	template<typename U, typename = std::enable_if_t<sub_or_same<U>::value>>
	iterator insert( const_iterator i, U&& u ) // creates copy
	{
		return explicit_insert< detail::clean<U> > ( i, std::forward<U>(u) );
	}
	
	iterator insert( const_iterator, T* ); // takes ownership, uses delete for disposal
	
	template<typename Deleter>
	iterator insert( const_iterator, T*, const Deleter& ); // takes ownership, uses custom deleter

	template<typename U=T, typename... Args, typename = std::enable_if_t<sub_or_same<U>::value>>
	iterator emplace( const_iterator, Args&&... );

	iterator erase( const_iterator i ) { return { data.erase(i.iter) }; }

	template<typename... Args>
	void push_back( Args&&... args )
	{ insert( end(), std::forward<Args>(args)... ); }

	template<typename U=T, typename... Args, typename = std::enable_if_t<sub_or_same<U>::value>>
	void emplace_back( Args&&... args )
	{ emplace<U>( end(), std::forward<Args>(args)... ); }

	void pop_back() { erase(end()); }

	T& back() { return *data.back(); }
	const T& back() const { return *data.back(); }

	T& front() { return *data.front(); }
	const T& front() const { return *data.front(); }

	T& operator[](std::size_t idx)
	{
		static_assert(
			std::is_same< underlying_iterator_category, std::random_access_iterator_tag >::value,
			"underlying container does not support random access"
		);
		return begin()[idx];
	}
	const T& operator[](std::size_t idx) const
	{
		static_assert(
			std::is_same< underlying_iterator_category, std::random_access_iterator_tag >::value,
			"underlying container does not support random access"
		);
		return begin()[idx];
	}
	
	T& at(std::size_t idx)
	{
		static_assert(
			std::is_same< underlying_iterator_category, std::random_access_iterator_tag >::value,
			"underlying container does not support random access"
		);
		if(idx>=size()) throw std::out_of_range{"index"};
		return begin()[idx];
	}
	const T& at(std::size_t idx) const
	{
		static_assert(
			std::is_same< underlying_iterator_category, std::random_access_iterator_tag >::value,
			"underlying container does not support random access"
		);
		if(idx>=size()) throw std::out_of_range{"index"};
		return begin()[idx];
	}


private:

	underlying_container data;

};

// ----------------------------------------------------------------------------------------------

template< typename T, template<typename...> class U, template<typename...> class A >
void swap(polymorphic_container<T,U,A>& lhs, polymorphic_container<T, U, A>& rhs)
{
	lhs.swap(rhs);
}

template< typename T, template<typename...> class U, template<typename...> class A >
bool operator == (polymorphic_container<T, U, A>& lhs, polymorphic_container<T, U, A>& rhs)
{
	auto li = lhs.begin();
	auto ri = rhs.begin();
	while(true)
	{
		bool le = li==lhs.end();
		bool re = ri==rhs.end();
		if(le&&re) return true;
		if(le||re) return false;
		if( *li != *ri ) return false;
		++li; ++ri;
	}
}

template< typename T, template<typename...> class U, template<typename...> class A >
bool operator != (polymorphic_container<T, U, A>& lhs, polymorphic_container<T, U, A>& rhs)
{
	return ! (lhs==rhs);
}

template< typename T, template<typename...> class U, template<typename...> class A >
bool operator < (polymorphic_container<T, U, A>& lhs, polymorphic_container<T, U, A>& rhs)
{
	auto li = lhs.begin();
	auto ri = rhs.begin();
	while(true)
	{
		bool le = li==lhs.end();
		bool re = ri==rhs.end();
		if(le&&re) return false;
		if(le||re) return le;
		if( *li < *ri ) return true;
		if( *li > *ri ) return false;
		++li; ++ri;
	}
}

template< typename T, template<typename...> class U, template<typename...> class A >
bool operator <= (polymorphic_container<T, U, A>& lhs, polymorphic_container<T, U, A>& rhs)
{
	auto li = lhs.begin();
	auto ri = rhs.begin();
	while(true)
	{
		bool le = li==lhs.end();
		bool re = ri==rhs.end();
		if(le&&re) return true;
		if(le||re) return le;
		if( *li < *ri ) return true;
		if( *li > *ri ) return false;
		++li; ++ri;
	}
}

template< typename T, template<typename...> class U, template<typename...> class A >
bool operator > (polymorphic_container<T, U, A>& lhs, polymorphic_container<T, U, A>& rhs)
{
	return ! (lhs<=rhs);
}

template< typename T, template<typename...> class U, template<typename...> class A >
bool operator >= (polymorphic_container<T, U, A>& lhs, polymorphic_container<T, U, A>& rhs)
{
	return ! (lhs<rhs);
}

// ----------------------------------------------------------------------------------------------

template< typename T, template<typename...> class Underlying, template<typename...> class Allocator >
void polymorphic_container<T,Underlying,Allocator>::Item::swap(Item& other) noexcept
{
	using std::swap;
	swap(value, other.value);
	swap(deleter, other.deleter);
}

template< typename T, template<typename...> class Underlying, template<typename...> class Allocator >
template<typename U>
auto polymorphic_container<T,Underlying,Allocator>::make_deleter() -> deleter_t
{
	auto deleter = [](T* t) -> void
	{
		U* item = (U*)t;
		allocator<U>().deallocate(item,1);
	};
	return deleter;
}

template< typename T, template<typename...> class Underlying, template<typename...> class Allocator >
void polymorphic_container<T,Underlying,Allocator>::Item::clear()
{
	if(value)
	{
		assert(deleter);
		deleter(value);
	}
	value = nullptr;
	deleter = nullptr;
}

template< typename T, template<typename...> class Underlying, template<typename...> class Allocator >
auto polymorphic_container<T,Underlying,Allocator>::Item::operator*() const -> T&
{
	assert(value);
	return *value;
}

template< typename T, template<typename...> class Underlying, template<typename...> class Allocator >
template<typename U>
auto polymorphic_container<T,Underlying,Allocator>::explicit_insert( const_iterator i, const U& u ) -> iterator
{
	assert( (typeid(u) == typeid(U)) && "allocator::deallocate need to know the exact type" );
	U* space = allocator<U>().allocate(1);
	U* value = new (space) U(u);
	auto iter = data.emplace(i.iter);
	iter->value = value;
	iter->deleter = make_deleter<U>();
	return {iter};
}

template< typename T, template<typename...> class Underlying, template<typename...> class Allocator >
template<typename U>
auto polymorphic_container<T,Underlying,Allocator>::explicit_insert( const_iterator i, U&& u ) -> iterator
{
	assert( (typeid(u) == typeid(U)) && "allocator::deallocate need to know the exact type" );
	U* space = allocator<U>().allocate(1);
	U* value = new (space) U(std::move(u));
	auto iter = data.emplace(i.iter);
	iter->value = value;
	iter->deleter = make_deleter<U>();
	return {iter};
}

template< typename T, template<typename...> class Underlying, template<typename...> class Allocator >
auto polymorphic_container<T,Underlying,Allocator>::insert( const_iterator i, T* t ) -> iterator
{
	auto iter = data.emplace(i.iter);
	iter->value = t;
	iter->deleter = [](T* t) -> void { delete t; };
	return {iter};
}

template< typename T, template<typename...> class Underlying, template<typename...> class Allocator >
template<typename Deleter>
auto polymorphic_container<T,Underlying,Allocator>::insert( const_iterator i, T* t, const Deleter& d ) -> iterator
{
	auto iter = data.emplace(i.iter);
	iter->value = t;
	iter->deleter = [d](T* t) -> void { d(t); };
	return {iter};
}

template< typename T, template<typename...> class Underlying, template<typename...> class Allocator >
template<typename U, typename... Args, typename >
auto polymorphic_container<T,Underlying,Allocator>::emplace( const_iterator i, Args&&... args ) -> iterator
{
	U* space = allocator<U>().allocate(1);
	U* value = new (space) U( std::forward<Args>(args)... );
	auto iter = data.emplace(i.iter);
	iter->value = value;
	iter->deleter = make_deleter<U>();
	return {iter};
}

}













#include <iostream>
#include <list>
#include <cstdlib>

using namespace ext;

// ----------------------------------------------------------------------------------------------

int tot_a = 0;
int tot_d = 0;

template<typename T,int Blocksize>
struct MyAlloc
{
    typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;

	MyAlloc()
	{
		static bool first = true;
		if (first)
		{
			std::cout << "allocator instatiated on a " << typeid(T).name() << std::endl;
			first = false;
		} else {
			std::cout << "caller assumes stateless, " << typeid(T).name() << std::endl;
		}
	}

	// A converting copy constructor: needed for MSVC
	template<class U> MyAlloc(const MyAlloc<U,Blocksize>&) noexcept
		: MyAlloc()
	{
	}

	template<typename U> struct rebind
	{
		typedef MyAlloc<U,Blocksize> other;
	};

	static T* allocate(std::size_t n)
	{
		auto a = sizeof(T)*n;
		tot_a += a;
		//std::cout << "allocated " << a << " bytes\n";
		return (T*)std::malloc(a);
	}
	static void deallocate(T* p, std::size_t n)
	{
		auto d = sizeof(T)*n;
		tot_d += d;
		//std::cout << "deallocated " << d << " bytes\n";
		std::free(p);
	}

	template<typename... Args>
	static void construct(T* p, Args&&... args)
	{
		new (p) T(std::forward<Args>(args)...);
	}

	static void destroy(T* p)
	{
		p->~T();
	}
};

struct Base
{
	virtual ~Base() = default;

	virtual void Print() const {};
};

std::ostream& operator<<(std::ostream& out, const Base& b)
{
	b.Print();
	return out;
}

struct Int : Base
{
	Int(int i) : i(i) {}
	virtual void Print() const override { std::cout << i; }
	int i;
};

struct Complex : Base
{
	Complex(float r, float i) : r(r), i(i) {}
	virtual void Print() const override { std::cout << r << "+" << i << "i"; }
	float r, i;
};

template<typename T>
void printit(const T& cont)
{
	for(auto&& x : cont)
	{
		std::cout << x << "  ";
	}
	std::cout << std::endl;
}

template<typename T>
using Alloc = MyAlloc<T,1024>;

int main()
{
	using namespace std;

	{
		polymorphic_container<Base, std::vector, Alloc> pcb;

		auto it_c = pcb.cbegin();
		auto it_r = pcb.begin();
		it_c = it_r;
		// it_r = it_c;

		pcb.emplace_back<Int>(3);
		pcb.emplace_back<Complex>(3.3f, 0.0f);
		pcb.emplace_back<Int>(7);
		pcb.emplace_back<Complex>(0.3f, 2.2f);
		pcb.emplace_back<Int>(11);

		cout << "reverse:" << endl;
		auto itr = pcb.crbegin();
		while( itr != pcb.crend() )
		{
			itr->Print(); cout << endl;
			++itr;
		}

		Complex c(1, 2);
		pcb.push_back(c);
		pcb.push_back(Complex{ 2,1 });

		auto iter = pcb.begin();
		iter = iter + 1;
		++iter;
		iter += 1;
		int i = iter - pcb.begin();

		cout << endl << "distance:" << endl;
		cout << i << endl;

		decltype(pcb)::iterator i1, i2;

		i1 = pcb.begin();
		i2 = (++i1);

		cout << endl << "regular:" << endl;
		for(size_t i=0; i<pcb.size(); ++i)
		{
			pcb.at(i).Print();
			cout << endl;
		}

		cout << endl << "oneline:" << endl;
		printit(pcb);

		polymorphic_container<int, std::list, Alloc> pcil;

		pcil.push_back(5);
		pcil.push_back(i);
		pcil.push_back(new int(7));
		
		cout << endl << "oneline int-list:" << endl;
		printit(pcil);

		cout << boolalpha << (pcil==pcil) << (pcil<pcil) << endl;

		decltype(pcil)::iterator it1, it2;

		it1 = pcil.begin();
		it2 = (++it1);

		cout << *it2 << endl;

	}

	cout << endl << tot_a << endl << tot_d << endl;

	cout << "\ndone\n";

}



