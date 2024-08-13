#include "ChineseG2p.h"
#include "G2pglobal.h"
#include "ChineseG2p_p.h"

#include <unordered_map>
#include <stdexcept>

namespace IKg2p
{
    static const std::unordered_map<char32_t, u8string> numMap = {
        {U'0', U"零"},
        {U'1', U"一"},
        {U'2', U"二"},
        {U'3', U"三"},
        {U'4', U"四"},
        {U'5', U"五"},
        {U'6', U"六"},
        {U'7', U"七"},
        {U'8', U"八"},
        {U'9', U"九"}
    };

    static u8string mid(const u8stringlist& inputList, size_t cursor, size_t length)
    {
        const size_t end = std::min(cursor + length, inputList.size());

        u8string result;
        for (size_t i = cursor; i < end; ++i)
        {
            result += inputList[i];
        }

        return result;
    }

    // reset pinyin to raw string
    static std::vector<G2pRes> resetZH(const u8stringlist& input, const std::vector<G2pRes>& res,
                                       const std::vector<int>& positions)
    {
        std::vector<G2pRes> result;
        result.reserve(input.size());
        for (const auto& lyric : input)
        {
            result.emplace_back(G2pRes{lyric.cpp_str()});
        }

        for (int i = 0; i < positions.size(); i++)
        {
            result[positions[i]] = res[i];
        }
        return result;
    }

    // delete elements from the list
    template <class T>
    static inline void removeElements(std::vector<T>& vector, int start, int n)
    {
        vector.erase(vector.begin() + start, vector.begin() + start + n);
    }

    ChineseG2pPrivate::ChineseG2pPrivate(std::string language) : m_language(std::move(language))
    {
    }

    ChineseG2pPrivate::~ChineseG2pPrivate() = default;

    // load zh convert dict
    void ChineseG2pPrivate::init()
    {
        const std::string dict_dir = dictionaryPath() + "/" + m_language;

        initialized = loadDict(dict_dir, "phrases_map.txt", phrases_map) &&
            loadDict(dict_dir, "phrases_dict.txt", phrases_dict) &&
            loadDict(dict_dir, "user_dict.txt", phrases_dict) &&
            loadDict(dict_dir, "word.txt", word_dict) &&
            loadDict(dict_dir, "trans_word.txt", trans_dict);
    }

    // get all chinese characters and positions in the list
    void ChineseG2pPrivate::zhPosition(const u8stringlist& input, u8stringlist& res, std::vector<int>& positions,
                                       const bool& convertNum)
    {
        res.reserve(input.size());
        positions.reserve(input.size());
        for (int i = 0; i < input.size(); ++i)
        {
            const auto& item = input[i];
            if (item.empty())
                continue;

            const auto simItem = trans_dict.find(item) != trans_dict.end() ? trans_dict[item] : item;

            if (word_dict.find(simItem) != word_dict.end())
            {
                res.emplace_back(simItem);
                positions.emplace_back(i);
                continue;
            }

            if (convertNum)
            {
                const auto it = numMap.find(input[i].front());
                if (it != numMap.end())
                {
                    res.emplace_back(it->second);
                    positions.emplace_back(i);
                }
            }
        }
    }

    ChineseG2p::ChineseG2p(const std::string& language) : ChineseG2p(
        *new ChineseG2pPrivate(language))
    {
    }

    ChineseG2p::~ChineseG2p() = default;

    bool ChineseG2p::initialized() const
    {
        return d_ptr->initialized;
    }

    std::vector<G2pRes> ChineseG2p::hanziToPinyin(const std::string& input, bool tone, bool convertNum,
                                                  errorType error) const
    {
        return hanziToPinyin(splitString(u8string(input)), tone, convertNum, error);
    }

    std::vector<G2pRes> ChineseG2p::hanziToPinyin(const std::vector<std::string>& input, bool tone, bool convertNum,
                                                  errorType error) const
    {
        u8stringlist inputList;
        inputList.reserve(input.size());
        for (const auto& item : input)
        {
            inputList.emplace_back(item);
        }
        return hanziToPinyin(inputList, tone, convertNum, error);
    }

    std::vector<std::string> ChineseG2p::resToStringList(const std::vector<G2pRes>& input)
    {
        std::vector<std::string> result;
        result.reserve(input.size());
        for (const auto& res : input)
            result.emplace_back(res.error ? res.lyric : res.syllable);
        return result;
    }

