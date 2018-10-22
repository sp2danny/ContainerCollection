

template<typename T, template<typename...> class A>
struct TreeVector<T, A>::Trunk
{
	Trunk*      prev;
	const char* str;
};

template<typename T, template<typename...> class A>
template<typename OS>
void TreeVector<T, A>::printTrunks(OS& out, Trunk* p) const
{
	if (!p)
	{
		return;
	}
	printTrunks(out, p->prev);
	out << p->str;
}

template<typename T, template<typename...> class A>
template<typename OS>
void TreeVector<T, A>::printTree(OS& out, NodeP n, Trunk* prev,
                                 bool is_left) const
{
	if (n == nil)
		return;

	Trunk this_disp = {prev, "     "};
	auto  prev_str  = this_disp.str;
	printTree(out, n->right(), &this_disp, true);

	if (!prev)
	{
		this_disp.str = "---";
	} else if (is_left)
	{
		this_disp.str = ".--";
		prev_str      = "    |";
	} else
	{
		this_disp.str = "`--";
		prev->str     = prev_str;
	}

	printTrunks(out, &this_disp);
	out << " " << n->item();
	out << " (" << std::showpos << n->balance() << std::noshowpos << ","
	    << n->weight() << "," << n->height() << ")\n";

	if (prev)
	{
		prev->str = prev_str;
	}
	this_disp.str = "    |";

	printTree(out, n->left(), &this_disp, false);
	if (!prev)
	{
		out << ("");
	}
}

template<typename T, template<typename...> class A>
struct TreeVector<T, A>::RV
{
	typedef std::vector<std::string> StrVec;
	StrVec                           sv;
	int                              trunk;
};

template<typename T, template<typename...> class A>
std::string TreeVector<T, A>::to_string() const
{
	RV rv;
	toStr(root->left(), rv);
	std::string str;
	for (auto&& x : rv.sv)
		str += x + "\n";
	return str;
}

template<typename T, template<typename...> class A>
void TreeVector<T, A>::toStr(NodeP p, RV& rv) const
{
	if (p == nil)
	{
		rv.sv.clear();
		rv.sv.push_back("-()");
		rv.trunk = 0;
		return;
	}

	bool ln = p->left() == nil;
	bool rn = p->right() == nil;

	RV res, lft, rgt;
	if (!ln)
		toStr(p->left(), lft);
	if (!rn)
		toStr(p->right(), rgt);

	int lsz = lft.sv.size();
	int rsz = rgt.sv.size();

	res.sv.reserve(lsz + 3 + rsz);

	std::string tr = " |  ";
	std::string em = "    ";
	std::string lt = "  / ";
	std::string rt = "  \\ ";

	if (!ln)
	{
		for (int i = 0; i < lft.trunk; ++i)
		{
			res.sv.push_back(em + lft.sv[i]);
		}
		res.sv.push_back("   ." + lft.sv[lft.trunk]);
		if (lft.trunk + 1 < lsz)
			res.sv.push_back(lt + lft.sv[lft.trunk + 1]);
		else
			res.sv.push_back(lt);
		for (int i = lft.trunk + 2; i < lsz; ++i)
		{
			res.sv.push_back(tr + lft.sv[i]);
		}
		res.sv.push_back(tr);
	}

	std::stringstream me;
	me << ((ln && rn) ? "-- <" : "-+-- <");
	me << p->item() << ">";
	if (!(ln && rn))
		me << " w:" << p->weight() << " h:" << p->height()
		   << " b:" << std::showpos << p->balance() << std::noshowpos;
	res.trunk = res.sv.size();
	res.sv.push_back(me.str());

	if (!rn)
	{
		res.sv.push_back(tr);
		for (int i = 0; i < rgt.trunk - 1; ++i)
		{
			res.sv.push_back(tr + rgt.sv[i]);
		}
		if (rgt.trunk - 1 >= 0)
			res.sv.push_back(rt + rgt.sv[rgt.trunk - 1]);
		else
			res.sv.push_back(rt);
		res.sv.push_back("   `" + rgt.sv[rgt.trunk]);
		for (int i = rgt.trunk + 1; i < rsz; ++i)
		{
			res.sv.push_back(em + rgt.sv[i]);
		}
	}

	rv = std::move(res);
}
