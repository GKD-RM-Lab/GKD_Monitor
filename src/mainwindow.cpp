#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "valve_manager.h"

#include <QDockWidget>
#include <QTreeView>
#include <QtCore/qobjectdefs.h>
#include <QTimer>
#include <QTime>
MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindow),
    valueTreeModel(new ValueTreeModel(this)) {
    ui->setupUi(this);
    
    // Create dock widget
    QDockWidget* dockWidget = new QDockWidget("Value Tree", this);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    
    // Create tree view
    QTreeView* treeView = new QTreeView(dockWidget);
    treeView->setModel(valueTreeModel);
    
    // Set up dock widget
    dockWidget->setWidget(treeView);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget);

    QTimer* timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, &QTimer::timeout, valueTreeModel, &ValueTreeModel::update);
    connect(timer, &QTimer::timeout, [this](){
      valueManager.set("aaa.bbb",QTime::currentTime().second());
      valueManager.set("aaa.b1bb",QTime::currentTime().second()+1);
    });

    timer->start();

}

MainWindow::~MainWindow() {
    delete ui;
}
