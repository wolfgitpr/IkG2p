#include <filesystem>

#include "G2pglobal.h"
#include "MandarinG2p.h"
#include <iostream>
#include <sstream>

#include "JyuptingTest.h"
#include "ManTest.h"

using namespace G2pTest;

int main(int argc, char* argv[])
{
    const std::string applicationDirPath = std::filesystem::current_path().string();
    IKg2p::setDictionaryPath(applicationDirPath + "\\dict");

    const ManTest manTest;
    std::cout << "MandarinG2p G2P test: " << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << "apiTest: " << manTest.apiTest() << std::endl;
    std::cout << "convertNumTest: " << manTest.convertNumTest() << std::endl;
    std::cout << "removeToneTest: " << manTest.removeToneTest() << std::endl;
    // manTest.batchTest();
    std::cout << "--------------------\n" << std::endl;

    const JyuptingTest jyuptingTest;
    std::cout << "Cantonese G2P test:" << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << "convertNumTest: " << jyuptingTest.convertNumTest() << std::endl;
    std::cout << "removeToneTest: " << jyuptingTest.removeToneTest() << std::endl;
    jyuptingTest.batchTest();
    std::cout << "--------------------\n" << std::endl;

    std::cout << "G2P mix test:" << std::endl;
    std::cout << "--------------------" << std::endl;
    const auto g2p_man = std::make_unique<IKg2p::MandarinG2p>();

    const std::string raw2 =
        "举杯あャ坐ュ饮放あ歌竹林间/清风拂 面悄word然xax asx a xxs拨？Q！动初弦/便推开烦恼与尘喧/便还是当时的少年";
    const auto res1 = IKg2p::MandarinG2p::resToStringList(g2p_man->hanziToPinyin(raw2, false, false));
    for (const auto& str : res1)
    {
        std::cout << str << " ";
    }
    std::cout << std::endl;

    std::cout << "--------------------" << std::endl;
    const auto res2 = IKg2p::MandarinG2p::resToStringList(
        g2p_man->hanziToPinyin(raw2, false, false, IKg2p::errorType::Ignore));
    for (const auto& str : res2)
    {
        std::cout << str << " ";
    }
    std::cout << std::endl;
    return 0;
}
