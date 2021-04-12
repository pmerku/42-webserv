//
// Created by jelle on 3/29/2021.
//

#include "utils/Uri.hpp"
#include "utils/split.hpp"
#include <algorithm>

using namespace utils;

Uri::Uri(): isWildcard(false), path("/"), query(), identifier() {}

Uri::Uri(const std::string &in): isWildcard(false), path(), query(), identifier() {
	std::string p = in;
	std::string::size_type iPos = p.find('#');
	if (iPos != std::string::npos) {
		identifier = p.substr(iPos+1);
		p = p.substr(0, iPos);
	}

	std::string::size_type qPos = p.find('?');
	if (qPos != std::string::npos) {
		identifier = p.substr(qPos+1);
		p = p.substr(0, qPos);
	}

	path = p;
}

void Uri::appendPath(const std::string &in, bool fixDirectoryTraversal) {
	std::string p = in;

	if (p.length() == 0) return;
	if (p[0] == '/') p = p.substr(1);
	if (p.length() == 0) return;
	if (path.rfind('/') != path.length()-1) {
		// doesnt end with slash, add one
		path += '/';
	}

	bool hasPrefixSlash = path[0] == '/';
	std::string fullPath = path + p;
	if (fixDirectoryTraversal) {
		std::vector<std::string> parts = utils::split(fullPath, "/");

		std::vector<std::string>::iterator directoryTraversal = std::find(parts.begin(), parts.end(), "..");
		while (directoryTraversal != parts.end()) {
			// remove one section of directory traversal
			if (directoryTraversal == parts.begin())
				break; // give up
			std::vector<std::string>::iterator previousSection = directoryTraversal;
			--previousSection;

			parts.erase(previousSection); // in vector all places move by one, this will remove previous part and ..
			parts.erase(previousSection);

			// recheck for traversal
			directoryTraversal = std::find(parts.begin(), parts.end(), "..");
		}

		fullPath.clear();
		std::vector<std::string>::iterator it = parts.begin();
		while (it != parts.end()) {
			fullPath += *it;
			++it;
			if (it != parts.end())
				fullPath += "/";
		}
		if (hasPrefixSlash)
			fullPath.insert(0, "/");
	}
	path = fullPath;
}

void Uri::setQuery() {
	query = "";
}

void Uri::setQuery(const std::string &q) {
	query = q;
}

void Uri::setIdentifier() {
	identifier = "";
}

void Uri::setIdentifier(const std::string &id) {
	identifier = id;
}

void Uri::setPath(const std::string &p) {
	path = p;
}

std::string Uri::getFull() const {
	std::string out = path;
	if (!query.empty()) {
		out += '?';
		out += query;
	}
	if (!identifier.empty()) {
		out += '#';
		out += identifier;
	}
	return out;
}

std::string Uri::getExt() const {
	std::string::size_type extPos = path.rfind('.');
	std::string::size_type slashPos = path.rfind('/');
	if (slashPos > extPos)
		return ""; // no extension
	return path.substr(extPos+1);
}
