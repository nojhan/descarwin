#ifndef __EO_JSON_UTILS_H__
#define __EO_JSON_UTILS_H__

# include "utils/json/Serializable.h"
# include <iostream>

namespace eoserial
{
    void printOn(const eoserial::Persistent* obj, std::ostream& out);
    void readFrom(eoserial::Persistent* obj, std::istream& _is);
}

#endif //__EO_JSON_UTILS_H__
