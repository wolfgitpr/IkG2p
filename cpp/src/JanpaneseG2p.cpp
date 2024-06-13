#include <regex>

#include "JapaneseG2p.h"
#include "JapaneseG2p_p.h"

#include "G2pglobal.h"

namespace IKg2p
{
    // keep only letters
    static std::string filterString(const std::string& str)
    {
        std::string words;
        for (const auto& ch : str)
        {
            const auto u = ch;
            if (u >= 128 || !isLetter(ch))
            {
                if (words.empty() || words.back() != ' ')
                {
                    words += ' ';
                }
                continue;
            }
            words += ch;
        }
        return words;
    }

    // split RomajiStr to List
    static u8stringlist splitRomaji(const std::string& input)
    {
        const std::string cleanStr = filterString(input);
        u8stringlist res;

        // romaji
        std::regex rx("((?=[^aiueo])[a-z]){0,2}[aiueo]");
        std::sregex_iterator begin(cleanStr.begin(), cleanStr.end(), rx);
        std::sregex_iterator end;

        for (std::sregex_iterator i = begin; i != end; ++i)
        {
            res.emplace_back(i->str());
        }

        return res;
    }

    JpG2pPrivate::JpG2pPrivate()
    {
    }

    JpG2pPrivate::~JpG2pPrivate() = default;

    // load jp convert dict
    void JpG2pPrivate::init()
    {
        const auto dict_dir = dictionaryPath();
        initialized = loadDict(dict_dir, "kana2romaji.txt", kanaToRomajiMap);

        for (auto it = kanaToRomajiMap.begin(); it != kanaToRomajiMap.end(); ++it)
        {
            romajiToKanaMap[it->second] = it->first;
        }
    }

    // convert Hiragana to Katakana
    u8stringlist JpG2pPrivate::convertKana(const u8stringlist& kanaList, KanaType kanaType)
    {
        constexpr int hiraganaStart = 0x3041;
        constexpr int katakanaStart = 0x30A1;

        u8stringlist inputlist;
        for (const auto& kana : kanaList)
            inputlist.emplace_back(kana);

        u8stringlist convertedList;
        const std::regex rx("[\u3040-\u309F\u30A0-\u30FF]+");

        for (const auto& kana : kanaList)
        {
            u8string convertedKana;
            if (std::regex_match(kana.cpp_str(), rx))
            {
                for (const char32_t kanaChar : kana)
                {
                    if (kanaType == KanaType::Hiragana)
                    {
                        // target is Hiragana
                        if (kanaChar >= katakanaStart && kanaChar < katakanaStart + 0x5E)
                        {
                            // Katakana->Hiragana
                            convertedKana +=
                                static_cast<char32_t>(kanaChar - katakanaStart + hiraganaStart);
                        }
                        else
                        {
                            convertedKana += kanaChar;
                        }
                    }
                    else
                    {
                        // target is Katakana
                        if (kanaChar >= hiraganaStart && kanaChar < hiraganaStart + 0x5E)
                        {
                            // Hiragana->Katakana
                            convertedKana +=
                                static_cast<char32_t>(kanaChar + katakanaStart - hiraganaStart);
                        }
                        else
                        {
                            convertedKana += kanaChar;
                        }
                    }
                }
            }
            else
            {
                convertedKana = kana;
            }
            convertedList.emplace_back(convertedKana);
        }
        return convertedList;
    }

    JapaneseG2p::JapaneseG2p() : JapaneseG2p(*new JpG2pPrivate())
    {
    }

    JapaneseG2p::~JapaneseG2p() = default;

    std::vector<std::string> JapaneseG2p::kanaToRomaji(const u8stringlist& kanaList,
                                                       const bool& doubleWrittenSokuon) const
    {
        const u8stringlist inputList = d_ptr->convertKana(kanaList, JpG2pPrivate::KanaType::Hiragana);
        u8stringlist romajiList;
        for (const u8string& kana : inputList)
        {
            if (kana != "゜" && kana != "ー")
            {
                if (d_ptr->kanaToRomajiMap.find(kana) != d_ptr->kanaToRomajiMap.end())
                    romajiList.push_back(d_ptr->kanaToRomajiMap.find(kana)->second);
                else
                    romajiList.push_back(kana);
            }
        }

        for (int i = 0; i < romajiList.size() - 1 && doubleWrittenSokuon; ++i)
        {
            char32_t nextChar = U' ';
            if (d_ptr->romajiToKanaMap.find(romajiList[i + 1]) != d_ptr->romajiToKanaMap.end())
                nextChar = d_ptr->romajiToKanaMap.find(romajiList[i + 1])->second.at(0);
            if (romajiList[i] == "cl" && isKana(nextChar) &&
                std::u32string(U"あいうえおアイウエオっんを").find(nextChar) == std::u32string::npos)
            {
                romajiList[i + 1].insert(romajiList[i + 1].begin(), romajiList[i + 1][0]);
                romajiList.erase(romajiList.begin() + i);
            }
        }
        return toStdList(romajiList);
    }

    std::vector<std::string> JapaneseG2p::kanaToRomaji(const std::string& kanaStr,
                                                       const bool& doubleWrittenSokuon) const
    {
        return kanaToRomaji(splitString(u8string(kanaStr)), doubleWrittenSokuon);
    }

    std::vector<std::string> JapaneseG2p::kanaToRomaji(const std::vector<std::string>& kanaList,
                                                       const bool& doubleWrittenSokuon) const
    {
        u8stringlist inputList;
        inputList.reserve(kanaList.size());
        for (const auto& item : kanaList)
        {
            inputList.emplace_back(item);
        }
        return kanaToRomaji(inputList, doubleWrittenSokuon);
    }

    JapaneseG2p::JapaneseG2p(JpG2pPrivate& d) : d_ptr(&d)
    {
        d.q_ptr = this;

        d.init();
    }

    std::vector<std::string> JapaneseG2p::romajiToKana(const std::string& romajiStr) const
    {
        const u8stringlist input = splitRomaji(romajiStr);
        return romajiToKana(input);
    }

    std::vector<std::string> JapaneseG2p::romajiToKana(const std::vector<std::string>& romajiList) const
    {
        u8stringlist inputList;
        inputList.reserve(romajiList.size());
        for (const auto& item : romajiList)
        {
            inputList.emplace_back(item);
        }
        return romajiToKana(inputList);
    }

    std::vector<std::string> JapaneseG2p::romajiToKana(const u8stringlist& romajiList) const
    {
        u8stringlist kanaList;
        for (const u8string& romaji : romajiList)
        {
            if (d_ptr->romajiToKanaMap.find(romaji) != d_ptr->romajiToKanaMap.end())
                kanaList.push_back(d_ptr->romajiToKanaMap.find(romaji)->second);
            else
                kanaList.push_back(romaji);
        }
        return toStdList(kanaList);
    }
}
