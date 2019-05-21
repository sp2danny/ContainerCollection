
#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace avl {

namespace detail {
template <typename It>
constexpr bool isRanIt =
    std::is_same<std::random_access_iterator_tag,
                 typename std::iterator_traits<It>::iterator_category>::value;
}

using namespace std::literals;

template <typename T, typename A = std::allocator<T>> class vector {
  struct Node;
  typedef Node *NodeP;
  struct Core;

  struct payload_tag {};
  struct sentry_tag {};

  struct Node {
    Node(sentry_tag) : dummy(0) {}
    template <typename... Args>
    Node(payload_tag, Args &&... args) : item(std::forward<Args>(args)...) {}
    NodeP parent, left, right;
    std::uint32_t weight : 27;
    std::uint32_t height : 5;
    union {
      T item;
      char dummy;
    };
    int balance() const {
      int rh = right->height;
      int lh = left->height;
      return rh - lh;
    }
    void setnil(NodeP nil) {
      parent = left = right = nil;
      weight = height = 0;
    }
    ~Node() {
      if (weight || height) {
        assert(weight && height);
        item.~T();
      }
    }
    bool sentry() const {
      if (weight || height) {
        assert(weight && height);
      }
      return !(weight && height);
    }
  };

  struct Core {
    NodeP root;
    NodeP nil;
  };

  Core core;

  std::size_t internal_indexof(const Node *p) const {
    std::size_t idx = 0;
    idx += p->left->weight;
    while (p != core.root) {
      if (internal_is_right(p)) {
        idx += p->parent->left->weight + 1;
      }
      p = p->parent;
    }
    return idx;
  }

  static void internal_link_r(NodeP par, NodeP r) {
    par->right = r;
    r->parent = par;
  }

  static void internal_link_l(NodeP par, NodeP l) {
    par->left = l;
    l->parent = par;
  }

  static bool internal_is_left(const Node *n) { return n->parent->left == n; }

  static bool internal_is_right(const Node *n) { return n->parent->right == n; }

  /// Insert existing node in the tree, before at
  NodeP internal_insert_node(NodeP at, NodeP newn) {
    if (at->left == core.nil) {
      internal_link_l(at, newn);
      internal_balance(at);
      return newn;
    }
    at = at->left;
    while (at->right != core.nil)
      at = at->right;
    internal_link_r(at, newn);
    internal_balance(at);
    return newn;
  }
  /// Insert new data in the tree, before at
  template <typename... Args> NodeP internal_insert(NodeP at, Args &&... args) {
    return internal_insert_node(at,
                                internal_node_new(std::forward<Args>(args)...));
  }

  NodeP internal_nth(std::size_t idx) {
    if (idx == size())
      return internal_last_node();
    assert(idx < size());

    static NodeP (*fndi)(Core & c, NodeP, std::size_t) =
        [](Core &c, NodeP p, std::size_t idx) -> NodeP {
      assert(p != c.nil);
      assert(p != c.root);
      auto lw = p->left->weight;
      if (lw == idx)
        return p;
      if (idx < lw)
        return fndi(c, p->left, idx);
      assert(idx >= (lw + 1));
      idx -= (lw + 1);
      return fndi(c, p->right, idx);
    };

    return fndi(core, core.root->left, (int)idx);
  }
  const Node *internal_nth(std::size_t idx) const {
    return const_cast<vector *>(this)->internal_nth(idx);
  }

  /// Insert data in its sorted position
  /// Only works if container is sorted (binary search)
  NodeP internal_insert_sorted(const T &data) {
    NodeP node = core.root;
    if (node->left == core.nil) {
      internal_link_l(node, internal_node_new(data));
      internal_balance(node);
      return node;
    }
    node = node->left;
    while (true) {
      if (data < node->item) {
        if (node->left == core.nil) {
          internal_link_l(node, internal_node_new(data));
          internal_balance(node);
          return node;
        }
        node = node->left;
      } else {
        if (node->right == core.nil) {
          internal_link_r(node, internal_node_new(data));
          internal_balance(node);
          return node;
        }
        node = node->right;
      }
    }
  }

  void internal_full_node_swap(NodeP n1, NodeP n2) {
    NodeP new_n1p = n2->parent;
    NodeP new_n1l = n2->left;
    NodeP new_n1r = n2->right;
    NodeP *n1_lnk = internal_is_left(n2) ? &new_n1p->left : &new_n1p->right;

    NodeP new_n2p = n1->parent;
    NodeP new_n2l = n1->left;
    NodeP new_n2r = n1->right;
    NodeP *n2_lnk = internal_is_left(n1) ? &new_n2p->left : &new_n2p->right;

    if (n2->parent == n1) {
      new_n1p = n2;
      n1_lnk = nullptr;
      if (n1->left == n2)
        new_n2l = n1;
      else
        new_n2r = n1;
    } else if (n1->parent == n2) {
      new_n2p = n1;
      n2_lnk = nullptr;
      if (n2->left == n1)
        new_n1l = n2;
      else
        new_n1r = n2;
    }

    n1->parent = new_n1p;
    n1->left = new_n1l;
    n1->left->parent = n1;
    n1->right = new_n1r;
    n1->right->parent = n1;
    if (n1_lnk)
      *n1_lnk = n1;

    n2->parent = new_n2p;
    n2->left = new_n2l;
    n2->left->parent = n2;
    n2->right = new_n2r;
    n2->right->parent = n2;
    if (n2_lnk)
      *n2_lnk = n2;

    uint32_t tmp;

#define SWP(a, b)                                                              \
  tmp = a;                                                                     \
  a = b;                                                                       \
  b = tmp

    SWP(n1->weight, n2->weight);
    SWP(n1->height, n2->height);

#undef SWP
  }

  auto internal_relink(NodeP n1, NodeP n2) {
    if (internal_is_left(n1))
      internal_link_l(n1->parent, n2);
    else
      internal_link_r(n1->parent, n2);
  };

  NodeP internal_unlink_node(NodeP node) {
    bool ln = node->left == core.nil;
    bool rn = node->right == core.nil;

    if (ln && rn) {
      internal_relink(node, core.nil);
      internal_balance(node->parent);
    } else if (ln) {
      internal_relink(node, node->right);
      internal_balance(node->right);
    } else if (rn) {
      internal_relink(node, node->left);
      internal_balance(node->left);
    } else {
      auto bal = node->balance();
      if (bal >= 0) {
        NodeP succ = internal_next_node(node);
        internal_full_node_swap(node, succ);
        return internal_unlink_node(node);
      } else {
        NodeP pred = internal_prev_node(node);
        internal_full_node_swap(node, pred);
        return internal_unlink_node(node);
      }
    }
    return node;
  }

  void internal_destruct_node(NodeP p) {
    p->~Node();
    allocator_type{}.deallocate(p, 1);
  }

  /// Removes a given node from the tree.
  void internal_delete_node(NodeP node) {
    NodeP p = internal_unlink_node(node);
    internal_destruct_node(p);
  }

  /// Searches the tree for a node containing the given data.
  /// Only works if container is sorted (binary search)
  NodeP internal_search_node(const T &what) {
    NodeP node = core.root->left;

    while (node != core.nil) {
      if (what < node->item)
        node = node->left;
      else if (node->item < what)
        node = node->right;
      else
        return node;
    }

    return core.root;
  }

  /// Searches the tree for earliest node larger than data (upper bound)
  NodeP internal_sorted_insert_position(const T &data) {
    NodeP node = core.root->left;
    NodeP lwl = core.root;

    if (node == core.nil)
      return core.root;

    while (true) {
      if (data < node->item) {
        if (node->left == core.nil)
          return node;
        lwl = node;
        node = node->left;
      } else {
        if (node->right == core.nil)
          return lwl;
        node = node->right;
      }
    }
  }

  /// Searches the tree for first node equal to, or first node larger than data
  NodeP internal_lower_bound(const T &data) {
    NodeP node = core.root->left;
    NodeP lwl = core.root;

    if (node == core.nil)
      return core.root;

    while (true) {
      if (data < node->item) {
        lwl = node;
        if (node->left == core.nil)
          break;
        node = node->left;
      } else if (node->item < data) {
        if (node->right == core.nil)
          break;
        node = node->right;
      } else {
        while ((node->left != core.nil) && (node->left->item == data))
          node = node->left;
        return node;
      }
    }
    return lwl;
  }

  /*
  /// Searches the tree for all nodes equal to data, return range
  std::pair<NodeP,NodeP> internal_equal_range(const T& data)
  {
                                                                  NodeP node =
  core.root->left;
                                                                  NodeP
  last_lower = nullptr;
                                                                  NodeP
  first_bigger = nullptr;

                                                                  if (node ==
  core.nil)
                                                                                                                                  return
  {core.root, core.root};

                                                                  while (true)
                                                                  {
                                                                                                                                  if (node->item
  < data)
                                                                                                                                  {
                                                                                                                                                                                                  if (!last_lower)
                                                                                                                                                                                                                                                                  last_lower =
  node;
                                                                                                                                                                                                  else if
  (last_lower->item < node->item) last_lower = node; if (node->left == core.nil)
  break; node = node->left;
                                                                                                                                  }
                                                                                                                                  else if
  (node->item < data)
                                                                                                                                  {
                                                                                                                                                                                                  if (!first_bigger)
                                                                                                                                                                                                                                                                  first_bigger =
  node;
                                                                                                                                                                                                  else if (node->item
  < first_bigger->item) first_bigger = node; if (node->right == core.nil) break;
  node = node->right;
                                                                                                                                  }
                                                                                                                                  else
                                                                                                                                  {
                                                                                                                                                                                                  break;
                                                                                                                                  }
                                                                  }
                                                                  if
  (!last_lower)
                                                                                                                                  last_lower =
  internal_first_node();
                                                                  if
  (!first_bigger)
                                                                                                                                  first_bigger =
  internal_last_node();
                                                                  while (true)
                                                                  {
                                                                                                                                  if
  (last_lower->item < data)
                                                                                                                                                                                                  last_lower =
  internal_next_node(last_lower);
                                                                                                                                  else
                                                                                                                                                                                                  break;
                                                                  }
                                                                  while (true)
                                                                  {
                                                                                                                                  auto p =
  internal_prev_node(first_bigger);
                                                                                                                                  if (!(p->item
  < data))
                                                                                                                                                                                                  first_bigger = p;
                                                                                                                                  else
                                                                                                                                                                                                  break;
                                                                  }

                                                                  return
  {last_lower, first_bigger};
  }
  */

  /// Returns the first node, or sentry.
  NodeP internal_first_node() {
    NodeP node = core.root->left;

    if (node == core.nil)
      return core.root;

    while (node->left != core.nil)
      node = node->left;

    return node;
  }

  /// Returns the node after the last. (end marker)
  NodeP internal_last_node() { return core.root; }

  /// Returns the last actual node
  NodeP internal_last_payload_node() {
    if (empty())
      return core.root;
    NodeP n = core.root->left;
    assert(n && n != core.nil);
    while (n->right != core.nil)
      n = n->right;
    return n;
  }

  /// Returns the predecessor of the given node.
  NodeP internal_prev_node(NodeP n) {
    if (n == core.root) {
      n = n->left;
      while (n->right != core.nil)
        n = n->right;
      return n;
    }

    NodeP nTemp;

    if (n->left != core.nil) {
      n = n->left;
      while (n->right != core.nil)
        n = n->right;
    } else {
      nTemp = n;
      n = n->parent;
      while ((n != core.root) && (n->left == nTemp)) {
        nTemp = n;
        n = n->parent;
      }
    }
    return n;
  }

  /// Returns the successor of the given node.
  NodeP internal_next_node(NodeP n) {
    if (n == core.root) {
      while (n->left != core.nil)
        n = n->left;
      return n;
    }

    Node *nTemp;

    if (n->right != core.nil) {
      n = n->right;
      while (n->left != core.nil)
        n = n->left;
    } else {
      nTemp = n;
      n = n->parent;
      while ((n != core.nil) && (n->right == nTemp)) {
        nTemp = n;
        n = n->parent;
      }
    }

    return n;
  }

  void internal_balance(NodeP node) {
    while (node != core.root) {
      NodeP n, par = node->parent;

      internal_updHW(node);

      int balance = node->balance();

      assert((balance >= -2) && (balance <= +2));

      if (balance <= -2) {
        par = node->left;
        int lb = node->left->balance();
        if (lb <= -1)
          n = internal_rotate_right(node);
        else
          n = internal_rotate_left_right(node);
      } else if (balance >= +2) {
        par = node->right;
        int rb = node->right->balance();
        if (rb >= +1)
          n = internal_rotate_left(node);
        else
          n = internal_rotate_right_left(node);
      }
      node = par;
      assert(node != core.nil);
    }
  }

  NodeP internal_rotate_left(NodeP node) {
    NodeP right = node->right;
    NodeP rightLeft = right->left;

    internal_relink(node, right);
    internal_link_l(right, node);
    internal_link_r(node, rightLeft);

    internal_updHW(node);

    return right;
  }

  NodeP internal_rotate_right(NodeP node) {
    NodeP left = node->left;
    NodeP leftRight = left->right;

    internal_relink(node, left);
    internal_link_r(left, node);
    internal_link_l(node, leftRight);

    internal_updHW(node);

    return left;
  }

  NodeP internal_rotate_left_right(NodeP node) {
    internal_link_l(node, internal_rotate_left(node->left));
    return internal_rotate_right(node);
  }

  NodeP internal_rotate_right_left(NodeP node) {
    internal_link_r(node, internal_rotate_right(node->right));
    return internal_rotate_left(node);
  }

  static void internal_updHW(NodeP node) {
    node->height = std::max(node->left->height, node->right->height) + 1;
    node->weight = node->left->weight + node->right->weight + 1;
  }

  struct SR // SortResult
  {
    T *min;
    T *max;
    bool sorted;
  };

  SR internal_is_sub_sorted(NodeP node) const {
    assert(!node->sentry());
    bool ln = node->left == core.nil;
    bool rn = node->right == core.nil;

    SR result{&node->item, &node->item, true};

    if (ln && rn) {
      return result;
    }

    if (result.sorted && !ln) {
      SR lft = internal_is_sub_sorted(node->left);
      if (!lft.sorted)
        result.sorted = false;
      if (node->item < *lft.max)
        result.sorted = false;
      result.min = lft.min;
    }

    if (result.sorted && !rn) {
      SR rgt = internal_is_sub_sorted(node->right);
      if (!rgt.sorted)
        result.sorted = false;
      if (*rgt.min < node->item)
        result.sorted = false;
      result.max = rgt.max;
    }

    return result;
  }

  template <typename... Args> NodeP internal_node_new(Args &&... args) {
    NodeP p = allocator_type{}.allocate(1);
    new (p) Node(payload_tag{}, std::forward<Args>(args)...);
    p->left = p->right = core.nil;
    p->weight = p->height = 1;
    return p;
  }

  bool internal_integrity(NodeP node) const {
    if (node == core.nil)
      return true;

    long lh = node->left->height;
    long rh = node->right->height;

    if (std::abs(lh - rh) > 1)
      return false;

    long h = std::max(lh, rh) + 1;
    long w = node->left->weight + node->right->weight + 1;

    if (h != node->height)
      return false;
    if (w != node->weight)
      return false;

    bool lrn = (node->left == core.nil) && (node->right == core.nil);

    if ((w == 1) || (h == 1) || lrn) {
      if ((w * h) != 1)
        return false;
      if (!lrn)
        return false;
    }

    if (node->left != core.nil) {
      if (node->left->parent != node)
        return false;
      if (!internal_integrity(node->left))
        return false;
    }

    if (node->right != core.nil) {
      if (node->right->parent != node)
        return false;
      if (!internal_integrity(node->right))
        return false;
    }

    return true;
  }

  struct Trunk {
    Trunk *prev;
    std::string str;
    bool pop;
  };

  std::string internal_print_trunks(Trunk *p) const {
    if (!p) {
      return "";
    }
    auto str = internal_print_trunks(p->prev);
    if (p->pop) {
      assert(!str.empty());
      assert(str.back() == ' ');
      str.pop_back();
    }
    str += p->str;
    return str;
  }

  void internal_print_tree(std::ostream &out, bool pp, NodeP n, Trunk *prev,
                           bool is_left, bool utf8 = false) const {
    if (n == core.nil)
      return;

    Trunk this_disp = {prev, "    ", false};

    std::string prev_str = this_disp.str;
    internal_print_tree(out, pp, n->right, &this_disp, true, utf8);

    static auto HL = "\xe2\x94\x80"s;
    static auto VL = "\xe2\x94\x82"s;
    static auto UL = "\xe2\x95\xad"s;
    static auto LL = "\xe2\x95\xb0"s;
    static auto SQ = "\xe2\x97\x87"s;

    if (!prev) {
      if (utf8) {
        this_disp.str = HL + HL;
      } else
        this_disp.str = "--";
    } else if (is_left) {
      if (utf8) {
        this_disp.pop = true;
        this_disp.str = UL + HL + HL;
      } else
        this_disp.str = ".-";
      prev_str = "   ";
      if (utf8)
        prev_str += VL;
      else
        prev_str += "|";
    } else {
      if (utf8) {
        this_disp.pop = true;
        this_disp.str = LL + HL + HL;
      } else
        this_disp.str = "`-";
      prev->str = prev_str;
      prev->pop = false;
    }

    out << internal_print_trunks(&this_disp);
    if (utf8) {
      out << HL << SQ << n->item;
    } else {
      out << " " << n->item;
    }
    {
      if (pp)
        out << "{0x" << std::hex << ((intptr_t)n) << std::dec << "} ";
      out << "\n";
    }

    if (prev) {
      prev->str = prev_str;
      prev->pop = false;
    }

    this_disp.str = "   ";
    if (utf8) {
      this_disp.str += VL;
    } else
      this_disp.str += "|";
    this_disp.pop = false;

    internal_print_tree(out, pp, n->left, &this_disp, false, utf8);
  }

  typedef std::vector<NodeP> VNP;

  NodeP internal_hang(NodeP *ap, NodeP *bp) {
    assert(ap <= bp);
    auto sz = bp - ap;
    if (!sz)
      return core.nil;
    if (sz == 1) {
      NodeP n = *ap;
      n->weight = n->height = 1;
      n->left = n->right = core.nil;
      return n;
    }
    auto center = sz / 2;
    NodeP *cp = ap + center;
    internal_link_l(*cp, internal_hang(ap, cp));
    internal_link_r(*cp, internal_hang(cp + 1, bp));
    internal_updHW(*cp);
    assert(internal_integrity(*cp));
    return *cp;
  }
  NodeP internal_hang(VNP &vnp) {
    NodeP *ptr = vnp.data();
    std::size_t sz = vnp.size();
    return internal_hang(ptr, ptr + sz);
  }
  std::size_t internal_flatten(VNP &vnp) {
    vnp.clear();
    auto sz = core.root->left->weight;
    vnp.reserve(sz);
    NodeP n = internal_first_node();
    NodeP e = internal_last_node();
    while (n != e) {
      vnp.push_back(n);
      n = internal_next_node(n);
    }
    assert(sz == vnp.size());
    return sz;
  }
  std::size_t internal_flatten_insert(VNP &target, NodeP breakp,
                                      VNP &inserted) {
    target.clear();
    auto sz = core.root->left->weight + inserted.size();
    target.reserve(sz);
    NodeP n = internal_first_node();
    NodeP e = internal_last_node();
    while (true) {
      if (n == breakp)
        target.insert(target.end(), inserted.begin(), inserted.end());
      if (n == e)
        break;
      target.push_back(n);
      n = internal_next_node(n);
    }
    assert(sz == target.size());
    return sz;
  }

  void internal_insert_range(NodeP n, VNP &vnp) {
    if (size() > vnp.size()) {
      for (NodeP p : vnp) {
        n = internal_insert_node(n, p);
        n = internal_next_node(n);
      }
    } else {
      VNP vnp_new;
      internal_flatten_insert(vnp_new, n, vnp);
      internal_link_l(core.root, internal_hang(vnp_new));
    }
  }

  template <typename Op = std::less<T>>
  static int item_compare(const T &v1, const T &v2, Op op = Op{}) {
    /**/ if (op(v1, v2))
      return -1;
    else if (op(v2, v1))
      return +1;
    else
      return 0;
  }

  mutable vector *me = this;

public:
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef T value_type;
  typedef T &reference;
  typedef const T &const_reference;
  typedef T *pointer;
  typedef const T *const_pointer;
  struct iterator;
  struct const_iterator;
  typedef typename std::allocator_traits<A>::template rebind_alloc<Node>
      allocator_type;

  friend struct iterator;

  /// Creates a new empty tree.
  vector() {
    NodeP p = allocator_type{}.allocate(2);
    core.root = new (p + 0) Node{sentry_tag{}};
    core.nil = new (p + 1) Node{sentry_tag{}};
    core.root->setnil(core.nil);
    core.nil->setnil(core.nil);
  }
  template <typename It> vector(It b, It e) : vector() {
    VNP vpn;
    if
      constexpr(detail::isRanIt<It>) {
        auto sz = e - b;
        vpn.reserve(sz);
      }
    while (b != e)
      vpn.push_back(internal_node_new(*b++));
    internal_link_l(core.root, internal_hang(vpn));
  }
  vector(std::initializer_list<T> il) : vector(il.begin(), il.end()) {}
  vector(std::size_t sz, const T &val) : vector() {
    VNP vpn;
    vpn.reserve(sz);
    while (sz--)
      vpn.push_back(internal_node_new(val));
    internal_link_l(core.root, internal_hang(vpn));
  }
  vector(const vector &other) : vector(other.begin(), other.end()) {}
  vector(vector &&other) : vector() { swap(other); }
  vector &operator=(const vector &other) {
    assign(other.begin(), other.end());
    return *this;
  }
  vector &operator=(std::initializer_list<T> il) {
    assign(il.begin(), il.end());
    return *this;
  }
  template <typename It> void assign(It b, It e) {
    clear();
    VNP vnp;
    if
      constexpr(detail::isRanIt<It>) { vnp.reserve(e - b); }
    while (b != e)
      vnp.push_back(internal_node_new(*b++));
    internal_link_l(core.root, internal_hang(vnp));
  }
  void assign(std::initializer_list<T> il) { assign(il.begin(), il.end()); }
  void assign(std::size_t n, const T &val) {
    VNP vnp;
    vnp.reserve(n);
    while (n--)
      vnp.push_back(internal_node_new(val));
    internal_link_l(core.root, internal_hang(vnp));
  }
  vector &operator=(vector &&other) noexcept {
    swap(other);
    return *this;
  }
  void swap(vector &other) noexcept {
    using std::swap;
    swap(core.root, other.core.root);
    swap(core.nil, other.core.nil);
  }
  std::size_t size() const { return core.root->left->weight; }

  void resize(std::size_t sz, const T &val = T{}) {
    if (sz == size())
      return;
    VNP vnp;
    vnp.reserve(sz);
    internal_flatten(vnp);
    while (sz < vnp.size()) {
      internal_delete_node(vnp.back());
      vnp.pop_back();
    }
    while (sz > vnp.size()) {
      vnp.push_back(internal_node_new(val));
    }
    internal_link_l(core.root, internal_hang(vnp));
    assert(size() == sz);
  }

  bool empty() const { return core.root->left == core.nil; }

  bool is_sorted() const {
    if (size() <= 1)
      return true;
    return internal_is_sub_sorted(core.root->left).sorted;
  }

  void clear() {
    static void (*rec_clr)(vector &, NodeP);
    rec_clr = [](vector &me, NodeP node) {
      if (node == me.core.nil)
        return;
      rec_clr(me, node->left);
      rec_clr(me, node->right);
      me.internal_destruct_node(node);
    };

    rec_clr(*this, core.root->left);
    core.root->left = core.nil;
  }

  ~vector() {
    clear();
    assert(core.root->sentry() && core.nil->sentry());
    allocator_type{}.deallocate(core.root, 2);
    core.root = core.nil = nullptr;
  }

  bool integrity() const {
    if (!core.nil)
      return false;
    if (core.nil->height != 0)
      return false;
    if (core.nil->weight != 0)
      return false;

    if (core.nil->left != core.nil)
      return false;
    if (core.nil->right != core.nil)
      return false;

    if (!core.root)
      return false;
    if (core.root->parent != core.nil)
      return false;
    if (core.root->right != core.nil)
      return false;

    return internal_integrity(core.root->left);
  }

  void print_tree(std::ostream &out, bool printpointer = false,
                  bool utf8 = false) const {
    internal_print_tree(out, printpointer, core.root->left, nullptr, true,
                        utf8);
  }

  struct iterator {
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef T *pointer;
    typedef T &reference;
    typedef std::ptrdiff_t difference_type;
    iterator() = default;
    T &operator*() const { return node->item; }
    T *operator->() const { return &node->item; }
    iterator &operator++() {
      node = avp->internal_next_node(node);
      return *this;
    }
    iterator &operator--() {
      node = avp->internal_prev_node(node);
      return *this;
    }
    iterator operator++(int) {
      auto tmp = *this;
      node = avp->internal_next_node(node);
      return tmp;
    }
    iterator operator--(int) {
      auto tmp = *this;
      node = avp->internal_prev_node(node);
      return tmp;
    }
    bool operator==(const iterator &other) const {
      assert(avp == other.avp);
      return node == other.node;
    }
    bool operator!=(const iterator &other) const {
      assert(avp == other.avp);
      return node != other.node;
    }
    bool operator<(const iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(node) < avp->internal_indexof(other.node);
    }
    bool operator<=(const iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(node) <= avp->internal_indexof(other.node);
    }
    bool operator>(const iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(node) > avp->internal_indexof(other.node);
    }
    bool operator>=(const iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(node) >= avp->internal_indexof(other.node);
    }
    std::ptrdiff_t operator-(const iterator &other) const {
      return std::ptrdiff_t(avp->internal_indexof(node)) -
             std::ptrdiff_t(avp->internal_indexof(other.node));
    }
    iterator &operator+=(std::ptrdiff_t ofs) {
      node = avp->internal_nth(avp->internal_indexof(node) + ofs);
      return *this;
    }
    iterator &operator-=(std::ptrdiff_t ofs) {
      node = avp->internal_nth(avp->internal_indexof(node) - ofs);
      return *this;
    }
    iterator operator+(std::ptrdiff_t ofs) const {
      iterator tmp = *this;
      tmp += ofs;
      return tmp;
    }
    iterator operator-(std::ptrdiff_t ofs) const {
      iterator tmp = *this;
      tmp -= ofs;
      return tmp;
    }
    friend class vector;
    friend struct const_iterator;

  private:
    iterator(vector *avp, Node *node) : avp(avp), node(node) {}
    vector *avp = nullptr;
    Node *node = nullptr;
  };

  struct const_iterator {
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef const T value_type;
    typedef const T *pointer;
    typedef const T &reference;
    typedef std::ptrdiff_t difference_type;
    const_iterator() = default;
    const_iterator(iterator i) : avp(i.avp), node(i.node) {}
    const T &operator*() const { return node->item; }
    const T *operator->() const { return &node->item; }
    const_iterator &operator++() {
      node = avp->internal_next_node(node);
      return *this;
    }
    const_iterator &operator--() {
      node = avp->internal_next_node(node);
      return *this;
    }
    const_iterator operator++(int) {
      auto tmp = *this;
      node = avp->internal_next_node(node);
      return tmp;
    }
    const_iterator operator--(int) {
      auto tmp = *this;
      node = avp->internal_next_node(node);
      return tmp;
    }
    bool operator==(const const_iterator &other) const {
      assert(avp == other.avp);
      return node == other.node;
    }
    bool operator!=(const const_iterator &other) const {
      assert(avp == other.avp);
      return node != other.node;
    }
    bool operator<(const const_iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(node) < avp->internal_indexof(other.node);
    }
    bool operator<=(const const_iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(node) <= avp->internal_indexof(other.node);
    }
    bool operator>(const const_iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(node) > avp->internal_indexof(other.node);
    }
    bool operator>=(const const_iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(node) >= avp->internal_indexof(other.node);
    }
    std::ptrdiff_t operator-(const const_iterator &other) const {
      return std::ptrdiff_t(avp->internal_indexof(node)) -
             std::ptrdiff_t(avp->internal_indexof(other.node));
    }
    const_iterator &operator+=(std::ptrdiff_t ofs) {
      node = avp->internal_nth(avp->internal_indexof(node) + ofs);
      return *this;
    }
    const_iterator &operator-=(std::ptrdiff_t ofs) {
      node = avp->internal_nth(avp->internal_indexof(node) - ofs);
      return *this;
    }
    const_iterator operator+(std::ptrdiff_t ofs) const {
      const_iterator tmp = *this;
      tmp += ofs;
      return tmp;
    }
    const_iterator operator-(std::ptrdiff_t ofs) const {
      const_iterator tmp = *this;
      tmp -= ofs;
      return tmp;
    }
    friend class vector;

  private:
    const_iterator(vector *avp, Node *node) : avp(avp), node(node) {}
    vector *avp = nullptr;
    Node *node = nullptr;
  };

  struct reverse_iterator {
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef T *pointer;
    typedef T &reference;
    typedef std::ptrdiff_t difference_type;
    reverse_iterator() = default;
    T &operator*() const { return node->item; }
    T *operator->() const { return &node->item; }
    reverse_iterator &operator++() {
      node = avp->internal_prev_node(node);
      return *this;
    }
    reverse_iterator &operator--() {
      node = avp->internal_next_node(node);
      return *this;
    }
    reverse_iterator operator++(int) {
      auto tmp = *this;
      node = avp->internal_prev_node(node);
      return tmp;
    }
    reverse_iterator operator--(int) {
      auto tmp = *this;
      node = avp->internal_next_node(node);
      return tmp;
    }
    bool operator==(const reverse_iterator &other) const {
      assert(avp == other.avp);
      return node == other.node;
    }
    bool operator!=(const reverse_iterator &other) const {
      assert(avp == other.avp);
      return node != other.node;
    }
    bool operator<(const reverse_iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(other.node) < avp->internal_indexof(node);
    }
    bool operator<=(const reverse_iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(other.node) <= avp->internal_indexof(node);
    }
    bool operator>(const reverse_iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(other.node) > avp->internal_indexof(node);
    }
    bool operator>=(const reverse_iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(other.node) >= avp->internal_indexof(node);
    }
    std::ptrdiff_t operator-(const reverse_iterator &other) const {
      return std::ptrdiff_t(avp->internal_indexof(other.node)) -
             std::ptrdiff_t(avp->internal_indexof(node));
    }
    reverse_iterator &operator+=(std::ptrdiff_t ofs) {
      node = avp->internal_nth(avp->internal_indexof(node) - ofs);
      return *this;
    }
    reverse_iterator &operator-=(std::ptrdiff_t ofs) {
      node = avp->internal_nth(avp->internal_indexof(node) + ofs);
      return *this;
    }
    reverse_iterator operator+(std::ptrdiff_t ofs) const {
      reverse_iterator tmp = *this;
      tmp -= ofs;
      return tmp;
    }
    reverse_iterator operator-(std::ptrdiff_t ofs) const {
      reverse_iterator tmp = *this;
      tmp += ofs;
      return tmp;
    }
    friend class vector;
    friend struct const_reverse_iterator;

  private:
    reverse_iterator(vector *avp, Node *node) : avp(avp), node(node) {}
    vector *avp = nullptr;
    Node *node = nullptr;
  };

  struct const_reverse_iterator {
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef const T value_type;
    typedef const T *pointer;
    typedef const T &reference;
    typedef std::ptrdiff_t difference_type;
    const_reverse_iterator() = default;
    const_reverse_iterator(reverse_iterator i) : avp(i.avp), node(i.node) {}
    const T &operator*() const { return node->item; }
    const T *operator->() const { return &node->item; }
    const_reverse_iterator &operator++() {
      node = avp->internal_prev_node(node);
      return *this;
    }
    const_reverse_iterator &operator--() {
      node = avp->internal_next_node(node);
      return *this;
    }
    const_reverse_iterator operator++(int) {
      auto tmp = *this;
      node = avp->internal_prev_node(node);
      return tmp;
    }
    const_reverse_iterator operator--(int) {
      auto tmp = *this;
      node = avp->internal_next_node(node);
      return tmp;
    }
    bool operator==(const const_reverse_iterator &other) const {
      assert(avp == other.avp);
      return node == other.node;
    }
    bool operator!=(const const_reverse_iterator &other) const {
      assert(avp == other.avp);
      return node != other.node;
    }
    bool operator<(const const_reverse_iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(other.node) < avp->internal_indexof(node);
    }
    bool operator<=(const const_reverse_iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(other.node) <= avp->internal_indexof(node);
    }
    bool operator>(const const_reverse_iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(other.node) > avp->internal_indexof(node);
    }
    bool operator>=(const const_reverse_iterator &other) const {
      assert(avp == other.avp);
      return avp->internal_indexof(other.node) >= avp->internal_indexof(node);
    }
    std::ptrdiff_t operator-(const const_reverse_iterator &other) const {
      return std::ptrdiff_t(avp->internal_indexof(other.node)) -
             std::ptrdiff_t(avp->internal_indexof(node));
    }
    const_reverse_iterator &operator+=(std::ptrdiff_t ofs) {
      node = avp->internal_nth(avp->internal_indexof(node) - ofs);
      return *this;
    }
    const_reverse_iterator &operator-=(std::ptrdiff_t ofs) {
      node = avp->internal_nth(avp->internal_indexof(node) + ofs);
      return *this;
    }
    const_reverse_iterator operator+(std::ptrdiff_t ofs) const {
      const_reverse_iterator tmp = *this;
      tmp -= ofs;
      return tmp;
    }
    const_reverse_iterator operator-(std::ptrdiff_t ofs) const {
      const_reverse_iterator tmp = *this;
      tmp += ofs;
      return tmp;
    }
    friend class vector;

  private:
    const_reverse_iterator(vector *avp, Node *node) : avp(avp), node(node) {}
    vector *avp = nullptr;
    Node *node = nullptr;
  };

  iterator begin() { return {this, internal_first_node()}; }
  iterator end() { return {this, internal_last_node()}; }

  const_iterator begin() const { return {me, me->internal_first_node()}; }
  const_iterator end() const { return {me, me->internal_last_node()}; }
  const_iterator cbegin() const { return begin(); }
  const_iterator cend() const { return end(); }

  reverse_iterator rbegin() { return {this, internal_last_payload_node()}; }
  reverse_iterator rend() { return {this, internal_last_node()}; }

  const_reverse_iterator rbegin() const {
    return {me, me->internal_last_payload_node()};
  }
  const_reverse_iterator rend() const { return {me, me->internal_last_node()}; }
  const_reverse_iterator crbegin() const { return rbegin(); }
  const_reverse_iterator crend() const { return rend(); }

  iterator nth(std::size_t idx) { return {this, internal_nth(idx)}; }
  const_iterator nth(std::size_t idx) const {
    return {me, me->internal_nth(idx)};
  }

  iterator insert(iterator itr, const T &item) {
    auto p = internal_insert(itr.node, item);
    return {this, p};
  }
  template <typename It> void insert(iterator itr, It b, It e) {
    VNP vnp;
    if
      constexpr(detail::isRanIt<It>) { vnp.reserve(e - b); }
    while (b != e) {
      vnp.push_back(internal_node_new(*b));
      ++b;
    }
    internal_insert_range(itr.node, vnp);
  }
  template <typename... Args> iterator emplace(iterator itr, Args &&... args) {
    auto p = internal_insert(itr.node, std::forward<Args>(args)...);
    return {this, p};
  }
  iterator erase(iterator itr) {
    auto p = internal_next_node(itr.node);
    internal_delete_node(itr.node);
    return {this, p};
  }
  iterator erase(iterator b, iterator e) {
    std::size_t n = e - b;
    std::size_t sz = size();
    if (n > (sz / 2)) {
      auto ix = internal_indexof(b.node);
      VNP vnp;
      internal_flatten(vnp);
      auto p = vnp.begin();
      auto b = p + ix;
      auto e = p + ix + n;
      for (auto i = b; i != e; ++i)
        internal_destruct_node(*i);
      vnp.erase(b, e);
      internal_link_l(core.root, internal_hang(vnp));
      return {this, internal_nth(ix)};
    } else {
      while (b != e) {
        b = erase(b);
      }
    }
    return b;
  }

  T &operator[](std::size_t idx) { return internal_nth(idx)->item; }
  const T &operator[](std::size_t idx) const { return internal_nth(idx)->item; }

  T &at(std::size_t idx) {
    if (idx >= size())
      throw std::out_of_range("index out of range");
    return internal_nth(idx)->item;
  }
  const T &at(std::size_t idx) const {
    if (idx >= size())
      throw std::out_of_range("index out of range");
    return internal_nth(idx)->item;
  }

  T &front() { return internal_first_node()->item; }
  T &back() { return internal_last_payload_node()->item; }
  const T &front() const { return me->internal_first_node()->item; }
  const T &back() const { return me->internal_last_payload_node()->item; }

  T &push_back(const T &item) {
    auto p = internal_insert(internal_last_node(), item);
    return p->item;
  }
  T &push_back(T &&item) {
    auto p = internal_insert(internal_last_node(), std::move(item));
    return p->item;
  }
  T &push_front(const T &item) {
    auto p = internal_insert(internal_first_node(), item);
    return p->item;
  }
  T &push_front(T &&item) {
    auto p = internal_insert(internal_first_node(), std::move(item));
    return p->item;
  }
  template <typename... Args> T &emplace_back(Args &&... args) {
    auto p = internal_insert(internal_last_node(), std::forward<Args>(args)...);
    return p->item;
  }
  template <typename... Args> T &emplace_front(Args &&... args) {
    auto p =
        internal_insert(internal_first_node(), std::forward<Args>(args)...);
    return p->item;
  }

  void pop_back() { internal_delete_node(internal_last_payload_node()); }
  void pop_front() { internal_delete_node(internal_first_node()); }

  template <typename Op = std::less<T>> void sort(Op op = Op{}) {
    VNP vnp;
    internal_flatten(vnp);
    auto nless = [&op](NodeP lhs, NodeP rhs) -> bool {
      return op(lhs->item, rhs->item);
    };
    std::sort(vnp.begin(), vnp.end(), nless);
    internal_link_l(core.root, internal_hang(vnp));
  }
  template <typename Op = std::less<T>> void stable_sort(Op op = Op{}) {
    VNP vnp;
    internal_flatten(vnp);
    auto nless = [&op](NodeP lhs, NodeP rhs) -> bool {
      return op(lhs->item, rhs->item);
    };
    std::stable_sort(vnp.begin(), vnp.end(), nless);
    internal_link_l(core.root, internal_hang(vnp));
  }

  template <typename Op = std::equal_to<T>> void unique(Op op = Op{}) {
    if (size() < 2)
      return;
    VNP vnp;
    internal_flatten(vnp);
    VNP uni, rst;
    auto itr = vnp.begin();
    uni.push_back(*itr++);
    while (itr != vnp.end()) {
      if (op((*itr)->item, uni.back()->item))
        rst.push_back(*itr);
      else
        uni.push_back(*itr);
      ++itr;
    }
    internal_link_l(core.root, internal_hang(uni));
    for (auto &&p : rst)
      internal_destruct_node(p);
  }

  void reverse() {
    VNP vnp;
    internal_flatten(vnp);
    std::reverse(vnp.begin(), vnp.end());
    internal_link_l(core.root, internal_hang(vnp));
  }

  template <typename Op = std::less<T>>
  void merge(vector &other, Op op = Op{}) {
    VNP me, ot, mrg;
    internal_flatten(me);
    other.internal_flatten(ot);
    mrg.reserve(me.size() + ot.size());
    auto cmp = [&op](NodeP lhs, NodeP rhs) -> bool {
      return op(lhs->item, rhs->item);
    };
    std::merge(me.begin(), me.end(), ot.begin(), ot.end(),
               std::back_inserter(mrg), cmp);
    other.internal_link_l(other.core.root, other.core.nil);
    internal_link_l(core.root, internal_hang(mrg));
  }

  void reserve(std::size_t) {}
  void shrink_to_fit() {}
  std::size_t capacity() const { return max_size(); }
  std::size_t max_size() const { return (1ul << 27) - 1ul; }

  std::size_t remove(const T &value) {
    auto op = [&value](const T &itm) { return itm == value; };
    return remove_if(op);
  }
  template <typename Op> std::size_t remove_if(Op op) {
    std::size_t cnt = 0;
    NodeP n = internal_first_node();
    NodeP e = internal_last_node();
    while (n != e) {
      NodeP t = internal_next_node(n);
      if (op(n->item)) {
        internal_delete_node(n);
        ++cnt;
      }
      n = t;
    }
    return cnt;
  }
  template <typename Op> std::size_t remove_if_many(Op op) {
    std::size_t sz = size();
    NodeP n = internal_first_node();
    NodeP e = internal_last_node();
    VNP keep, discard;
    keep.reserve(sz);
    discard.reserve(sz);
    while (n != e) {
      if (op(n->item))
        discard.push_back(n);
      else
        keep.push_back(n);
      n = internal_next_node(n);
    }
    for (auto x : discard)
      internal_destruct_node(x);
    AVL_link_l(core.root, internal_hang(keep));
    return discard.size();
  }

  void splice(iterator pos, vector &other) {
    VNP vnp_me, vnp_ot;

    other.internal_flatten(vnp_ot);
    other.core.root->left = other.core.nil;
    internal_flatten_insert(vnp_me, pos.node, vnp_ot);
    internal_link_l(core.root, internal_hang(vnp_me));
  }
  void splice(iterator pos, vector &&other) { splice(pos, other); }

  void splice(iterator pos, vector &other, iterator it) {
    NodeP n = other.internal_unlink_node(it.node);
    internal_insert_node(pos.node, n);
  }
  void splice(iterator pos, vector &&other, iterator it) {
    splice(pos, other, it);
  }

  void splice(iterator pos, vector &other, iterator ot_beg, iterator ot_end) {
    VNP me, ot, targ, rest;
    internal_flatten(me);
    other.internal_flatten(ot);

    auto in_sz = me.size() + ot.size();
    targ.reserve(in_sz);
    rest.reserve(in_sz);

    auto app_here = [&]() {
      bool in = false;
      for (auto &&q : ot) {
        if (q == ot_beg.node)
          in = true;
        if (q == ot_end.node)
          in = false;
        if (in)
          targ.push_back(q);
        else
          rest.push_back(q);
      }
    };

    if (pos == end()) {
      me.swap(targ);
      app_here();
    } else
      for (auto &&p : me) {
        if (p == pos.node) {
          app_here();
        }
        targ.push_back(p);
      }

    assert((targ.size() + rest.size()) == in_sz);

    internal_link_l(core.root, internal_hang(targ));
    internal_link_l(other.core.root, other.internal_hang(rest));
  }

  void splice(iterator pos, vector &&other, iterator ot_beg, iterator ot_end) {
    splice(pos, other, ot_beg, ot_end);
  }

  /// stable insert position for sorted containers
  iterator upper_bound(const T &val) {
    return {this, internal_sorted_insert_position(val)};
  }
  /// lower_bound
  iterator lower_bound(const T &val) {
    return {this, internal_lower_bound(val)};
  }
  /// return first found item == val, or end
  iterator binary_find(const T &val) {
    return {this, internal_search_node(val)};
  }
  bool binary_search(const T &val) const {
    return me->internal_search_node(val) != core.root;
  }

  template <typename Op = std::less<T>>
  int compare(const vector &other, Op op = Op{}) const {
    auto i1 = begin();
    auto e1 = end();
    auto i2 = other.begin();
    auto e2 = other.end();
    while (true) {
      bool ate1 = (i1 == e1);
      bool ate2 = (i2 == e2);
      if (ate1 && ate2)
        return 0;
      if (ate1)
        return -1;
      if (ate2)
        return +1;
      int cmp = item_compare(*i1, *i2, op);
      if (cmp)
        return cmp;
      ++i1;
      ++i2;
    }
  }
};

template <typename T, typename A>
bool operator<(const vector<T, A> &lhs, const vector<T, A> &rhs) {
  return lhs.compare(rhs) < 0;
}

template <typename T, typename A>
bool operator<=(const vector<T, A> &lhs, const vector<T, A> &rhs) {
  return lhs.compare(rhs) <= 0;
}

template <typename T, typename A>
bool operator>(const vector<T, A> &lhs, const vector<T, A> &rhs) {
  return lhs.compare(rhs) > 0;
}

template <typename T, typename A>
bool operator>=(const vector<T, A> &lhs, const vector<T, A> &rhs) {
  return lhs.compare(rhs) >= 0;
}

template <typename T, typename A>
bool operator==(const vector<T, A> &lhs, const vector<T, A> &rhs) {
  if (lhs.size() != rhs.size())
    return false;
  return lhs.compare(rhs) == 0;
}

template <typename T, typename A>
bool operator!=(const vector<T, A> &lhs, const vector<T, A> &rhs) {
  if (lhs.size() != rhs.size())
    return true;
  return lhs.compare(rhs) != 0;
}

template class vector<int>;

} // namespace avl
