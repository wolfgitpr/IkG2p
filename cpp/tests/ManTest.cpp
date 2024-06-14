#include "ManTest.h"

#include <chrono>
#include <iostream>

#include "Common.h"

namespace G2pTest
{
    ManTest::ManTest() : g2p_zh(std::make_unique<IKg2p::MandarinG2p>())
    {
    }

    ManTest::~ManTest() = default;

    bool ManTest::apiTest() const
    {
        if (!g2p_zh->isPolyphonic("的"))
            return false;
        if (g2p_zh->isPolyphonic("犬"))
            return false;
        if (g2p_zh->tradToSim("臺") != "台")
            return false;
        if (g2p_zh->tradToSim("犬") != "犬")
            return false;
        if (!(g2p_zh->getDefaultPinyin("杆").at(0) == "gan3" && g2p_zh->getDefaultPinyin("杆").at(1) == "gan1"))
            return false;
        if (g2p_zh->getDefaultPinyin("杆", false).at(0) != "gan")
            return false;
        return true;
    }

    bool ManTest::convertNumTest() const
    {
        const std::string raw1 = "明月@1几32时有##一";
        const std::string tar1 = "ming yue yi ji san er shi you yi";
        const auto g2pRes1 = g2p_zh->hanziToPinyin(raw1, false, true, IKg2p::errorType::Ignore);
        const std::string res1 = IKg2p::join(g2p_zh->resToStringList(g2pRes1), " ");
        const auto g2pRes1List = g2p_zh->hanziToPinyin(raw1, false, true, IKg2p::errorType::Ignore);
        const std::string res1List = IKg2p::join(g2p_zh->resToStringList(g2pRes1List), " ");
        if (res1 != tar1 | res1List != tar1)
        {
            std::cout << "raw1:" << raw1 << std::endl;
            std::cout << "tar1:" << tar1 << std::endl;
            std::cout << "res1:" << res1 << std::endl;
            return false;
        }

        const std::string raw2 = "明月@1几32时有##一";
        const std::string tar2 = "ming yue ji shi you yi";
        const auto g2pRes2 = g2p_zh->hanziToPinyin(raw1, false, false, IKg2p::errorType::Ignore);
        const std::string res2 = IKg2p::join(g2p_zh->resToStringList(g2pRes2), " ");
        const auto g2pRes2List = g2p_zh->hanziToPinyin(raw1, false, false, IKg2p::errorType::Ignore);
        const std::string res2List = IKg2p::join(g2p_zh->resToStringList(g2pRes2List), " ");
        if (res2 != tar2 | res2List != tar2)
        {
            std::cout << "raw2:" << raw2 << std::endl;
            std::cout << "tar2:" << tar2 << std::endl;
            std::cout << "res2:" << res2 << std::endl;
            return false;
        }
        return true;
    }

    bool ManTest::removeToneTest() const
    {
        const std::string raw1 = "明月@1几32时有##一";
        const std::string tar1 = "ming2 yue4 yi1 ji3 san1 er4 shi2 you3 yi1";
        const auto g2pRes1 = g2p_zh->hanziToPinyin(raw1, true, true, IKg2p::errorType::Ignore);
        const std::string res1 = IKg2p::join(g2p_zh->resToStringList(g2pRes1), " ");
        if (res1 != tar1)
        {
            std::cout << "raw1:" << raw1 << std::endl;
            std::cout << "tar1:" << tar1 << std::endl;
            std::cout << "res1:" << res1 << std::endl;
            return false;
        }

        const std::string raw2 = "明月@1几32时有##一";
        const std::string tar2 = "ming2 yue4 ji3 shi2 you3 yi1";
        const auto g2pRes2 = g2p_zh->hanziToPinyin(raw1, true, false, IKg2p::errorType::Ignore);
        const std::string res2 = IKg2p::join(g2p_zh->resToStringList(g2pRes2), " ");
        if (res2 != tar2)
        {
            std::cout << "raw2:" << raw2 << std::endl;
            std::cout << "tar2:" << tar2 << std::endl;
            std::cout << "res2:" << res2 << std::endl;
            return false;
        }
        return true;
    }

    bool ManTest::batchTest(bool resDisplay) const
    {
        size_t count = 0;
        size_t error = 0;

        const auto dataLines = readData("testData/op_lab.txt");

        const auto start = std::chrono::high_resolution_clock::now();
        for (const auto& line : dataLines)
        {
            const auto keyValuePair = IKg2p::split(line, "|");

            if (keyValuePair.size() == 2)
            {
                const std::string& key = keyValuePair[0];
                const std::string& value = keyValuePair[1];

                auto g2pRes = g2p_zh->hanziToPinyin(key, false, true);
                std::string result = IKg2p::join(g2p_zh->resToStringList(g2pRes), " ");

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
}
