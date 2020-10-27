#pragma once

#include <QtWidgets/QDialog>
#include "ui_filecodetooldialog.h"
#include "def.h"

namespace Ui {
    class FileCodeToolDialog;
}

class CodeChangeHelper;
class FileCodeToolDialog: public QDialog {
    Q_OBJECT

public:
    explicit FileCodeToolDialog(QWidget* parent = 0);
    ~FileCodeToolDialog();

private slots:
    void SrcBtnClick();
    void DstBtnClick();
    void UpdateWorkingShow(QString currFile, int w, int t);
    void OKBtnClick();
    void ChangeTypeToggled(bool checked);
    void WorkThreadFinished();
    void UpdateFailShow(QString txt);

private:
    Ui::FileCodeToolDialog* ui;

    QString srcDir;
    QString dstDir;
    WorkingType workType;
    CodeChangeHelper* workThread;
};