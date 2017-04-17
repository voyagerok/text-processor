#include <vector>
#include <unicode/unistr.h>

namespace tproc {

bool split_unistring(const UnicodeString &unistring, const std::vector<UnicodeString> &splitSeqs, std::vector<UnicodeString> &result);
//bool split_unistring(const UnicodeString &unistring, const UnicodeString &splitRegex, std::vector<UnicodeString> &result);
}
