
// AVL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



#include <iostream>
#include <gsl/multi_span>
#include <memory>

constexpr int X_SZ = 3;
constexpr int Y_SZ = 4;
constexpr int Z_SZ = 5;

auto arrptr = std::make_unique<int[]>( X_SZ * Y_SZ * Z_SZ );

gsl::multi_span<int, X_SZ, Y_SZ, Z_SZ> arr3d ( arrptr.get(), X_SZ*Y_SZ*Z_SZ );

int foobar()
{
	arr3d[1][1][1] += 7;
	for(auto&& x : arr3d)
		x += 1;
	return arr3d[1][1][1];
}


#include "TreeVector.hpp"
#include "TreeVector.cpp"

template class TreeVector<int>;

using namespace std::string_literals;

extern void foo();

template<typename Result>
void construct_from(Result& res)
{
}

template<typename Result,typename Arg1,typename... Rest>
void construct_from(Result& res, Arg1&& a1, Rest&&... rest)
{
	typedef std::remove_reference<Arg1>::type A;
	std::get<A>(res) = std::forward<Arg1>(a1);
	construct_from(res, std::forward<Rest>(rest)... );
}

typedef std::tuple<int,float,std::string> IFS;

template<typename... Args>
void print_vals(Args&&... args)
{
	IFS ifs;
	std::get<0>(ifs) = 2;
	std::get<1>(ifs) = 2.2f;
	std::get<2>(ifs) = "222"s;

	construct_from(ifs, std::forward<Args>(args)... );

	std::cout << std::get<0>(ifs) << std::endl;
	std::cout << std::get<1>(ifs) << std::endl;
	std::cout << std::get<2>(ifs) << std::endl;
}

#include "atomic_stack.hpp"

atomic_stack<int> asi;

template class atomic_stack<int>;

void thread_one()
{
	for(int i=1; i<=100; ++i)
		asi.push( i );
}

void thread_two()
{
	while( ! asi.empty() )
	{
		int i = asi.pop();
		std::cout << i << ' ';
	}
	std::cout << "\n\n";
}

#include <thread>

int main()
{
	extern void testit();
	testit();



	using namespace std;

	foo();

	asi.push(-1);

	std::thread t1(thread_one);

	thread_two();

	t1.join();

	extern int xxx_main();
	xxx_main(); exit(0);

	print_vals(3.3f,55);

	foo();



	//#ifndef NDEBUG
	fgetc(stdin);
	//#endif
}

//-----------------------------------------------

struct pick_7          {};
struct pick_6 : pick_7 {};
struct pick_5 : pick_6 {};
struct pick_4 : pick_5 {};
struct pick_3 : pick_4 {};
struct pick_2 : pick_3 {};
struct pick_1 : pick_2 {};
static pick_1 selector;

//-----------------------------------------------

template<typename V1, typename V2>
auto helper_compare(pick_1, const V1& v1, const V2& v2)
	-> decltype((int)v1.compare(v2))
{
	return v1.compare(v2);
}

template<typename V1, typename V2>
auto helper_compare(pick_2, const V1& v1, const V2& v2)
	-> decltype((int)compare(v1, v2))
{
	return compare(v1, v2);
}

template<typename V1, typename V2>
auto helper_compare(pick_3, const V1& v1, const V2& v2)
	-> decltype((int)(v1 - v2))
{
	return v1 - v2;
}

template<typename V1, typename V2>
auto helper_compare(pick_4, const V1& v1, const V2& v2)
	-> decltype(bool(v2 < v1), bool(v1 < v2), int{})
{
	if (v1 < v2)
		return -1;
	if (v2 < v1)
		return +1;
	return 0;
}

template<typename V1, typename V2>
auto helper_compare(pick_5, const V1& v1, const V2& v2)
	-> decltype(std::less<V1>()(v1, v2), int{})
{
	if (std::less<V1>()(v1, v2))
		return -1;
	if (std::less<V1>()(v2, v1))
		return +1;
	return 0;
}

