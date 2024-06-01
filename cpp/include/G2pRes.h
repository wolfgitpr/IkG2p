#ifndef G2PRES_H
#define G2PRES_H

#include <QString>
#include <QStringList>

namespace IKg2p
{
    struct G2pRes
    {
        QString lyric;
        QString syllable = QString();
        QStringList candidates = QStringList();
        bool error = true;
    };
}
#endif //G2PRES_H
