
#pragma once


template<typename T>
struct is_iterator
{
	typedef char yes_t;
	typedef long no_t;

	no_t test(...);

	template<typename U=T, typename =
		decltype( ++std::declval<U&>(), *std::declval<U&>(), std::declval<U&>()!=std::declval<U&>(), void() )
	>
	yes_t test( U* u );

	const static bool value = sizeof(test(0)) == sizeof(yes_t);
};



