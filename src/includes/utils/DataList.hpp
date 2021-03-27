//
// Created by jelle on 3/27/2021.
//

#ifndef DATALIST_HPP
#define DATALIST_HPP

#include <list>

namespace utils {

	class DataList {
	public:
		typedef unsigned long	size_type;

	private:
		class DataListSection {
		private:
			static void	copyData(const char *in, size_type size, char **out) {
				char *newData = new char[size];
				for (size_type i = 0; i < size; i++)
					newData[i] = in[i];
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
		typedef std::list<DataListSection>::iterator iterator;
		typedef std::list<DataListSection>::const_iterator const_iterator;

		DataList();
		size_type	size() const;
		bool		empty() const;

		void	add(const char *data, size_type size);
		void	add(const char *str);
		void	add_front(const char *data, size_type size);
		void	add_front(const char *str);

		iterator begin();
		iterator end();
	};

}

#endif //DATALIST_HPP
