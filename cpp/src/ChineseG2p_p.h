#ifndef ChineseG2pPRIVATE_H
#define ChineseG2pPRIVATE_H

#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "ChineseG2p_p.h"

namespace IKg2p
{
    class ChineseG2pPrivate final
    {
    public:
        explicit ChineseG2pPrivate(std::string language);
        virtual ~ChineseG2pPrivate();

        void init();

        bool initialized = false;

        ChineseG2p* q_ptr{};

        std::unordered_map<u8string, u8string> phrases_map;
        std::unordered_map<u8string, u8stringlist> phrases_dict;
        std::unordered_map<u8string, u8stringlist> word_dict;
        std::unordered_map<u8string, u8string> trans_dict;

        std::string m_language;

        inline bool isPolyphonic(const u8string& text) const
        {
            return phrases_map.find(text) != phrases_map.end();
        }

        inline u8string tradToSim(const u8string& text) const
        {
            const auto it = trans_dict.find(text);
            return it != trans_dict.end() ? it->second : text;
        }

        inline u8stringlist getDefaultPinyin(const u8string& text, bool tone) const
        {
            const auto it = word_dict.find(tradToSim(text));
            if (it == word_dict.end())
                return {text};

            u8stringlist candidates = it->second;
            u8stringlist res;
            res.reserve(candidates.size());

            std::unordered_set<u8string> seen;

            for (u8string& item : candidates)
            {
                if (!tone && isDigit(item.back()))
                {
                    item.pop_back();
                }
                if (seen.insert(item).second)
                {
                    res.push_back(item);
                }
            }

            if (res.empty())
                return {text};
            return res;
        }

        void zhPosition(const u8stringlist& input, u8stringlist& res, std::vector<int>& positions,
                        const bool& convertNum);
    };
}

#endif // ChineseG2pPRIVATE_H
