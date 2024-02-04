#include "syllable2p.h"
#include "syllable2p_p.h"
#include <QDebug>
#include <utility>

#include "g2pglobal.h"

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#  include <QtCore5Compat>
#endif

namespace IKg2p {
    Syllable2pPrivate::Syllable2pPrivate(QString phonemeDict, QString sep1, QString sep2) : phonemeDict(
            std::move(phonemeDict)), sep1(std::move(sep1)), sep2(std::move(sep2)) {
    }

    Syllable2pPrivate::~Syllable2pPrivate() {
    }

    void Syllable2pPrivate::init() {
        auto dict_dir = dictionaryPath();
        loadDict(dict_dir, "syllable2p.txt", phonemeMap, sep1, sep2);
    }

    Syllable2p::Syllable2p(QString phonemeDict, QString sep1, QString sep2, QObject *parent) : Syllable2p(
            *new Syllable2pPrivate(std::move(phonemeDict), std::move(sep1), std::move(sep2)), parent) {
    }

    Syllable2p::~Syllable2p() {
    }

    QList<QStringList> Syllable2p::syllableToPhoneme(const QStringList &syllables) const {
        Q_D(const Syllable2p);
        QList<QStringList> phonemeList;
        for (const QString &word: syllables) {
            phonemeList.append(d->phonemeMap.value(word, QStringList()));
        }
        return phonemeList;
    }


    QStringList Syllable2p::syllableToPhoneme(const QString &syllable) const {
        Q_D(const Syllable2p);
        return d->phonemeMap.value(syllable, QStringList());
    }

    Syllable2p::Syllable2p(Syllable2pPrivate &d, QObject *parent) : QObject(parent), d_ptr(&d) {
        d.q_ptr = this;

        d.init();
    }
}
