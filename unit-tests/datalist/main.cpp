//
// Created by jelle on 4/17/2021.
//

#include "utils/DataList.hpp"
#include <iostream>
#include <unistd.h>

static void error(int i) {
	std::cout << "Test errored: " << i << std::endl;
	::exit(1);
}

int main() {
	// sublist tests
	{
		utils::DataList l;
		l.add("1234");
		l.add("c67");
		l.add("8");
		l.add("90a1234b56d");

		{ // start midstring to end
			utils::DataList::DataListIterator it = l.find("c");
			utils::DataList out;
			l.subList(out, it, l.endList());
			if (out.substring(out.beginList(), out.endList()) != "c67890a1234b56d") {
				std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
				error(1);
			}
		}
		{ // start midstring to end midstring
			utils::DataList::DataListIterator it = l.find("a");
			utils::DataList::DataListIterator itLast = l.find("b");
			utils::DataList out;
			l.subList(out, it, itLast);
			if (out.substring(out.beginList(), out.endList()) != "a1234") {
				std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
				error(2);
			}
		}
		{ // single character midstring
			utils::DataList::DataListIterator it = l.find("d");
			utils::DataList out;
			l.subList(out, it, l.endList());
			if (out.substring(out.beginList(), out.endList()) != "d") {
				std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
				error(3);
			}
		}
		{ // copy nothing
			utils::DataList out;
			l.subList(out, l.endList(), l.endList());
			if (out.substring(out.beginList(), out.endList()) != "") {
				std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
				error(4);
			}
		}
	}

	std::cout << "All successful" << std::endl;
	return 0;
}