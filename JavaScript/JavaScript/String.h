#pragma once

#include <tchar.h>
#include <string>

#if defined(_UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

namespace JavaScript {

	class String :
		public tstring
	{
	public:
		String(void) {};
		String(const TCHAR* s) : tstring(s) {};
		String(const tstring& src) : tstring(src) {};
		~String(void) {};

		String toUpperCase() const;
		String toLowerCase() const;
	};

} // namespace JavaScript
