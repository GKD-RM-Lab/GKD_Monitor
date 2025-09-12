#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Reciver;
class QTimer;
class ValueTreeModel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void addChart(const std::string& name);
    void updateValues();

private:
    Ui::MainWindow *ui;
    ValueTreeModel* _valueTreeModel;
    Reciver* _reciver;
    QTimer* _timer;
};

#endif // MAINWINDOW_H
