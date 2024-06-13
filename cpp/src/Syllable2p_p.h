#ifndef SYLLABLE2PPRIVATE_H
#define SYLLABLE2PPRIVATE_H

#include <unordered_map>

#include "Syllable2p.h"

namespace IKg2p
{
    class Syllable2pPrivate
    {
    public:
        explicit Syllable2pPrivate(std::string dictPath, std::string dictName, const char& sep1 = '\t',
                                   std::string sep2 = " ");

        ~Syllable2pPrivate();

        void init();

        bool initialized = false;

        Syllable2p* q_ptr;

        std::unordered_map<std::string, std::vector<std::string>> phonemeMap;

    private:
        std::string dictPath;
        std::string dictName;
        char sep1;
        std::string sep2;
    };
}
#endif // SYLLABLE2PPRIVATE_H
