#include "cyaStrdk.h"

CHAR_TYPE_INLINE const CHAR_TYPE* OnStrRChrRsTmpl(const CHAR_TYPE* const base, const CHAR_TYPE* str, const CHAR_TYPE* chrs)
{
	while (str >= base)
	{
		if (StrChr_(chrs, *str))
			return str;
		str--;
	}
	return NULL;
}

const char* StrRChrRs(const char* const base, const char* str, const char* chrs)
{
	return OnStrRChrRsTmpl(base, str, chrs);
}

const WCHAR* StrRChrRs(const WCHAR* const base, const WCHAR* str, const WCHAR* chrs)
{
	return OnStrRChrRsTmpl(base, str, chrs);
}