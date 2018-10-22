
template<typename T, template<typename...> class A>
bool operator<(const TreeVector<T, A>& lhs, const TreeVector<T, A>& rhs)
{
	return lhs.compare(rhs) < 0;
}

template<typename T, template<typename...> class A>
bool operator<=(const TreeVector<T, A>& lhs, const TreeVector<T, A>& rhs)
{
	return lhs.compare(rhs) <= 0;
}

template<typename T, template<typename...> class A>
bool operator>(const TreeVector<T, A>& lhs, const TreeVector<T, A>& rhs)
{
	return lhs.compare(rhs) > 0;
}

template<typename T, template<typename...> class A>
bool operator>=(const TreeVector<T, A>& lhs, const TreeVector<T, A>& rhs)
{
	return lhs.compare(rhs) >= 0;
}

template<typename T, template<typename...> class A>
bool operator==(const TreeVector<T, A>& lhs, const TreeVector<T, A>& rhs)
{
	return lhs.compare(rhs) == 0;
}

template<typename T, template<typename...> class A>
bool operator!=(const TreeVector<T, A>& lhs, const TreeVector<T, A>& rhs)
{
	return lhs.compare(rhs) != 0;
}

template<typename T, template<typename...> class A>
void swap(TreeVector<T, A>& a, TreeVector<T, A>& b)
{
	a.swap(b);
}

template<typename T, template<typename...> class A>
std::ostream& operator<<(std::ostream& out, const TreeVector<T, A>& tv)
{
	tv.print(out);
	return out;
}

template<typename T, template<typename...> class A>
std::string to_string(const TreeVector<T, A>& tv)
{
	return tv.to_string();
}
