#ifndef CODECHANGEHELPER_H
#define CODECHANGEHELPER_H
#include <QThread>
#include "def.h"
#include <QStringList>
#include <QFileInfoList>

class CodeChangeHelper : public QThread
{
    Q_OBJECT

public:
    CodeChangeHelper(QString srcDir, QString filter, QString dstDir, WorkingType ty);

    virtual void run();

Q_SIGNALS:
    void CurrStateChange(QString currFile, int w, int t);
    void AddFailInfo(QString info);

private:
    QFileInfoList getFileInfoList(QString path);
    QTextCodec* checkDataCode(QByteArray data);

    QString srcDir;
    QString dstDir;
    WorkingType workType;
    QStringList nameFilter;
    QFileInfoList failList;
};

#endif // CODECHANGEHELPER_H
