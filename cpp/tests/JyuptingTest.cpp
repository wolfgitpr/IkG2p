#include "JyuptingTest.h"

#include <chrono>
#include <iostream>

#include "Common.h"

namespace G2pTest
{
    JyuptingTest::JyuptingTest() : g2p_can(new IKg2p::CantoneseG2p())
    {
    }

    JyuptingTest::~JyuptingTest() = default;

    bool JyuptingTest::convertNumTest() const
    {
        const std::string raw1 = "明月@1几32时有##一";
        const std::string tar1 = "ming jyut jat gei saam ji si jau jat";
        const auto g2pRes1 = g2p_can->hanziToPinyin(raw1, false, true, IKg2p::errorType::Ignore);
        const std::string res1 = IKg2p::join(g2p_can->resToStringList(g2pRes1), " ");
        if (res1 != tar1)
        {
            std::cout << "raw1:" << raw1 << std::endl;
            std::cout << "tar1:" << tar1 << std::endl;
            std::cout << "res1:" << res1 << std::endl;
            return false;
        }

        const std::string raw2 = "明月@1几32时有##一";
        const std::string tar2 = "ming jyut gei si jau jat";
        const auto g2pRes2 = g2p_can->hanziToPinyin(raw1, false, false, IKg2p::errorType::Ignore);
        const std::string res2 = IKg2p::join(g2p_can->resToStringList(g2pRes2), " ");
        if (res2 != tar2)
        {
            std::cout << "raw2:" << raw2 << std::endl;
            std::cout << "tar2:" << tar2 << std::endl;
            std::cout << "res2:" << res2 << std::endl;
            return false;
        }
        return true;
    }

    bool JyuptingTest::removeToneTest() const
    {
        const std::string raw1 = "明月@1几32时有##一";
        const std::string tar1 = "ming4 jyut6 jat1 gei2 saam1 ji6 si4 jau5 jat1";
        const auto g2pRes1 = g2p_can->hanziToPinyin(raw1, true, true, IKg2p::errorType::Ignore);
        const std::string res1 = IKg2p::join(g2p_can->resToStringList(g2pRes1), " ");
        if (res1 != tar1)
        {
            std::cout << "raw1:" << raw1 << std::endl;
            std::cout << "tar1:" << tar1 << std::endl;
            std::cout << "res1:" << res1 << std::endl;
            return false;
        }

        const std::string raw2 = "明月@1几32时有##一";
        const std::string tar2 = "ming4 jyut6 gei2 si4 jau5 jat1";
        const auto g2pRes2 = g2p_can->hanziToPinyin(raw1, true, false, IKg2p::errorType::Ignore);
        const std::string res2 = IKg2p::join(g2p_can->resToStringList(g2pRes2), " ");
        if (res2 != tar2)
        {
            std::cout << "raw2:" << raw2 << std::endl;
            std::cout << "tar2:" << tar2 << std::endl;
            std::cout << "res2:" << res2 << std::endl;
            return false;
        }
        return true;
    }

    bool JyuptingTest::batchTest(const bool& resDisplay) const
    {
        size_t count = 0;
        size_t error = 0;

        const auto dataLines = readData("testData/jyutping_test.txt");

        const auto start = std::chrono::high_resolution_clock::now();

        for (const auto& line : dataLines)
        {
            const auto keyValuePair = IKg2p::split(line, "|");

            if (keyValuePair.size() == 2)
            {
                const std::string& key = keyValuePair[0];
                const std::string& value = keyValuePair[1];

                const auto g2pRes = g2p_can->hanziToPinyin(key, false, true);
                std::string result = IKg2p::join(g2p_can->resToStringList(g2pRes), " ");

                auto words = IKg2p::split(value, " ");
                const auto wordSize = words.size();
                count += wordSize;

                bool diff = false;
                auto resWords = IKg2p::split(result, " ");

                for (int i = 0; i < wordSize; i++)
                {
                    const auto expectedWords = IKg2p::split(words[i], "/");
                    if (std::find(expectedWords.begin(), expectedWords.end(), resWords[i]) == expectedWords.end())
                    {
                        diff = true;
                        error++;
                    }
                }

                if (resDisplay && diff)
                {
                    std::cout << "text: " << key << std::endl;
                    std::cout << "raw: " << value << std::endl;
                    std::cout << "res: " << result << std::endl;
                }
            }
            else
            {
                std::cerr << "Invalid line format: " << line << std::endl;
            }
        }

        const auto end = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        const double percentage = (static_cast<double>(error) / static_cast<double>(count)) * 100.0;

        std::cout << "batchTest: success" << std::endl;
        std::cout << "batchTest time: " << duration << "ms" << std::endl;
        std::cout << "错误率: " << percentage << "%" << std::endl;
        std::cout << "错误数: " << error << std::endl;
        std::cout << "总字数: " << count << std::endl;
        return true;
    }
} // G2pTest
