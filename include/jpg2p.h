#ifndef JPG2P_H
#define JPG2P_H

#include <QObject>

namespace IKg2p {

    class JpG2pPrivate;

    class JpG2p : public QObject {
        Q_OBJECT
        Q_DECLARE_PRIVATE(JpG2p)
    public:
        explicit JpG2p(QObject *parent = nullptr);
        ~JpG2p() override;

        QStringList kanaToRomaji(const QString &kanaStr, bool doubleWrittenSokuon = false) const;
        QStringList kanaToRomaji(const QStringList &kanaList, bool doubleWrittenSokuon = false) const;

        QStringList romajiToKana(const QString &romajiStr) const;
        QStringList romajiToKana(const QStringList &romajiList) const;

    protected:
        explicit JpG2p(JpG2pPrivate &d, QObject *parent = nullptr);

        QScopedPointer<JpG2pPrivate> d_ptr;
    };

}

#endif // JPG2P_H