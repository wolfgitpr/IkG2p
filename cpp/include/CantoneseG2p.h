#ifndef DATASET_TOOLS_CANTONESE_H
#define DATASET_TOOLS_CANTONESE_H

#include "ChineseG2p.h"

namespace IKg2p
{
    class CantoneseG2p final : public ChineseG2p
    {
    public:
        explicit CantoneseG2p() : ChineseG2p("cantonese")
        {
        };
        ~CantoneseG2p();
    };
}
#endif // DATASET_TOOLS_CANTONESE_H
