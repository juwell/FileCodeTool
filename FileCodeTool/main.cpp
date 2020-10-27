#include "FileCodeTool.h"
#include <QtWidgets/QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator* t = new QTranslator();
    bool state = t->load("zh_cn.qm", "translations");
    a.installTranslator(t);
    FileCodeToolDialog w;
    w.show();
    return a.exec();
}
