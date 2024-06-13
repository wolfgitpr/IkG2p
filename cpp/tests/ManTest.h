#ifndef DATASET_TOOLS_MANTEST_H
#define DATASET_TOOLS_MANTEST_H

#include "G2pglobal.h"
#include "MandarinG2p.h"

namespace G2pTest
{
    class ManTest
    {
    public:
        explicit ManTest();
        ~ManTest();
        bool apiTest() const;
        bool convertNumTest() const;
        bool removeToneTest() const;
        bool batchTest(bool resDisplay = false) const;

    private:
        std::unique_ptr<IKg2p::ChineseG2p> g2p_zh;
    };
} // G2pTest

#endif // DATASET_TOOLS_MANTEST_H
