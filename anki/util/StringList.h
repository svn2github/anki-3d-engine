#ifndef ANKI_UTIL_STRING_LIST_H
#define ANKI_UTIL_STRING_LIST_H

#include <vector>
#include <string>


namespace anki {


/// A simple convenience class
class StringList: public std::vector<std::string>
{
	public:
		typedef std::vector<std::string> Base; ///< Its the vector of strings
		typedef Base::value_type ValueType; ///< Its string

		void join(const ValueType& sep, ValueType& out) const;
};


inline void StringList::join(const ValueType& sep, ValueType& out) const
{
	Base::const_iterator it = begin();
	for(; it != end(); it++)
	{
		out += *it;
		if(it != end() - 1)
		{
			out += sep;
		}
	}
}


} // end namespace


#endif
