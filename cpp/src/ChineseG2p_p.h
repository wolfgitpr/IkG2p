#ifndef ChineseG2pPRIVATE_H
#define ChineseG2pPRIVATE_H

#include <iostream>
#include <unordered_map>

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
            if (trans_dict.find(text) != trans_dict.end())
                return trans_dict.find(text)->second;
            return text;
        }

        inline u8string getDefaultPinyin(const u8string& text) const
        {
            if (word_dict.find(text) != word_dict.end())
                return word_dict.find(text)->second.at(0);
            return {};
        }

        void zhPosition(const u8stringlist& input, u8stringlist& res, std::vector<int>& positions,
                        const bool& convertNum);
    };
}

#endif // ChineseG2pPRIVATE_H