template<typename C1, typename C2>
auto compare(const C1& c1, const C2& c2)
	-> typename std::enable_if<
		std::is_same<decltype(std::begin(c1)), decltype(std::end(c1))>::value &&
		std::is_same<decltype(std::begin(c2)), decltype(std::end(c2))>::value &&
		std::is_same<decltype(*std::begin(c1)),
		decltype(*std::begin(c2))>::value, int>::type
{
	auto i1 = std::begin(c1);
	auto i2 = std::begin(c2);
	auto e1 = std::end(c1);
	auto e2 = std::end(c2);

	while (true)
	{
		if ((i1 == e1) && (i2 == e2))
			return 0;

		if (i1 == e1)
			return -1;
		if (i2 == e2)
			return +1;

		int cmp = helper_compare(selector, *i1, *i2);
		if (cmp)
			return cmp;

		++i1;
		++i2;
	}
}

#include <list>
#include <iostream>

#include <random>
#include <algorithm>

//-----------------------------------------------

template<typename I>
void push_back(const I&)
{
}

template<typename I1, typename C1, typename... Cs>
void push_back(const I1& i, C1& c1, Cs&&... cs)
{
	c1.push_back(i);
	push_back(i, std::forward<Cs>(cs)...);
}

//-----------------------------------------------

void pop_back() {}

template<typename C1, typename... Cs>
void pop_back(C1& c1, Cs&&... cs)
{
	c1.pop_back();
	pop_back(std::forward<Cs>(cs)...);
}

//-----------------------------------------------

template<typename C1>
auto nth(C1& c1, std::size_t p, pick_1) -> decltype(c1.nth(p))
{
	return c1.nth(p);
}

template<typename C1>
auto nth(C1& c1, std::size_t p, pick_2) -> auto
{
	auto itr = c1.begin();
	std::advance(itr, p);
	return itr;
}

void erase_index(std::size_t) {}

template<typename C1, typename... Cs>
void erase_index(std::size_t idx, C1& c1, Cs&&... cs)
{
	auto itr = nth(c1, idx, selector);
	c1.erase(itr);
	erase_index(idx, std::forward<Cs>(cs)...);
}

//-----------------------------------------------

void sort() {}

template<typename C1>
auto d_sort(C1& c1, pick_1) -> decltype(c1.sort())
{
	return c1.sort();
}

template<typename C1>
void d_sort(C1& c1, pick_2)
{
	std::sort(std::begin(c1), std::end(c1));
}

template<typename C1, typename... Cs>
void sort(C1& c1, Cs&&... cs)
{
	d_sort(c1, selector);
	sort(std::forward<Cs>(cs)...);
}

//-----------------------------------------------

void reverse() {}

template<typename C1>
auto d_reverse(C1& c1, pick_1) -> decltype(c1.reverse())
{
	return c1.reverse();
}

template<typename C1>
void d_reverse(C1& c1, pick_2)
{
	std::reverse(std::begin(c1), std::end(c1));
}

template<typename C1, typename... Cs>
void reverse(C1& c1, Cs&&... cs)
{
	d_reverse(c1, selector);
	reverse(std::forward<Cs>(cs)...);
}

//-----------------------------------------------

template<typename I1>
void insert_sorted(const I1& i)
{
}

template<typename C1, typename I1>
auto d_ub(C1& c1, const I1& i1, pick_1) -> decltype(c1.upper_bound(i1))
{
	return c1.upper_bound(i1);
}

template<typename C1, typename I1>
auto d_ub(C1& c1, const I1& i1, pick_2) -> auto
{
	return std::upper_bound(std::begin(c1), std::end(c1), i1);
}

template<typename I1, typename C1>
auto d_insert_sorted(const I1& i, C1& c1, pick_1)
    -> decltype(c1.insert_sorted(i), void())
{
	c1.insert_sorted(i);
}

template<typename I1, typename C1>
void d_insert_sorted(const I1& i, C1& c1, pick_2)
{
	auto pos = d_ub(c1, i, selector);
	c1.insert(pos, i);
}

template<typename I1, typename C1, typename... Cs>
void insert_sorted(const I1& i, C1& c1, Cs&&... cs)
{
	d_insert_sorted(i, c1, selector);
	insert_sorted(i, std::forward<Cs>(cs)...);
}

//-----------------------------------------------

template<typename C1>
bool all_eq(const C1&)
{
	return true;
}

template<typename C1, typename C2, typename... Cs>
bool all_eq(const C1& c1, const C2& c2, Cs&&... cs)
{
	if (compare(c1, c2) != 0)
		return false;
	return all_eq(c1, std::forward<Cs>(cs)...);
}

//-----------------------------------------------

void unique() {}

template<typename C1>
auto d_unique(C1& c1, pick_1) -> decltype(c1.unique())
{
	return c1.unique();
}

