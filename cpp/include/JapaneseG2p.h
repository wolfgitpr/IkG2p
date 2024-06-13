#ifndef JPG2P_H
#define JPG2P_H

#include "G2pglobal.h"

namespace IKg2p
{
    class JpG2pPrivate;

    class JapaneseG2p final
    {
    public:
        explicit JapaneseG2p();
        ~JapaneseG2p();

        std::vector<std::string> kanaToRomaji(const std::string& kanaStr,
                                              const bool& doubleWrittenSokuon = false) const;
        std::vector<std::string> kanaToRomaji(const std::vector<std::string>& kanaList,
                                              const bool& doubleWrittenSokuon = false) const;

        std::vector<std::string> romajiToKana(const std::string& romajiStr) const;
        std::vector<std::string> romajiToKana(const std::vector<std::string>& romajiList) const;

    protected:
        explicit JapaneseG2p(JpG2pPrivate& d);

        std::unique_ptr<JpG2pPrivate> d_ptr;

    private:
        std::vector<std::string> romajiToKana(const u8stringlist& romajiList) const;
        std::vector<std::string> kanaToRomaji(const u8stringlist& kanaList,
                                              const bool& doubleWrittenSokuon = false) const;
    };
}

#endif // JPG2P_H
