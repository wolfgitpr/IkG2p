#ifndef SYLLABLE2P_H
#define SYLLABLE2P_H

#include <string>
#include <vector>
#include <memory>

namespace IKg2p
{
    class Syllable2pPrivate;

    class Syllable2p
    {
    public:
        explicit Syllable2p(std::string dictPath, std::string dictName, const char& sep1 = '\t',
                            const std::string& sep2 = " ");

        ~Syllable2p();

        std::vector<std::string> syllableToPhoneme(const std::string& syllable) const;

        std::vector<std::vector<std::string>> syllableToPhoneme(
            const std::vector<std::string>& syllables) const;

    protected:
        explicit Syllable2p(Syllable2pPrivate& d);

        std::unique_ptr<Syllable2pPrivate> d_ptr;
    };
}


#endif // SYLLABLE2P_H