template<typename C1>
void d_unique(C1& c1, pick_2)
{
	auto itr = std::unique(std::begin(c1), std::end(c1));
	c1.erase(itr, std::end(c1));
}

template<typename C1, typename... Cs>
void unique(C1& c1, Cs&&... cs)
{
	d_unique(c1, selector);
	unique(std::forward<Cs>(cs)...);
}

//-----------------------------------------------

/* */
template<typename C1, typename Op>
auto helper_for_all(pick_1, C1& cont, Op&& op)
    -> decltype(cont.for_each(op), void())
{
	cont.for_each(std::forward<Op>(op));
}
/* */

template<typename C1, typename Op>
auto helper_for_all(pick_2, C1& cont, Op&& op) -> decltype(void())
{
	std::for_each(cont.begin(), cont.end(), std::forward<Op>(op));
}

//-----------------------------------------------

template<typename T>
T makeval()
{
	return T{};
}

template<>
int makeval<int>()
{
	return rand();
}

template<typename T>
double pt_insert_erase(int size, int reps)
{
	using namespace std;
	using namespace chrono;

	T cont;
	typedef T::value_type VT;
	for (int i = 0; i < size; ++i)
	{
		cont.push_back(makeval<VT>());
	}
	auto iter = cont.begin();
	advance(iter, size / 2);
	auto t1 = high_resolution_clock::now();
	for (int i = 0; i < reps; ++i)
	{
		iter = cont.insert(iter, makeval<VT>());
		iter = cont.erase(iter);
	}
	auto t2 = high_resolution_clock::now();
	duration<double, milli> dur = t2 - t1;
	return dur.count();
}

template<typename T>
double pt_random_access(int size, int reps)
{
	using namespace std;
	using namespace chrono;

	T cont;
	typedef T::value_type VT;
	for (int i = 0; i < size; ++i)
	{
		cont.push_back(makeval<VT>());
	}
	auto t1 = high_resolution_clock::now();
	for (int i = 0; i < reps; ++i)
	{
		auto iter = cont.begin();
		advance(iter, rand() % size);
		*iter = makeval<VT>();
	}
	auto t2 = high_resolution_clock::now();
	duration<double, milli> dur = t2 - t1;
	return dur.count();
}

template<typename T>
double pt_iterate(int size, int reps)
{
	using namespace std;
	using namespace chrono;

	T cont;
	typedef T::value_type VT;
	for (int i = 0; i < size; ++i)
	{
		cont.push_back(makeval<VT>());
	}
	auto t1 = high_resolution_clock::now();
	for (int i = 0; i < reps; ++i)
	{
		int  cnt  = 0;
		auto iter = cont.begin();
		while (iter != cont.end())
		{
			*iter = VT{};
			++iter;
			++cnt;
		}
		assert(cnt == size);
	}
	auto t2 = high_resolution_clock::now();
	duration<double, milli> dur = t2 - t1;
	return dur.count();
}

template<typename T>
double pt_sort(int size, int reps)
{
	using namespace std;
	using namespace chrono;

	T cont;
	typedef T::value_type VT;
	for (int i = 0; i < size; ++i)
	{
		cont.push_back(makeval<VT>());
	}
	auto t1 = high_resolution_clock::now();
	for (int i = 0; i < reps; ++i)
	{
		sort(cont);
	}
	auto t2 = high_resolution_clock::now();
	duration<double, milli> dur = t2 - t1;
	return dur.count();
}

template<typename T>
double pt_for_all(int size, int reps)
{
	using namespace std;
	using namespace chrono;

	T cont;
	typedef T::value_type VT;
	for (int i = 0; i < size; ++i)
	{
		cont.push_back(makeval<VT>());
	}

	auto set_val = [](VT& val) { val = makeval<VT>(); };

	auto t1 = high_resolution_clock::now();

	for (int i = 0; i < reps; ++i)
	{
		helper_for_all(selector, cont, set_val);
	}

	auto t2 = high_resolution_clock::now();

	duration<double, milli> dur = t2 - t1;
	return dur.count();
}

#define AA_USE_RANDOM_ACCESS_TAG

#include "avl_array.hpp"

static long long current_size;
static long long maximum_size;
static long long total_alloc_size;

static void clr_alloc_cnt()
{
	current_size     = 0;
	maximum_size     = 0;
	total_alloc_size = 0;
}

