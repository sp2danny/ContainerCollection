
#include "stdafx.h"

#include "TreeVector.hpp"

template <typename T, template <typename...> class A>
void TreeVector<T, A>::setEmpty()
{
	root->height() = root->weight() = 0;
	nil->height()  = nil->weight()  = 0;
	root->parent() = root->left()   = root->right() = nil;
	nil->parent()  = nil->left()    = nil->right()  = nil;
}

template <typename T, template <typename...> class A>
TreeVector<T, A>::TreeVector()
{
	root = makeSentry();
	nil  = makeSentry();
	setEmpty();
}

template <typename T, template <typename...> class A>
TreeVector<T, A>::TreeVector(const allocator_type& a)
	: alloc(a)
{
	root = makeSentry();
	nil  = makeSentry();
	setEmpty();
}

template <typename T, template <typename...> class A>
TreeVector<T, A>::TreeVector(const TreeVector& other)
	: TreeVector()
{
	construct(other.begin(), other.end());
}

template <typename T, template <typename...> class A>
TreeVector<T, A>::TreeVector(TreeVector&& other)
{
	swap(other);
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::operator=(const TreeVector& other) -> TreeVector&
{
	clear();
	construct(other.begin(), other.end());
	return *this;
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::operator=(TreeVector&& other) noexcept -> TreeVector&
{
	swap(other);
	return *this;
}

template <typename T, template <typename...> class A>
TreeVector<T, A>::~TreeVector()
{
	clear();
	alloc.deallocate(root, 1);
	alloc.deallocate(nil, 1);
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::swap(TreeVector& other) noexcept
{
	using std::swap;
	swap(alloc, other.alloc);
	swap(root, other.root);
	swap(nil, other.nil);
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::clear()
{
	static void (*rec_clr)(NodeP, TreeVector*, NodeP) =
		[](NodeP node, TreeVector* me, NodeP nil) {
			if (node == nil)
				return;
			rec_clr(node->left(), me, nil);
			rec_clr(node->right(), me, nil);
			me->unmakeNode(node);
		};

	rec_clr(head(), this, nil);
	setEmpty();
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::back() -> T&
{
	NodeP p = lastNode();
	assert(!p->sentry());
	return p->item();
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::back() const -> const T&
{
	NodeP p = lastNode();
	assert(!p->sentry());
	return p->item();
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::front() -> T&
{
	NodeP p = firstNode();
	assert(!p->sentry());
	return p->item();
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::front() const -> const T&
{
	NodeP p = firstNode();
	assert(!p->sentry());
	return p->item();
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::prevNode(NodeP n) const -> NodeP
{
	if (n == root)
	{
		while (n->right() != nil)
			n = n->right();
		return n;
	}

	NodeP nTemp;

	if (n->left() != nil)
	{
		n = n->left();
		while (n->right() != nil)
			n = n->right();
	}
	else
	{
		nTemp = n;
		n	 = n->parent();
		while ((n != nil) && (n->left() == nTemp))
		{
			nTemp = n;
			n	 = n->parent();
		}
	}
	return n;
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::nextNode(NodeP n) const -> NodeP
{
	if (n == root)
	{
		while (n->left() != nil)
			n = n->left();
		return n;
	}

	Node* nTemp;

	if (n->right() != nil)
	{
		n = n->right();
		while (n->left() != nil)
			n = n->left();
	} else
	{
		nTemp = n;
		n = n->parent();
		while ((n != nil) && (n->right() == nTemp))
		{
			nTemp = n;
			n = n->parent();
		}
	}

	return n;
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::head() const -> NodeP&
{
	return root->left();
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::firstNode() const -> NodeP
{
	NodeP p = head();
	if (p == nil)
		return root;
	while (p->left() != nil)
		p = p->left();
	return p;
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::lastNode() const -> NodeP
{
	NodeP p = head();
	if (p == nil)
		return root;
	while (p->right() != nil)
		p = p->right();
	return p;
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::getIdx(NodeP p, std::size_t idx) const -> NodeP
{
	assert(!p->sentry());

	auto lw = p->left()->weight();

	if (lw == idx)
		return p;

	if (int(idx) < lw)
		return getIdx(p->left(), idx);

	idx -= lw + 1;
	return getIdx(p->right(), idx);
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::operator[](std::size_t idx) -> T&
{
	return getIdx(head(), idx)->item();
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::operator[](std::size_t idx) const -> const T&
{
	return getIdx(head(), idx)->item();
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::at(std::size_t idx) -> T&
{
	if (idx >= size())
		throw std::out_of_range("index");
	return getIdx(head(), idx)->item();
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::at(std::size_t idx) const -> const T&
{
	if (idx >= size())
		throw std::out_of_range("index");
	return getIdx(head(), idx)->item();
}

template <typename T, template <typename...> class A>
std::size_t TreeVector<T, A>::index_of(iterator i) const
{
	assert(i.owner == this);
	return nodeIdx(i.node);
}

template <typename T, template <typename...> class A>
std::size_t TreeVector<T, A>::index_of(const_iterator i) const
{
	assert(i.owner == this);
	return nodeIdx(i.node);
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::nth(std::size_t idx) -> iterator
{
	auto sz = size();
	if (idx == sz)
		return end();
	if (idx > sz)
		throw std::out_of_range("index");
	Node* n = getIdx(head(), idx);
	return {n, this};
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::nth(std::size_t idx) const -> const_iterator
{
	auto sz = size();
	if (idx == sz)
		return end();
	if (idx > sz)
		throw std::out_of_range("index");
	Node* n = getIdx(head(), idx);
	return {n, this};
}

// ----------------------------------------------------------------------------

#include "TreeVector_Iterator.cpp"

// ----------------------------------------------------------------------------

#include "TreeVector_Node.cpp"

// ----------------------------------------------------------------------------

#include "TreeVector_Print.cpp"

// ----------------------------------------------------------------------------

template <typename T, template <typename...> class A>
std::size_t TreeVector<T, A>::nodeIdx(NodeP n) const
{
	if (n == root)
		return head()->weight();
	bool lft;
	int  idx = n->left()->weight();
	while (true)
	{
		lft = isLeft(n);
		n   = n->parent();
		if (n == root)
			break;
		if (!lft)
			idx += 1 + n->left()->weight();
	}
	return idx;
}

template <typename T, template <typename...> class A>
template <typename... Args>
auto TreeVector<T, A>::makeNode(Args&&... args) -> NodeP
{
	NodeP n = alloc.allocate(1);
	new (n) Node(node_tag{}, std::forward<Args>(args)...);
	return n;
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::makeSentry() -> NodeP
{
	NodeP n = alloc.allocate(1);
	new (n) Node(sentry_tag{});
	return n;
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::unmakeNode(NodeP p)
{
	p->~Node();
	alloc.deallocate(p, 1);
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::linkL(NodeP p, NodeP l)
{
	p->left()   = l;
	l->parent() = p;
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::linkR(NodeP p, NodeP r)
{
	p->right()  = r;
	r->parent() = p;
}

template <typename T, template <typename...> class A>
bool TreeVector<T, A>::isLeft(NodeP n) const
{
	return n->parent()->left() == n;
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::fixHW(NodeP p)
{
	p->height() = 1 + std::max(p->left()->height(), p->right()->height());
	p->weight() = 1 + p->left()->weight() + p->right()->weight();
}

template <typename T, template <typename...> class A>
template <typename It, typename>
void TreeVector<T, A>::assign(It b, It e)
{
	clear();
	construct(b, e);
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::assign(std::size_t sz, const T& t)
{
	clear();
	construct(sz, t);
}

template <typename T, template <typename...> class A>
template <typename Cont>
void TreeVector<T, A>::assign(const Cont& cnt)
{
	clear();
	construct(cnt.begin(), cnt.end());
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::assign(std::initializer_list<T> il)
{
	clear();
	construct(il.begin(), il.end());
}

template <typename T, template <typename...> class A>
TreeVector<T, A>::TreeVector(std::initializer_list<T> il) : TreeVector()
{
	construct(il.begin(), il.end());
}

template <typename T, template <typename...> class A>
TreeVector<T, A>::TreeVector(std::size_t n, const T& t) : TreeVector()
{
	construct(n, t);
}

template <typename T, template <typename...> class A>
template <typename It, typename>
TreeVector<T, A>::TreeVector(It b, It e) : TreeVector()
{
	construct(b, e);
}

template <typename T, template <typename...> class A>
template <typename It>
void TreeVector<T, A>::construct(It b, It e, std::random_access_iterator_tag)
{
	static void (*ra)(TreeVector*, NodeP, NodeP&, It, It, NodeP) =
		[](TreeVector* self, NodeP par, NodeP& me, It a, It b, NodeP nil) {
			auto sz = b - a;
			if (sz == 0)
			{
				me = nil;
				return;
			}
			if (sz == 1)
			{
				me = self->makeNode(*a);
				me->parent() = par;
				me->left() = me->right() = nil;
				me->weight() = me->height() = 1;
				return;
			}
			auto p = sz / 2;
			me = self->makeNode(a[p]);
			me->parent() = par;
			me->weight() = me->height() = -1;
			ra(self, me, me->left(), a, a + p, nil);
			ra(self, me, me->right(), a + p + 1, b, nil);
			self->fixHW(me);
		};

	ra(this, root, head(), b, e, nil);
}

template <typename T, template <typename...> class A>
template <typename It>
void TreeVector<T, A>::construct(It b, It e, std::forward_iterator_tag)
{
	while (b != e)
	{
		push_back(*b);
		++b;
	}
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::construct(std::size_t n, const T& t)
{
	static void (*ra)(TreeVector*, NodeP, NodeP&, std::size_t, const T&, NodeP) =
		[](TreeVector* self, NodeP par, NodeP& me, std::size_t sz, const T& t, NodeP nil) {
			if (sz == 0)
			{
				me = nil;
				return;
			}
			if (sz == 1)
			{
				me = self->makeNode(t);
				me->parent() = par;
				me->left() = me->right() = nil;
				return;
			}
			auto p = sz / 2;
			me	 = self->makeNode(t);
			ra(self, me, me->left(), p, t, nil);
			ra(self, me, me->right(), p - 1, t, nil);
			self->fixHW(me);
		};

	ra(this, root, head(), n, t, nil);
}

template <typename T, template <typename...> class A>
template <typename It>
void TreeVector<T, A>::construct_p(It b, It e)
{
	static void (*ra)(TreeVector*, NodeP, NodeP&, It, It, NodeP) =
		[](TreeVector* self, NodeP par, NodeP& me, It a, It b, NodeP nil) {
			auto sz = b - a;
			assert(sz >= 0);
			if (sz == 0)
			{
				me = nil;
				return;
			}
			if (sz == 1)
			{
				me = *a;
				me->parent() = par;
				me->left() = me->right() = nil;
				me->weight() = me->height() = 1;
				return;
			}
			auto p = sz / 2;
			me = a[p];
			me->parent() = par;
			ra(self, me, me->left(), a, a + p, nil);
			ra(self, me, me->right(), a + p + 1, b, nil);
			self->fixHW(me);
		};

	ra(this, root, head(), b, e, nil);
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::pop_back()
{
	NodeP p = lastNode();
	assert(!p->sentry());
	treeDeleteNode(p);
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::pop_front()
{
	NodeP p = firstNode();
	assert(!p->sentry());
	treeDeleteNode(p);
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::push_back(const T& t)
{
	insert(end(), t);
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::push_back(T&& t)
{
	insert(end(), (T &&) t);
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::push_front(const T& t)
{
	insert(begin(), t);
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::push_front(T&& t)
{
	insert(begin(), (T &&) t);
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::insert(iterator i, const T& t) -> iterator
{
	return emplace(i, t);
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::insert(iterator i, T&& t) -> iterator
{
	return emplace(i, (T &&) t);
}

template <typename T, template <typename...> class A>
template <typename... Args>
auto TreeVector<T, A>::emplace(iterator i, Args&&... args) -> iterator
{
	NodeP p = makeNode(std::forward<Args>(args)...);

	treeInsert(i.node, p);

	return {p, this};
}

template <typename T, template <typename...> class A>
template <typename... Args>
T& TreeVector<T, A>::emplace_back(Args&&... args)
{
	return *emplace(end(), std::forward<Args>(args)...);
}

template <typename T, template <typename...> class A>
template <typename... Args>
T& TreeVector<T, A>::emplace_front(Args&&... args)
{
	return *emplace(begin(), std::forward<Args>(args)...);
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::upper_bound(const T& data) -> iterator
{
	NodeP node = head();
	NodeP lwl  = root;

	if (node->sentry())
		return end();

	while (true)
	{
		if (data < node->item())
		{
			if (node->left() == nil)
				return {node, this};
			lwl  = node;
			node = node->left();
		} else
		{
			if (node->right() == nil)
				return {lwl, this};
			node = node->right();
		}
	}
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::lower_bound(const T& data) -> iterator
{
	NodeP node = head();
	NodeP lwl  = root;

	if (node->sentry())
		return end();

	while (true)
	{
		if (data < node->item())
		{
			if (node->left() == nil)
				return {node, this};
			lwl  = node;
			node = node->left();
		} else if (node->item() < data)
		{
			if (node->right() == nil)
				return {lwl, this};
			node = node->right();
		} else
		{
			while (true)
			{
				NodeP prv = prevNode(node);
				if (prv == nil)
					break;
				if (prv == root)
					break;
				if (prv->item() < data)
					break;
				node = prv;
			}
			return {node, this};
		}
	}
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::upper_bound(const T& t) const -> const_iterator
{
	return {((TreeVector*)this)->upper_bound(t)};
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::lower_bound(const T& t) const -> const_iterator
{
	return {((TreeVector*)this)->lower_bound(t)};
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::unstable_insert_position(const T& data) -> iterator
{
	NodeP node = head();
	NodeP lwl  = root;

	if (node->sentry())
		return end();

	while (true)
	{
		auto&& ni = node->item();
		if (data < ni)
		{
			if (node->left() == nil)
				return {node, this};
			lwl  = node;
			node = node->left();
		} else if (ni < data)
		{
			if (node->right() == nil)
				return {lwl, this};
			node = node->right();
		} else
		{
			return {node, this};
		}
	}
}

template <typename T, template <typename...> class A>
template <typename Op>
bool TreeVector<T, A>::is_sorted(Op&& op) const
{
	if (size() <= 1)
		return true;
	auto curr = begin();
	auto next = curr;
	++next;
	while (next != end())
	{
		if (op(*next, *curr))
			return false;
		curr = next;
		++next;
	}
	return true;
}

template <typename T, template <typename...> class A>
template <typename Op>
void TreeVector<T, A>::sort(Op&& op)
{
	NVec vec;
	vec.reserve(size());

	recursiveFlatten(head(), vec);

	auto NodeLess = [&op](NodeP lhs, NodeP rhs) -> bool {
		return op(lhs->item(), rhs->item());
	};

	std::sort(vec.begin(), vec.end(), NodeLess);

	construct_p(vec.begin(), vec.end());
}

template <typename T, template <typename...> class A>
template <typename Op>
void TreeVector<T, A>::stable_sort(Op&& op)
{
	NVec vec;
	vec.reserve(size());

	recursiveFlatten(head(), vec);

	auto NodeLess = [&op](NodeP lhs, NodeP rhs) -> bool {
		return op(lhs->item(), rhs->item());
	};

	std::stable_sort(vec.begin(), vec.end(), NodeLess);

	construct_p(vec.begin(), vec.end());
}

template <typename T, template <typename...> class A>
template <typename Op>
void TreeVector<T, A>::merge(TreeVector& other, Op&& op)
{
	if (this == &other)
		return;
	if (other.empty())
		return;
	if (empty())
	{
		swap(other);
		return;
	}

	NVec tvec, ovec, mvec;
	size_t tsz = size();
	size_t osz = other.size();
	tvec.reserve(tsz);
	ovec.reserve(osz);
	mvec.reserve(tsz + osz);

	recursiveFlatten(head(), tvec);
	other.recursiveFlatten(other.head(), ovec);

	auto NodeLess = [&op](NodeP lhs, NodeP rhs) -> bool {
		return op(lhs->item(), rhs->item());
	};

	std::merge(tvec.begin(), tvec.end(), ovec.begin(), ovec.end(),
	           std::back_inserter(mvec), NodeLess);

	construct_p(mvec.begin(), mvec.end());

	other.setEmpty();
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::reverse()
{
	static void (*rec_rev)(NodeP) = [](NodeP p) -> void {
		if (p->sentry())
			return;
		using std::swap;
		swap(p->left(), p->right());
		rec_rev(p->left());
		rec_rev(p->right());
	};

	rec_rev(head());
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::resize(std::size_t sz, const T& t)
{
	std::size_t curr = size();
	if (sz == curr)
		return;

	if (!sz)
	{
		clear();
		return;
	}

	NVec vec;
	vec.reserve(std::max(sz, curr));

	recursiveFlatten(head(), vec);

	if (sz < curr)
	{
		for (auto i = sz; i < curr; ++i)
		{
			NodeP p = vec[i];
			unmakeNode(p);
		}
		construct_p(vec.begin(), vec.begin() + sz);
	}
	else
	{
		for (auto i = curr; i < sz; ++i)
		{
			NodeP p = makeNode(t);
			vec.push_back(p);
		}
		construct_p(vec.begin(), vec.end());
	}
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::splice(iterator pos, TreeVector& other, iterator item)
{
	assert(pos.owner == this);
	assert(item.owner == &other);
	NodeP where = pos.node;
	NodeP node  = item.node;
	if (node != where)
	{
		other.helperUnlink(node);
		treeInsert(where, node);
	}
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::splice(iterator pos, TreeVector& other)
{
	assert(this != &other);

	NVec vec;
	vec.reserve(size() + other.size());

	iterator ii = begin();
	while (ii != pos)
	{
		vec.push_back(ii.node);
		++ii;
	}

	other.recursiveFlatten(other.head(), vec);

	while (ii != end())
	{
		vec.push_back(ii.node);
		++ii;
	}

	construct_p(vec.begin(), vec.end());
	other.setEmpty();
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::splice(iterator pos, TreeVector& other, iterator b, iterator e)
{
	assert(pos.owner == this);
	assert(b.owner == &other);
	assert(e.owner == &other);

	assert(this != &other);

	NVec tvec, ovec;

	auto ti = begin();
	auto te = end();
	auto oi = other.begin();
	auto oe = other.end();

	while (ti != pos)
	{
		tvec.push_back(ti.node);
		++ti;
	}
	while (oi != b)
	{
		ovec.push_back(oi.node);
		++oi;
	}
	while (oi != e)
	{
		tvec.push_back(oi.node);
		++oi;
	}
	while (ti != te)
	{
		tvec.push_back(ti.node);
		++ti;
	}
	while (oi != oe)
	{
		ovec.push_back(oi.node);
		++oi;
	}

	construct_p(tvec.begin(), tvec.end());
	other.construct_p(ovec.begin(), ovec.end());
}

template <typename T, template <typename...> class A>
int TreeVector<T, A>::compare(const TreeVector& other) const
{
	if (this == &other)
		return 0;

	auto lb = begin();
	auto rb = other.begin();
	auto le = end();
	auto re = other.end();

	while (true)
	{
		if ((lb == le) && (rb == re))
			return 0;
		if (lb == le)
			return -1;
		if (rb == re)
			return +1;
		if (*lb < *rb)
			return -1;
		if (*rb < *lb)
			return +1;
		++lb;
		++rb;
	}
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::begin() -> iterator
{
	NodeP p = firstNode();
	assert(!p->sentry());
	return iterator{p, this};
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::end() -> iterator
{
	return /*iterator*/{root, this};
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::begin() const -> const_iterator
{
	NodeP p = firstNode();
	assert(!p->sentry());
	return /*const_iterator*/{p, this};
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::end() const -> const_iterator
{
	NodeP p = root;
	return /*const_iterator*/{p, this};
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::erase(iterator i) -> iterator
{
	auto r = i;
	++r;
	treeDeleteNode(i.node);
	return r;
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::erase(iterator b, iterator e)
{
	while (b != e)
	{
		b = erase(b);
	}
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::unique()
{
	NVec keep, discard;
	keep.reserve(size());

	NodeP last = nullptr;

	auto want = [&](NodeP p) -> bool
	{
		if (last && (last->item() == p->item()))
		{
			return false;
		} else
		{
			last = p;
			return true;
		}
	};

	recursiveFlatten(head(), keep, discard, want);

	construct_p(keep.begin(), keep.end());

	for (auto&& p : discard)
		unmakeNode(p);
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::remove(const T& t)
{
	NVec keep, discard;
	keep.reserve(size());

	auto want = [&](NodeP n) -> bool { return n->item() != t; };

	recursiveFlatten(head(), keep, discard, want);

	construct_p(keep.begin(), keep.end());

	for (auto&& p : discard)
		unmakeNode(p);
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::recursiveFlatten(NodeP n, NVec& vec)
{
	if (n == nil)
		return;
	assert(!n->sentry());
	recursiveFlatten(n->left(), vec);
	vec.push_back(n);
	recursiveFlatten(n->right(), vec);
};

template <typename T, template <typename...> class A>
template <typename Op>
void TreeVector<T, A>::recursiveFlatten(NodeP n, NVec& keep, NVec& discard, Op&& want)
{
	if (n == nil)
		return;
	assert(!n->sentry());
	recursiveFlatten(n->left(), keep, discard, std::forward<Op>(want));
	if (want(n))
		keep.push_back(n);
	else
		discard.push_back(n);
	recursiveFlatten(n->right(), keep, discard, std::forward<Op>(want));
}

template <typename T, template <typename...> class A>
template <typename Op>
void TreeVector<T, A>::remove_if(Op&& op)
{
	NVec keep, discard;
	keep.reserve(size());

	auto want = [&](NodeP n) -> bool { return !op(n->item()); };

	recursiveFlatten(head(), keep, discard, want);

	construct_p(keep.begin(), keep.end());

	for (auto&& p : discard)
		unmakeNode(p);
}

template <typename T, template <typename...> class A>
template <typename Op>
void TreeVector<T, A>::for_each(Op&& op)
{
	static void (*fa)(Op&, NodeP) = [](Op& op, NodeP p) {
		if (p->sentry())
			return;
		fa(op, p->left());
		op(p->item());
		fa(op, p->right());
	};

	fa(op, head());
}

template <typename T, template <typename...> class A>
std::size_t TreeVector<T, A>::size() const
{
	return head()->weight();
}

template <typename T, template <typename...> class A>
std::size_t TreeVector<T, A>::max_size() const
{
	return std::numeric_limits<int>::max();
}

template <typename T, template <typename...> class A>
bool TreeVector<T, A>::empty() const
{
	return root->left() == nil;
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::helperUnlink(NodeP node)
{
	auto relink = [&](NodeP n1, NodeP n2) {
		if (isLeft(n1))
			linkL(n1->parent(), n2);
		else
			linkR(n1->parent(), n2);
	};

	bool ln = node->left()  == nil;
	bool rn = node->right() == nil;

	if (ln && rn)
	{
		relink(node, nil);
		treeBalance(node->parent());
	} else if (ln)
	{
		relink(node, node->right());
		treeBalance(node->parent());
	} else if (rn)
	{
		relink(node, node->left());
		treeBalance(node->parent());
	} else
	{
		NodeP succ = nextNode(node);

		assert(succ->left() == nil);

		if (succ->parent() == node)
		{
			relink(node, succ);
			linkL(succ, node->left());
			treeBalance(succ);
		} else
		{
			linkL(succ->parent(), succ->right());
			treeBalance(succ->parent());
			relink(node, succ);
			linkL(succ, node->left());
			linkR(succ, node->right());
		}
		fixHW(succ);
	}
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::treeDeleteNode(NodeP node)
{
	helperUnlink(node);
	unmakeNode(node);
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::treeInsert(NodeP where, NodeP newnode)
{
	newnode->weight() = newnode->height() = 1;
	newnode->left() = newnode->right() = nil;

	if (where->left() == nil)
	{
		where->left() = newnode;
		newnode->parent() = where;
		treeBalance(where);
		return;
	}
	where = where->left();
	while (where->right() != nil)
		where = where->right();

	where->right() = newnode;
	newnode->parent() = where;
	treeBalance(where);
}

template <typename T, template <typename...> class A>
void TreeVector<T, A>::treeBalance(NodeP node)
{
	while (node != root)
	{
		fixHW(node);

		int balance = node->balance();

		assert((balance >= -2) && (balance <= +2));

		if (balance <= -2)
		{
			int lb = node->left()->balance();
			if (lb == -1)
				treeRotateRight(node);
			else
				treeRotateLeftRight(node);
		}
		else if (balance >= +2)
		{
			int rb = node->right()->balance();
			if (rb == +1)
				treeRotateLeft(node);
			else
				treeRotateRightLeft(node);
		}

		node = node->parent();
		if (node == nil)
			break;
	}
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::treeRotateLeft(NodeP node) -> NodeP
{
	NodeP right     = node->right();
	NodeP rightLeft = right->left();
	NodeP parent    = node->parent();

	if (isLeft(node))
		linkL(parent, right);
	else
		linkR(parent, right);

	linkL(right, node);
	linkR(node, rightLeft);

	fixHW(node);
	fixHW(right);

	return right;
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::treeRotateRight(NodeP node) -> NodeP
{
	NodeP left      = node->left();
	NodeP leftRight = left->right();
	NodeP parent    = node->parent();

	if (isLeft(node))
		linkL(parent, left);
	else
		linkR(parent, left);

	linkR(left, node);
	linkL(node, leftRight);

	fixHW(node);
	fixHW(left);

	return left;
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::treeRotateLeftRight(NodeP node) -> NodeP
{
	linkL(node, treeRotateLeft(node->left()));
	return treeRotateRight(node);
}

template <typename T, template <typename...> class A>
auto TreeVector<T, A>::treeRotateRightLeft(NodeP node) -> NodeP
{
	linkR(node, treeRotateRight(node->right()));
	return treeRotateLeft(node);
}

#ifndef NDEBUG

template <typename T, template <typename...> class A>
bool TreeVector<T, A>::integrity(NodeP node) const
{
	if (node == nil)
		return true;
	if (node == root)
		return false;

	bool amleft  = node->parent()->left() == node;
	bool amright = node->parent()->right() == node;

	if (node->weight() < 1)
		return false;
	if (node->height() < 1)
		return false;

	if (amleft == amright)
		return false;

	int lh = node->left()->height();
	int rh = node->right()->height();

	if (std::abs(lh - rh) > 1)
		return false;

	int lw = node->left()->weight();
	int rw = node->right()->weight();

	int h = std::max(lh, rh) + 1;
	int w = lw + rw + 1;

	if (h != node->height())
		return false;
	if (w != node->weight())
		return false;

	bool lrn = (node->left() == nil) && (node->right() == nil);

	if ((w == 1) || (h == 1) || lrn)
	{
		if (w != 1)
			return false;
		if (h != 1)
			return false;
		if (!lrn)
			return false;
	}

	if (node->left() != nil)
	{
		if (node->left()->parent() != node)
			return false;
		if (!integrity(node->left()))
			return false;
	}

	if (node->right() != nil)
	{
		if (node->right()->parent() != node)
			return false;
		if (!integrity(node->right()))
			return false;
	}

	return true;
}

template <typename T, template <typename...> class A>
bool TreeVector<T, A>::integrity() const
{
	if (!nil)
		return false;
	if (nil->height() != 0)
		return false;
	if (nil->weight() != 0)
		return false;
	if (nil->left() != nil)
		return false;
	if (nil->right() != nil)
		return false;

	if (!root)
		return false;
	if (root->parent() != nil)
		return false;
	if (root->right() != nil)
		return false;
	if (root->height() != 0)
		return false;
	if (root->weight() != 0)
		return false;

	if (root->left() != head())
		return false;
	if (!integrity(head()))
		return false;

	int  i = 0, n = size();
	auto iter = begin();
	while (iter != end())
	{
		if (iter != nth(i))
			return false;
		if (index_of(iter) != i)
			return false;
		++i;
		++iter;
	}
	if (i != n)
		return false;

	return true;
}

#endif

template class TreeVector<int>;

template std::ostream& operator<<(std::ostream&, const TreeVector<int>&);

