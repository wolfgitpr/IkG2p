#ifndef DATASET_TOOLS_MANDARIN_H
#define DATASET_TOOLS_MANDARIN_H

#include "ChineseG2p.h"

namespace IKg2p
{
    class MandarinG2p final : public ChineseG2p
    {
    public:
        explicit MandarinG2p() : ChineseG2p("mandarin")
        {
        };
        ~MandarinG2p() = default;
    };
}

#endif // DATASET_TOOLS_MANDARIN_H