    std::vector<G2pRes> ChineseG2p::hanziToPinyin(const u8stringlist& input, bool tone, bool convertNum,
                                                  errorType error) const
    {
        u8stringlist inputList;
        std::vector<int> inputPos;

        // get char&pos in dict
        d_ptr->zhPosition(input, inputList, inputPos, convertNum);

        std::vector<G2pRes> result;
        result.reserve(inputList.size());
        int cursor = 0;
        while (cursor < inputList.size())
        {
            // get char
            const auto& current_char = inputList[cursor];

            // not in dict
            if (d_ptr->word_dict.find(current_char) == d_ptr->word_dict.end())
            {
                result.emplace_back(G2pRes{current_char.cpp_str()});
                cursor++;
                continue;
            }

            // is polypropylene
            if (!d_ptr->isPolyphonic(current_char))
            {
                const auto pinyin = d_ptr->getDefaultPinyin(current_char, tone);
                result.emplace_back(G2pRes{
                    current_char.cpp_str(),
                    pinyin[0].cpp_str(),
                    toStdList(pinyin),
                    false
                });
                cursor++;
            }
            else
            {
                bool found = false;
                for (int length = 4; length >= 2 && !found; length--)
                {
                    if (cursor + length <= inputList.size())
                    {
                        // cursor: 地, subPhrase: 地久天长
                        const u8string subPhrase = mid(inputList, cursor, length);
                        if (d_ptr->phrases_dict.find(subPhrase) != d_ptr->phrases_dict.end())
                        {
                            const auto subRes = d_ptr->phrases_dict[subPhrase];
                            for (int i = 0; i < subRes.size(); i++)
                            {
                                const auto lyric = subPhrase.substr(i, 1).cpp_str();
                                result.emplace_back(G2pRes{
                                    lyric,
                                    subRes[i].cpp_str(),
                                    toStdList(d_ptr->getDefaultPinyin(lyric, tone)),
                                    false
                                });
                            }
                            cursor += length;
                            found = true;
                        }

                        if (cursor >= 1)
                        {
                            // cursor: 重, subPhrase_1: 语重心长
                            const u8string subPhrase1 = mid(inputList, cursor - 1, length);
                            auto it = d_ptr->phrases_dict.find(subPhrase1);
                            if (it != d_ptr->phrases_dict.end())
                            {
                                result.pop_back();
                                const auto& subRes1 = it->second;
                                for (int i = 0; i < subRes1.size(); i++)
                                {
                                    const auto lyric = subPhrase1.substr(i, 1).cpp_str();
                                    result.emplace_back(G2pRes{
                                        lyric,
                                        subRes1[i].cpp_str(),
                                        toStdList(d_ptr->getDefaultPinyin(lyric, tone)),
                                        false
                                    });
                                }
                                cursor += length - 1;
                                found = true;
                            }
                        }
                    }

                    if (cursor + 1 >= length && cursor + 1 <= inputList.size())
                    {
                        // cursor: 好, xSubPhrase: 各有所好
                        const u8string subPhraseBack = mid(inputList, cursor + 1 - length, length);
                        auto it = d_ptr->phrases_dict.find(subPhraseBack);
                        if (it != d_ptr->phrases_dict.end())
                        {
                            // overwrite pinyin
                            removeElements(result, cursor + 1 - length, length - 1);
                            const auto& subResBack = it->second;
                            for (int i = 0; i < subResBack.size(); i++)
                            {
                                const auto lyric = subPhraseBack.substr(i, 1).cpp_str();
                                result.emplace_back(G2pRes{
                                    lyric,
                                    subResBack[i].cpp_str(),
                                    toStdList(d_ptr->getDefaultPinyin(lyric, tone)),
                                    false
                                });
                            }
                            cursor += 1;
                            found = true;
                        }
                    }

                    if (cursor + 2 >= length && cursor + 2 <= inputList.size())
                    {
                        // cursor: 好, xSubPhrase: 叶公好龙
                        const u8string subPhraseBack1 = mid(inputList, cursor + 2 - length, length);
                        const auto it = d_ptr->phrases_dict.find(subPhraseBack1);
                        if (it != d_ptr->phrases_dict.end())
                        {
                            // overwrite pinyin
                            removeElements(result, cursor + 2 - length, length - 2);
                            const auto& subResBack1 = it->second;
                            for (int i = 0; i < subResBack1.size(); i++)
                            {
                                const auto lyric = subPhraseBack1.substr(i, 1).cpp_str();
                                result.emplace_back(G2pRes{
                                    lyric,
                                    subResBack1[i].cpp_str(),
                                    toStdList(d_ptr->getDefaultPinyin(lyric, tone)),
                                    false
                                });
                            }
                            cursor += 2;
                            found = true;
                        }
                    }
                }

                // not found, use default pinyin
                if (!found)
                {
                    result.emplace_back(G2pRes{
                        current_char.cpp_str(),
                        d_ptr->getDefaultPinyin(current_char, tone)[0].cpp_str(),
                        toStdList(d_ptr->getDefaultPinyin(current_char, tone)),
                        false
                    });
                    cursor++;
                }
            }
        }

        if (!tone)
        {
            for (G2pRes& item : result)
            {
                if (!item.error && isDigit(item.syllable.back()))
                {
                    item.syllable.pop_back();
                }
            }
        }

        // Alloc 2
        if (error == errorType::Ignore)
        {
            return result;
        }
        return resetZH(input, result, inputPos);
    }

    ChineseG2p::ChineseG2p(ChineseG2pPrivate& d) : d_ptr(&d)
    {
        d.q_ptr = this;

        d.init();
    }

    std::string ChineseG2p::tradToSim(const std::string& text) const
    {
        return d_ptr->tradToSim(text).cpp_str();
    }

    bool ChineseG2p::isPolyphonic(const std::string& text) const
    {
        return d_ptr->isPolyphonic(text);
    }

    std::vector<std::string> ChineseG2p::getDefaultPinyin(const std::string& text, bool tone) const
    {
        return toStdList(d_ptr->getDefaultPinyin(d_ptr->tradToSim(text), tone));
    }
}
