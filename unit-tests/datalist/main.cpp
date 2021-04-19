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

	{
		utils::DataList out;
		out.add("42 is not as cool as ME");
		out.findAndReplaceOne("42", "CODAM");
		if (out.substring(out.beginList(), out.endList()) != "CODAM is not as cool as ME") {
			std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
			error(5);
		}
	}

	{
		utils::DataList out;
		out.add("4");
		out.add("2 is not as cool as Jelle");
		out.findAndReplaceOne("42", "CODAM");
		if (out.substring(out.beginList(), out.endList()) != "CODAM is not as cool as Jelle") {
			std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
			error(6);
		}
	}

	{
		utils::DataList out;
		out.add("4");
		out.add("2");
		out.add("0 is not as cool as Jelle");
		out.findAndReplaceOne("420", "CODAM");
		if (out.substring(out.beginList(), out.endList()) != "CODAM is not as cool as Jelle") {
			std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
			error(7);
		}
	}

	{
		utils::DataList out;
		out.add("4");
		out.add("2");
		out.add(" is not as cool as Jelle");
		out.findAndReplaceOne("42", "CODAM");
		if (out.substring(out.beginList(), out.endList()) != "CODAM is not as cool as Jelle") {
			std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
			error(8);
		}
	}

	{
		utils::DataList out;
		out.add("4");
		out.add("2");
		out.findAndReplaceOne("42", "CODAM");
		if (out.substring(out.beginList(), out.endList()) != "CODAM") {
			std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
			error(9);
		}
	}

	{
		utils::DataList out;
		out.add("YOO");
		out.add("is not as cool as 42");
		out.findAndReplaceOne("42", "CODAM");
		if (out.substring(out.beginList(), out.endList()) != "YOOis not as cool as CODAM") {
			std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
			error(10);
		}
	}

	{
		utils::DataList out;
		out.add("H");
		out.add("A is not as cool as Jelle");
		out.findAndReplaceOne("42", "CODAM");
		if (out.substring(out.beginList(), out.endList()) != "HA is not as cool as Jelle") {
			std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
			error(11);
		}
	}

	{
		utils::DataList out;
		out.add("4");
		out.add("2 is not as cool as Jelle");
		out.findAndReplaceOne("Jelle", "42");
		if (out.substring(out.beginList(), out.endList()) != "42 is not as cool as 42") {
			std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
			error(12);
		}
	}

	{
		utils::DataList out;
		out.add("J");
		out.add("elle is not as cool as Codam");
		out.findAndReplaceOne("Jelle", "42");
		if (out.substring(out.beginList(), out.endList()) != "42 is not as cool as Codam") {
			std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
			error(13);
		}
	}

	{
		utils::DataList out;
		out.add("J");
		out.add("e");
		out.add("l");
		out.add("l");
		out.add("e is not as cool as 42");
		out.findAndReplaceOne("Jelle", "42");
		if (out.substring(out.beginList(), out.endList()) != "42 is not as cool as 42") {
			std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
			error(14);
		}
	}

	{
		utils::DataList out;
		out.add("4");
		out.add("2 is not as cool as Jelle");
		out.findAndReplaceOne("Jelle", "42");
		if (out.substring(out.beginList(), out.endList()) != "42 is not as cool as 42") {
			std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
			error(15);
		}
	}

	{
		utils::DataList out;
		out.add("Jel");
		out.add("le");
		out.findAndReplaceOne("Jelle", "42");
		if (out.substring(out.beginList(), out.endList()) != "42") {
			std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
			error(16);
		}
	}

	{
		utils::DataList out;
		out.add("4");
		out.add("2 is not Jelle, you silly");
		out.findAndReplaceOne("Jelle", "42");
		if (out.substring(out.beginList(), out.endList()) != "42 is not 42, you silly") {
			std::cerr << "output: " << out.substring(out.beginList(), out.endList()) << std::endl;
			error(17);
		}
	}

	std::cout << "All successful" << std::endl;
	return 0;
}