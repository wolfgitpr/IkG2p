#ifndef G2PRES_H
#define G2PRES_H

#include <string>

namespace IKg2p
{
    struct G2pRes
    {
        std::string lyric;
        std::string syllable;
        std::vector<std::string> candidates;
        bool error = true;
    };
}
#endif //G2PRES_H
