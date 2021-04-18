//
// Created by jelle on 3/27/2021.
//

#include "utils/DataList.hpp"
#include <algorithm>
#include <cstring>

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

void DataList::resize(DataListIterator start, DataListIterator finish) {
	// make finish an end
	if (finish._index != 0)
		++finish._it;

	// resize packet list
	_list.erase(_list.begin(), start._it);
	_list.erase(finish._it, _list.end());

	// resize packets themselves
	if (finish._index != 0)
		--finish._it;
	if (finish._it == start._it) {
		// resize with two indexes
		size_type size = finish._index - start._index;
		if (size == 0) {
			clear();
			return;
		}
		char *newData = new char[size];
		start._it->size = size;
		std::memcpy(newData, start._it->data + start._index, size);
		delete[] start._it->data;
		start._it->data = newData;
	}
	else {
		// resize start
		{
			size_type size = start._it->size - start._index;
			char *newData = new char[size];
			start._it->size = size;
			std::memcpy(newData, start._it->data + start._index, size);
			delete[] start._it->data;
			start._it->data = newData;
		}
		// resize end
		if (finish._index != 0) {
			// 0123456789
			// 6, 10
			size_type size = finish._index;
			char *newData = new char[size];
			finish._it->size = size;
			std::memcpy(newData, finish._it->data + finish._index, size);
			delete[] finish._it->data;
			finish._it->data = newData;
		}
	}

	// recalculate size
	_size = size(beginList(), endList());
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

void DataList::clear() {
	_list.clear();
	_size = 0;
}



DataList::DataListIterator	DataList::findAndReplaceOne(const std::string &needle, const std::string &newNeedle) {
	return findAndReplaceOne(needle, newNeedle, beginList(), endList());
}

DataList::DataListIterator	DataList::findAndReplaceOne(const std::string &needle, const std::string &newNeedle, DataListIterator start) {
	return findAndReplaceOne(needle, newNeedle, start, endList());
}

DataList::DataListIterator DataList::findAndReplaceOne(const std::string &needle, const std::string &newNeedle, DataListIterator start, DataListIterator last) {
	DataListIterator it = this->find(needle, start, last);

	// it._it   list_item -> <it._index, _it._index+needle.size()>
	// it._it.data = it._it->data + newNeedle;
	// it._it.size = it._it->data.size()
	// remove/resize rest of packets that may still contain the needle bytes

	// remove needle part from main packet
	size_type needlePartSize = it._it->size - it._index;
	if (needlePartSize > needle.size())
		needlePartSize = needle.size();
	char *newData = new char[it._it->size-needlePartSize];
	::memmove(newData, it._it->data, it._index); // [abc]NEEDLEdef
	if (needlePartSize+it._index < it._it->size)
		::memmove(newData+it._index, it._it->data+needlePartSize, it._it->size-needlePartSize-it._index); // abcNEEDLE[def]
	return it;
}
