#ifndef ChineseG2p_H
#define ChineseG2p_H

#include "G2pglobal.h"

#include "G2pRes.h"

namespace IKg2p
{
    enum class errorType
    {
        Default = 0, Ignore = 1
    };

    class ChineseG2pPrivate;

    class ChineseG2p
    {
    public:
        explicit ChineseG2p(const std::string& language);

        ~ChineseG2p();

        [[nodiscard]] bool initialized() const;

        std::vector<G2pRes> hanziToPinyin(const std::string& input, bool tone = true, bool convertNum = true,
                                          errorType error = errorType::Default);

        std::vector<G2pRes> hanziToPinyin(const std::vector<std::string>& input, bool tone = true,
                                          bool convertNum = true,
                                          errorType error = errorType::Default);

        static std::vector<std::string> resToStringList(const std::vector<G2pRes>& input);

        [[nodiscard]] u8string tradToSim(const u8string& text) const;

        [[nodiscard]] bool isPolyphonic(const u8string& text) const;

        [[nodiscard]] u8stringlist getDefaultPinyin(const u8string& text, bool tone = true) const;

    protected:
        explicit ChineseG2p(ChineseG2pPrivate& d);

        std::unique_ptr<ChineseG2pPrivate> d_ptr;

    private:
        std::vector<G2pRes> hanziToPinyin(const u8stringlist& input, bool tone = true, bool convertNum = true,
                                          errorType error = errorType::Default);
    };
}

#endif // ChineseG2p_H
