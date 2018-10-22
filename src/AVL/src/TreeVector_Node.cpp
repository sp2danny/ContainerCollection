
template<typename T, template<typename...> class A>
TreeVector<T, A>::Node::Node(sentry_tag) : s{0, 0}
{
}

template<typename T, template<typename...> class A>
template<typename... Args>
TreeVector<T, A>::Node::Node(node_tag, Args&&... args)
	: n(std::forward<Args>(args)...)
{
}

template<typename T, template<typename...> class A>
TreeVector<T, A>::Node::~Node()
{
	if (s.weight)
		n.item.~T();
}

template<typename T, template<typename...> class A>
int& TreeVector<T, A>::Node::weight()
{
	return s.weight;
}

template<typename T, template<typename...> class A>
int& TreeVector<T, A>::Node::height()
{
	return s.height;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::Node::parent() -> NodeP&
{
	return s.parent;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::Node::left() -> NodeP&
{
	return s.left;
}

template<typename T, template<typename...> class A>
auto TreeVector<T, A>::Node::right() -> NodeP&
{
	return s.right;
}

template<typename T, template<typename...> class A>
T& TreeVector<T, A>::Node::item()
{
	assert(!sentry());
	return n.item;
}

template<typename T, template<typename...> class A>
bool TreeVector<T, A>::Node::sentry()
{
	return !s.weight;
}

template<typename T, template<typename...> class A>
int TreeVector<T, A>::Node::balance()
{
	return right()->height() - left()->height();
}

// ----------------------------------------------------------------------------
