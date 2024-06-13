#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string>

namespace IKg2p
{
    std::string wideToUtf8(const std::wstring& utf16Str);
    std::wstring utf8ToWide(const std::string& utf8Str);
}

#endif //STRINGUTIL_H
