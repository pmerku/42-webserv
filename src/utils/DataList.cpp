//
// Created by jelle on 3/27/2021.
//

#include "utils/DataList.hpp"
#include <algorithm>

using namespace utils;

DataList::DataList(): _list(), _size(0) {}

DataList::size_type DataList::size() const {
	return _size;
}

DataList::size_type DataList::size(DataList::DataListIterator start) {
	return size(start, endList());
}

/*
 * size with iterators, including start, excluding end
 */
DataList::size_type DataList::size(DataList::DataListIterator start, DataList::DataListIterator last) {
	if (start == beginList() && last == endList())
		return size();
	size_type s = 0;
	if (start._it == last._it)
		return last._index - start._index;
	s += start._it->size - start._index; // index start
	++(start._it);
	for (;start._it != last._it; ++(start._it))
		s += start._it->size; // in between packet sizes
	s += last._index; // last packet start
	return s;
}

bool DataList::empty() const {
	return _size == 0;
}

void DataList::add(const char *data, DataList::size_type size) {
	char *newData = new char[size];
	for (size_type i = 0; i < size; i++)
		newData[i] = data[i];
	_size += size;
	_list.push_back(DataListSection(newData, size));
}

void DataList::add(const char *str) {
	size_type size = 0;
	while (str[size] != 0)
		size++;
	add(str, size);
}

void DataList::add_front(const char *data, DataList::size_type size) {
	char *newData = new char[size];
	for (size_type i = 0; i < size; i++)
		newData[i] = data[i];
	_size += size;
	_list.push_front(DataListSection(newData, size));
}

void DataList::add_front(const char *str) {
	size_type size = 0;
	while (str[size] != 0)
		size++;
	add_front(str, size);
}

DataList::iterator DataList::begin() {
	return _list.begin();
}

DataList::iterator DataList::end() {
	return _list.end();
}

DataList::DataListIterator DataList::beginList() {
	return DataListIterator(_list.begin(), 0);
}

DataList::DataListIterator DataList::endList() {
	return DataListIterator(_list.end(), 0);
}

DataList DataList::subList(DataList::const_iterator start, DataList::const_iterator finish) {
	DataList newDataList;
	const_iterator it = _list.begin();
	for (; it != start && it != _list.end(); ++it);
	for (; it != finish && it != _list.end(); ++it) {
		newDataList._list.push_back(DataListSection((*it).data, (*it).size));
	}
	return newDataList;
}

DataList::DataListIterator DataList::find(const std::string &data, DataListIterator first) {
	return find(data, first, endList());
}

DataList::DataListIterator DataList::find(const std::string &data) {
	return find(data, beginList(), endList());
}

DataList::DataListIterator DataList::find(const std::string &data, DataListIterator first, DataListIterator last) {
	for (; first != last; ++first) {
		if (data[0] == *first) {
			DataListIterator it = first;
			bool	found = true;
			for (std::string::size_type i = 0; i < data.length(); ++i) {
				// if not equal, exit loop
				if (data[i] != *it) {
					found = false;
					break;
				}
				// if end of string, exit loop
				if (i+1 == data.length())
					break;
				++it;
				// if end of datalist, exit loop
				if (it == last) {
					found = false;
					break;
				}
			}
			if (found)
				return first;
		}
	}
	return last;
}

std::string DataList::substring(DataListIterator first, DataListIterator last) {
	std::string out;
	// copy over lose characters
	for (; first != last; ++first) {
		out += *first;
	}
	return out;
}

std::string DataList::substring(DataListIterator first) {
	return substring(first, endList());
}
