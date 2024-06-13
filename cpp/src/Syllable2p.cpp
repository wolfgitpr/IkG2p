#include "Syllable2p.h"
#include "Syllable2p_p.h"

#include <utility>

#include "G2pglobal.h"


namespace IKg2p
{
    Syllable2pPrivate::Syllable2pPrivate(std::string dictPath, std::string dictName, const char& sep1,
                                         std::string sep2) : dictPath(std::move(
                                                                 dictPath)), dictName(std::move(dictName)),
                                                             sep1(sep1), sep2(std::move(sep2))
    {
    }

    Syllable2pPrivate::~Syllable2pPrivate() = default;

    void Syllable2pPrivate::init()
    {
        initialized = loadDict(dictPath, dictName, phonemeMap, sep1, sep2);
    }

    Syllable2p::Syllable2p(std::string dictPath, std::string dictName, const char& sep1,
                           const std::string& sep2) : Syllable2p(
        *new Syllable2pPrivate(std::move(dictPath), std::move(dictName), sep1, sep2))
    {
    }

    Syllable2p::~Syllable2p() = default;

    std::vector<std::vector<std::string>> Syllable2p::syllableToPhoneme(const std::vector<std::string>& syllables) const
    {
        std::vector<std::vector<std::string>> phonemeList;
        for (const std::string& word : syllables)
        {
            std::vector<std::string> res;
            if (d_ptr->phonemeMap.find(word) != d_ptr->phonemeMap.end())
                res = d_ptr->phonemeMap.find(word)->second;
            phonemeList.push_back(res);
        }
        return phonemeList;
    }


    std::vector<std::string> Syllable2p::syllableToPhoneme(const std::string& syllable) const
    {
        if (d_ptr->phonemeMap.find(syllable) != d_ptr->phonemeMap.end())
            return d_ptr->phonemeMap.find(syllable)->second;
        return {};
    }

    Syllable2p::Syllable2p(Syllable2pPrivate& d) : d_ptr(&d)
    {
        d.q_ptr = this;

        d.init();
    }
}
