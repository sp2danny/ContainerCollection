
#include <string>

#include "hb_map.hpp"

template class hb_map<int, std::string>;

typedef hb_map<int, std::string> ISM;

extern "C" void test_hb_map()
{
	ISM ism;

	ism.insert(3,"helo");
	ism.clear();

	int cnt = 1;
	for (;;++cnt) {
		bool want_print = false;
		int i =  rand() % 1000;
		auto s = std::to_string(i);
		if (rand()%2)
		{
			//printf("trying to insert %d\n", i);
			auto result = ism.insert(i, s);
			if (result.first) {
				//printf("insert succeeded\n");
				want_print = true;
			}
		} else {
			//printf("trying to remove %d\n", i);
			auto res = ism.remove(i);
			if (res) {
				//printf("remove succeeded\n");
				want_print = true;
			}
		}
		bool ok = ism.verify();
		if (!ok)
		{
			printf("tree not ok after %d step\n", cnt);
			break;
		}
		if ((cnt%20000)==0)
		//if (cnt==254)
		//	std::cout << "about to drop the hammer\n";
		//if (cnt>=254)
		//if (want_print)
		{
			printf("new dump, count at %d, size at %zu\n", cnt, ism.size() );
			ism.print(std::cout);
		}
	}


}

