#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qwidget.h>

class Reciver;
class QTimer;
class ValueTreeModel;
class LogBox;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    inline LogBox* logBox()const{return _logBox;}

public slots:
    void addChart(const std::string& name);
    void updateValues();

private:
    QDockWidget* createDockWidget(QWidget* widget,const QString& name,bool closable = true);

private:
    Ui::MainWindow *ui;
    ValueTreeModel* _valueTreeModel;
    Reciver* _reciver;
    QTimer* _timer;
    LogBox* _logBox;
};

#endif // MAINWINDOW_H
