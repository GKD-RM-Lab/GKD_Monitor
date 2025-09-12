#include "log_box.h"
#include "ui_logbox.h"
#include <QtWidgets/qpushbutton.h>
#include <QClipboard>

LogBox::LogBox(QWidget *parent)
    : QWidget(parent)
{
    _ui = new Ui::LogBox;
    _ui->setupUi(this);
    connect(_ui->clearBtn,&QPushButton::clicked,[this](){
        _ui->logArea->clear();
    });

    connect(_ui->copyBtn,&QPushButton::clicked,[this](){
        QApplication::clipboard()->setText(_ui->logArea->toPlainText());
    });
}

void LogBox::addText(const QString& text)
{
    _ui->logArea->append(text);
}

