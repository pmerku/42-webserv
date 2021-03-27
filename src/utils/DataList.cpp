//
// Created by jelle on 3/27/2021.
//

#include "DataList.hpp"

using namespace utils;

DataList::DataList(): _list(), _size(0) {}

DataList::size_type DataList::size() const {
	return _size;
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
