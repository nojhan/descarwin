#ifndef __EO_JSON_UTILS_H__
#define __EO_JSON_UTILS_H__

# include "utils/json/Serializable.h"
# include <iostream>

namespace json
{
    void printOn(const json::Serializable* obj, std::ostream& out);
    void readFrom(json::Serializable* obj, std::istream& _is);
}

#endif //__EO_JSON_UTILS_H__
