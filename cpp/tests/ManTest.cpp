#include "ManTest.h"
#include "Common.h"

#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QTextCodec>

namespace G2pTest
{
    ManTest::ManTest() : g2p_zh(new IKg2p::MandarinG2p())
    {
    }

    ManTest::~ManTest()
    {
    }

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
        qDebug() << "apiTest: success";
        return true;
    }

    bool ManTest::convertNumTest()
    {
        const QString raw1 = "明月@1几32时有##一";
        const QString tar1 = "ming yue yi ji san er shi you yi";
        const auto g2pRes1 = g2p_zh->hanziToPinyin(raw1, false, true, IKg2p::errorType::Ignore);
        const QString res1 = g2p_zh->resToStringList(g2pRes1).join(" ");
        const auto g2pRes1List = g2p_zh->hanziToPinyin(raw1.split(""), false, true, IKg2p::errorType::Ignore);
        const QString res1List = g2p_zh->resToStringList(g2pRes1List).join(" ");
        if (res1 != tar1 | res1List != tar1)
        {
            qDebug() << "raw1:" << raw1;
            qDebug() << "tar1:" << tar1;
            qDebug() << "res1:" << res1;
            return false;
        }

        const QString raw2 = "明月@1几32时有##一";
        const QString tar2 = "ming yue ji shi you yi";
        const auto g2pRes2 = g2p_zh->hanziToPinyin(raw1, false, false, IKg2p::errorType::Ignore);
        const QString res2 = g2p_zh->resToStringList(g2pRes2).join(" ");
        const auto g2pRes2List = g2p_zh->hanziToPinyin(raw1.split(""), false, false, IKg2p::errorType::Ignore);
        const QString res2List = g2p_zh->resToStringList(g2pRes2List).join(" ");
        if (res2 != tar2 | res2List != tar2)
        {
            qDebug() << "raw2:" << raw2;
            qDebug() << "tar2:" << tar2;
            qDebug() << "res2:" << res2;
            return false;
        }

        qDebug() << "convertNumTest: success";
        return true;
    }

    bool ManTest::removeToneTest()
    {
        const QString raw1 = "明月@1几32时有##一";
        const QString tar1 = "ming2 yue4 yi1 ji3 san1 er4 shi2 you3 yi1";
        const auto g2pRes1 = g2p_zh->hanziToPinyin(raw1, true, true, IKg2p::errorType::Ignore);
        const QString res1 = g2p_zh->resToStringList(g2pRes1).join(" ");
        if (res1 != tar1)
        {
            qDebug() << "raw1:" << raw1;
            qDebug() << "tar1:" << tar1;
            qDebug() << "res1:" << res1;
            return false;
        }

        const QString raw2 = "明月@1几32时有##一";
        const QString tar2 = "ming2 yue4 ji3 shi2 you3 yi1";
        const auto g2pRes2 = g2p_zh->hanziToPinyin(raw1, true, false, IKg2p::errorType::Ignore);
        const QString res2 = g2p_zh->resToStringList(g2pRes2).join(" ");
        if (res2 != tar2)
        {
            qDebug() << "raw2:" << raw2;
            qDebug() << "tar2:" << tar2;
            qDebug() << "res2:" << res2;
            return false;
        }

        qDebug() << "removeToneTest: success";
        return true;
    }

    bool ManTest::batchTest(bool resDisplay)
    {
        int count = 0;
        int error = 0;

        QStringList dataLines = readData(qApp->applicationDirPath() + "/testData/op_lab.txt");

        QElapsedTimer time;
        time.start();
        foreach(const QString &line, dataLines)
        {
            QStringList keyValuePair = line.trimmed().split('|');

            if (keyValuePair.size() == 2)
            {
                QString key = keyValuePair[0];

                QString value = keyValuePair[1];
                auto g2pRes = g2p_zh->hanziToPinyin(key, false, true);
                QString result = g2p_zh->resToStringList(g2pRes).join(" ");

                QStringList words = value.split(" ");
                int wordSize = words.size();
                count += wordSize;

                bool diff = false;
                QStringList resWords = result.split(" ");
                for (int i = 0; i < wordSize; i++)
                {
                    if (words[i] != resWords[i] && !words[i].split("/").contains(resWords[i]))
                    {
                        diff = true;
                        error++;
                    }
                }

                if (resDisplay && diff)
                {
                    qDebug() << "text: " << key;
                    qDebug() << "raw: " << value;
                    qDebug() << "res: " << result;
                }
            }
            else
            {
                qDebug() << keyValuePair;
            }
        }

        double percentage = ((double)error / (double)count) * 100.0;

        qDebug() << "batchTest: success";
        qDebug() << "batchTest time:" << time.elapsed() << "ms";
        qDebug() << "错误率: " << percentage << "%";
        qDebug() << "错误数: " << error;
        qDebug() << "总字数: " << count;
        return true;
    }
} // G2pTest
