//
// Created by jelle on 3/27/2021.
//

#ifndef DATALIST_HPP
#define DATALIST_HPP

#include <list>
#include <string>
#include <cstring>

namespace utils {

	class DataList {
	public:
		typedef unsigned long	size_type;

	private:
		class DataListSection {
		private:
			static void	copyData(const char *in, size_type size, char **out) {
				char *newData = new char[size];
				std::memcpy(newData, in, size);
				*out = newData;
			}

		public:
			DataListSection(): data(0), size(0) {};
			DataListSection(char *d, size_type s): data(d), size(s) {};
			DataListSection(const DataListSection &rhs): data(0), size(rhs.size) {
				DataListSection::copyData(rhs.data, rhs.size, &data);
			};

			~DataListSection() {
				delete[] data;
			};

			DataListSection	&operator=(const DataListSection &rhs) {
				if (&rhs == this) return *this;
				size = rhs.size;
				DataListSection::copyData(rhs.data, rhs.size, &data);
				return *this;
			}

			char		*data;
			size_type	size;
		};

		std::list<DataListSection>	_list;
		size_type					_size;

	public:
		class DataListIterator {
		public:
			typedef std::ptrdiff_t					difference_type;
			typedef std::bidirectional_iterator_tag	iterator_category;
			typedef DataListSection	value_type;
			typedef char		pointer;
			typedef char&		reference;

			DataListIterator(std::list<DataListSection>::iterator it, DataList::size_type index) : _it(it), _index(index) {}
			friend class DataList;

			reference	operator*() const { return _it->data[_index]; }
			pointer		operator->() const { return _it->data[_index]; }

			DataListIterator &operator++() {
				_index++;
				if (_index >= _it->size) {
					_index = 0;
					_it++;
				}
				return *this;
			}

			DataListIterator operator++(int) {
				DataListIterator tmp = *this;
				_index++;
				if (_index >= _it->size) {
					_index = 0;
					++_it;
				}
				return tmp;
			}

			DataListIterator &operator--() {
				if (_index == 0) {
					--_it;
					_index = _it->size;
				}
				_index--;
				return *this;
			}

			DataListIterator operator--(int) {
				DataListIterator tmp = *this;
				if (_index == 0) {
					--_it;
					_index = _it->size;
				}
				_index--;
				return tmp;
			}

			friend bool operator==(const DataListIterator& a, const DataListIterator& b) { return a._it == b._it && a._index == b._index; };
			friend bool operator!=(const DataListIterator& a, const DataListIterator& b) { return a._it != b._it || a._index != b._index; };

		private:
			std::list<DataListSection>::iterator	_it;
			DataList::size_type						_index;
		};

		typedef std::list<DataListSection>::iterator iterator;
		typedef std::list<DataListSection>::const_iterator const_iterator;

		DataList();
		size_type	size() const;
		size_type	size(DataListIterator start);
		size_type	size(DataListIterator start, DataListIterator last);
		bool		empty() const;

		void	add(const char *data, size_type size);
		void	add(const char *str);
		void	add_front(const char *data, size_type size);
		void	add_front(const char *str);

		void	resize(DataListIterator start, DataListIterator finish);
		void	clear();

		iterator begin();
		iterator end();
		DataListIterator beginList();
		DataListIterator endList();

		DataListIterator find(const std::string &data);
		DataListIterator find(const std::string &data, DataListIterator first);
		static DataListIterator find(const std::string &data, DataListIterator first, DataListIterator last);

		std::string		substring(DataListIterator start);
		static std::string 	substring(DataListIterator start, DataListIterator last);
	};

} // namespace utils

#endif //DATALIST_HPP
