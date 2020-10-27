#include "codechangehelper.h"
#include <QDir>
#include <QTextCodec>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>

CodeChangeHelper::CodeChangeHelper(QString srcDir, QString filter, QString dstDir, WorkingType ty)
    : QThread(), srcDir(srcDir), dstDir(dstDir), workType(ty)
{
    nameFilter = filter.split(";");
    for (int i = 0; i < nameFilter.size(); ++i)
    {
        // 过滤掉空的
        if (nameFilter[i].isEmpty())
        {
            nameFilter.erase(nameFilter.begin() + i);
            --i;
        }
    }
}

void CodeChangeHelper::run()
{
    // 检查保存目录, 写入权限这些

    // 搜索目录下的所有文件
    QFileInfoList files = getFileInfoList(srcDir);
    emit CurrStateChange(tr("Found %1 Files").arg(files.size()), 0, 0);

    // 循环
    QFile tempFile;
    QByteArray data;
    QString relativePath;
    QFileInfo *tempInfo;
    QDir d(dstDir);
    QTextCodec* curr;
    int totalSize = files.size();
    for (int i = 1; i < totalSize; ++i)
    {
        tempInfo = &files[i];
        emit CurrStateChange(tempInfo->absoluteFilePath(), i, totalSize);

        // 读取文件
        tempFile.setFileName(tempInfo->absoluteFilePath());
        if (!tempFile.open(QIODevice::ReadWrite))
        {
            failList.push_back(*tempInfo);
            emit AddFailInfo(tr("Open Failed, File Name:%1").arg(tempInfo->absoluteFilePath()));
        }

        //QTextStream stream(&tempFile);
        //qDebug() << stream.autoDetectUnicode();
        //qDebug() << stream.codec()->name();
        data = tempFile.readAll();
        //curr = checkDataCode(data);
        //qDebug() << curr->name();

        // 判断保存目录
        if (!dstDir.isEmpty()) {
            tempFile.close();
            // 保存目录为空则表示直接覆盖, 不需要重新打开文件
            // 如果非空, 则需要根据路径创建目录, 并创建文件
            relativePath = tempInfo->absolutePath();
            relativePath = relativePath.mid(srcDir.length() + 1);
            if (!d.mkpath(dstDir + "/" + relativePath)) {
                emit AddFailInfo(tr("Make Path Error, File Name:%1").arg(tempInfo->absoluteFilePath()));
                continue;
            }
            tempFile.setFileName(dstDir + "/" + relativePath + "/" + tempInfo->fileName());
            tempFile.open(QIODevice::WriteOnly);
        }

        //      转换编码
        switch (workType)
        {
        case AnsiToUtf8:
        {
            QTextCodec* code = QTextCodec::codecForName("GBK");
            relativePath = code->toUnicode(data);
            QByteArray b;
            b.append(relativePath.toUtf8());
            tempFile.write(b);
        }
        break;
        case Utf8ToAnsi:
        {
            QTextCodec* code = QTextCodec::codecForName("UTF-8");
            relativePath = code->toUnicode(data);
            QByteArray b;
            b.append(relativePath.toLocal8Bit());
            tempFile.write(b);
        }
        break;
        }
        //QString temp(data);
        //data = temp.toUtf8();
        //      写入文件
        tempFile.close();
    }

    emit CurrStateChange(tr("Totle %1, Success %2, Failed %3").arg(files.length()).arg(files.length() - failList.length()).arg(failList.length()), 1, 1);
}

QFileInfoList CodeChangeHelper::getFileInfoList(QString path)
{
    QFileInfoList out;
    QDir dir = QDir(path);
    
    QFileInfoList files = dir.entryInfoList(nameFilter, QDir::Files);
    out += files;
    emit CurrStateChange(tr("Found %1 Files").arg(out.size()), 0, 0);

    QFileInfoList dirs = dir.entryInfoList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < dirs.size(); ++i) {
        // 递归查找子目录
        out += getFileInfoList(dirs[i].absoluteFilePath());
    }

    return out;
}

// 这个方法功能不正确的
QTextCodec* CodeChangeHelper::checkDataCode(QByteArray data) {
    QTextCodec* c = nullptr;
    QTextCodec::ConverterState state;
    QList<QByteArray> codes = QTextCodec::availableCodecs();
    for (int i = 0; i < codes.size(); ++i) {
        c = QTextCodec::codecForName(codes[i]);
        c->toUnicode(data, data.size(), &state);
        if (state.invalidChars <= 0 && state.remainingChars <= 0) {
            return c;
        }
    }

    return c;
}