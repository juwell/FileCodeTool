#include "FileCodeTool.h"

#include <QFileDialog>
#include "codechangehelper.h"

FileCodeToolDialog::FileCodeToolDialog(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::FileCodeToolDialog),
    workThread(nullptr)
{
    ui->setupUi(this);

    this->setWindowTitle(tr(AppName) + "v" + Version);
    ui->WorkingLabel->setText("");

    connect(ui->SrcDirBtn, SIGNAL(clicked()), this, SLOT(SrcBtnClick()));
    connect(ui->DstDirBtn, SIGNAL(clicked()), this, SLOT(DstBtnClick()));
    connect(ui->OKBtn, SIGNAL(clicked()), this, SLOT(OKBtnClick()));
    connect(ui->AToURadio, SIGNAL(toggled(bool)), this, SLOT(ChangeTypeToggled(bool)));
    connect(ui->UToARadio, SIGNAL(toggled(bool)), this, SLOT(ChangeTypeToggled(bool)));

    ui->AToURadio->setProperty("typeEnum", AnsiToUtf8);
    ui->UToARadio->setProperty("typeEnum", Utf8ToAnsi);
    workType = AnsiToUtf8;
}

FileCodeToolDialog::~FileCodeToolDialog() {
    delete ui;

    if (workThread) {
        workThread->terminate();
        delete workThread;
    }
}

void FileCodeToolDialog::SrcBtnClick() {
    srcDir = QFileDialog::getExistingDirectory(this, tr("File Dir"));
    ui->SrcDirEdit->setText(srcDir);
}

void FileCodeToolDialog::DstBtnClick() {
    dstDir = QFileDialog::getExistingDirectory(this, tr("Save Dir"));
    ui->DstDirEdit->setText(dstDir);
}

void FileCodeToolDialog::UpdateWorkingShow(QString currFile, int w, int t) {
    ui->WorkingLabel->setText(currFile);
    ui->Progress->setValue(w);
    ui->Progress->setMaximum(t);
}

void FileCodeToolDialog::OKBtnClick() {
    ui->OKBtn->setDisabled(true);
    QString filter = ui->FileTypeCombo->currentText();
    if (workThread) {
        workThread->terminate();
        workThread->disconnect();
        delete workThread;
    }
    srcDir = ui->SrcDirEdit->text();
    dstDir = ui->DstDirEdit->text();
    workThread = new CodeChangeHelper(srcDir, filter, dstDir, workType);
    workThread->start();
    connect(workThread, SIGNAL(finished()), this, SLOT(WorkThreadFinished()));
    connect(workThread, SIGNAL(CurrStateChange(QString, int, int)), this, SLOT(UpdateWorkingShow(QString, int, int)));
    connect(workThread, SIGNAL(AddFailInfo(QString)), this, SLOT(UpdateFailShow(QString)));
}

void FileCodeToolDialog::ChangeTypeToggled(bool checked) {
    if (!checked) {
        return;
    }

    auto r = sender();
    workType = WorkingType(r->property("typeEnum").toInt());
}

void FileCodeToolDialog::WorkThreadFinished() {
    if (workThread) {
        workThread->disconnect(SIGNAL(finished()));
        delete workThread;
        workThread = nullptr;
    }

    ui->OKBtn->setEnabled(true);
}

void FileCodeToolDialog::UpdateFailShow(QString txt) {
    ui->FailListWidget->addItem(txt);
}