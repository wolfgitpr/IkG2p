#include "Common.h"

#include <codecvt>
#include <iostream>
#include <fstream>

#include "../src/StringUtil.h"

namespace G2pTest
{
    std::vector<std::string> readData(const std::string& filename)
    {
        std::vector<std::string> dataLines;

#ifdef _WIN32
        // Convert the UTF-8 string to a wide string
        std::ifstream file(IKg2p::utf8ToWide(filename));
#else
        std::ifstream file(filename);
#endif

        std::string line;

        if (file.is_open())
        {
            while (std::getline(file, line))
            {
                dataLines.push_back(line);
            }
            file.close();
        }
        else
        {
            std::cerr << "Unable to open file: " << filename << std::endl;
        }

        return dataLines;
    }
}