static void prnt_alloc_cnt()
{
	std::cout << "current_size     " << current_size << "\n";
	std::cout << "maximum_size     " << maximum_size << "\n";
	std::cout << "total_alloc_size " << total_alloc_size << "\n";
}

static void upd_alloc(std::size_t sz)
{
	current_size += sz;
	total_alloc_size += sz;
	if (current_size > maximum_size)
		maximum_size = current_size;
}
static void upd_dealloc(std::size_t sz) { current_size -= sz; }

template<typename T>
class counting_allocator : public std::allocator<T>
{
public:
	T* allocate(std::size_t n)
	{
		std::size_t sz = n * sizeof(T);
		upd_alloc(sz);
		return allocator::allocate(n);
	}
	void deallocate(T* p, std::size_t n)
	{
		std::size_t sz = n * sizeof(T);
		upd_dealloc(sz);
		allocator::deallocate(p, n);
	}
	template<typename U>
	struct rebind
	{
		typedef counting_allocator<U> other;
	};
};

void foo()
{

	using namespace std;
	using namespace mkr;

	cout << foobar() << endl;


	cout << "TreeVector\n";
	typedef TreeVector<int, counting_allocator /*<int>*/> CNT_TV;
	{
		CNT_TV cnt;
		for (int i = 0; i < 100; ++i)
		{
			cnt.push_back(i);
		}
	}
	prnt_alloc_cnt();

	cout << "\n\navl_tree\n";
	typedef avl_array<int, counting_allocator<int>> CNT_AT;
	clr_alloc_cnt();
	{
		CNT_AT cnt;
		for (int i = 0; i < 100; ++i)
		{
			cnt.push_back(i);
		}
	}
	prnt_alloc_cnt();

	/*
	int errcnt = 0;

	TVI tvi = { 1,2,3,4,4,5,6,7,8,8,9 };
	cout << tvi << endl;
	tvi.unique();
	cout << tvi << endl;

	for(int rep=0;rep<100;++rep)
	{

	    TreeVector<int> tvi;
	    vector<int> vi;
	    list<int> li;
	    //avl_array<int> aai;

	    #define LST tvi, vi, li
	    //, aai

	    #define TST do { if (!all_eq(LST)) ++errcnt; if(!tvi.integrity())
	++errcnt; } while(false)

	    for (int i=0;i<25;++i)
	    {
	        int val = rand()%100;
	        push_back( val, LST );
	    }

	    TST;

	    erase_index( 2 + rand()%20, LST );

	    TST;

	    sort( LST );

	    TST;

	    insert_sorted( rand()%100, LST );

	    TST;

	    unique( LST );

	    TST;

	    pop_back( LST );

	    TST;

	    reverse( LST );

	    TST;

	}

	#undef TST

	cout << errcnt << endl;

	*/

	cout << "--------------------------------------------" << endl;

#ifndef NDEBUG
	const int SZ  = 25'000;
	const int REP = 100;
#else
	const int SZ  = 100'000;
	const int REP = 1'000;
#endif

#define TST(v, tst, sz, rep)                                                   \
	auto t1##v = pt_##tst<vector<int>>(sz, rep);                               \
	auto t2##v = pt_##tst<list<int>>(sz, rep);                                 \
	auto t3##v = pt_##tst<TreeVector<int>>(sz, rep);                           \
	auto t4##v = pt_##tst<avl_array<int>>(sz, rep);                            \
	cout << #tst " : sz " << sz << endl;                                       \
	cout << "\tstd::vector     " << t1##v << endl;                             \
	cout << "\tstd::list       " << t2##v << endl;                             \
	cout << "\tTreeVector      " << t3##v << endl;                             \
	cout << "\tmkr::avl_array  " << t4##v << endl

	TST(a, insert_erase, SZ, REP * 5);

	TST(b, random_access, SZ, REP * 5);

	TST(c, iterate, SZ, REP / 100);

	TST(d, for_all, SZ, REP / 100);

	TST(z, sort, SZ, 1);

	cout << "summation : sz " << SZ << endl;
	cout << "\tstd::vector     " << t1a + t1b + t1c + t1d + t1z << endl;
	cout << "\tstd::list       " << t2a + t2b + t2c + t2d + t1z << endl;
	cout << "\tTreeVector      " << t3a + t3b + t3c + t3d + t1z << endl;
	cout << "\tmkr::avl_array  " << t4a + t4b + t4c + t4d + t1z << endl;
}

