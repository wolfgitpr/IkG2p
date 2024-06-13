#ifndef JPG2PPRIVATE_H
#define JPG2PPRIVATE_H

#include <unordered_map>

#include "G2pglobal.h"
#include "JapaneseG2p_p.h"

namespace IKg2p
{
    class JpG2pPrivate
    {
    public:
        JpG2pPrivate();
        virtual ~JpG2pPrivate();

        void init();

        bool initialized = false;

        JapaneseG2p* q_ptr;

        std::unordered_map<u8string, u8string> kanaToRomajiMap;
        std::unordered_map<u8string, u8string> romajiToKanaMap;

        enum KanaType { Hiragana, Katakana };

        static u8stringlist convertKana(const u8stringlist& kanaList, KanaType kanaType);
    };
}

#endif // JPG2PPRIVATE_H
