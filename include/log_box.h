#pragma once

#include <QWidget>

namespace Ui{
    class LogBox ;
}

class LogBox : public QWidget
{ 
    Q_OBJECT
public:
    explicit LogBox(QWidget *parent = nullptr);

public slots:
    void addText(const QString& text);

private:
    Ui::LogBox *_ui;
};