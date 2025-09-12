#include "mainwindow.h"
#include "setting_manager.hpp"
#include "ui_mainwindow.h"
#include "valve_manager.h"
#include "chart_widget.h"

#include <QDockWidget>
#include <QTreeView>
#include <QtCore/qnamespace.h>
#include <QtCore/qobjectdefs.h>
#include <QTimer>
#include <QTime>
#include <QtWidgets/qdockwidget.h>

#include "value_tree_model.h"
#include "reciver.h"
MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindow),
    _valueTreeModel(new ValueTreeModel(this)) {
    ui->setupUi(this);
    SettingManager::init("config.ini");

    this->_reciver = new Reciver(this);
    _reciver->listen(SettingManager::getValue<int>("port", 8080));
    
    // Create dock widget
    QDockWidget* dockWidget = new QDockWidget("Value Tree", this);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    
    // Create tree view
    QTreeView* treeView = new QTreeView(dockWidget);
    treeView->setModel(_valueTreeModel);
    treeView->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *actionVisualize = new QAction(tr("Visualize"), treeView);
    treeView->addAction(actionVisualize);
    connect(actionVisualize, &QAction::triggered, [this,treeView](bool checked){
        auto index = treeView->selectionModel()->currentIndex();
        if(!index.isValid())
          return;

        std::string name = index.data(Qt::UserRole).toString().toStdString();

        if(valueManager.hasName(name))
            addChart(name);
    });
    
    // Set up dock widget
    dockWidget->setWidget(treeView);
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);

    _timer = new QTimer(this);
    _timer->setInterval(100);
    connect(_timer, &QTimer::timeout, this, &MainWindow::updateValues);
    // connect(_timer, &QTimer::timeout, [this](){
    //   valueManager.updateValue("aaa.bbb",QTime::currentTime().second());
    //   valueManager.updateValue("aaa.b1bb",QTime::currentTime().second()+1);
    // });

    _timer->start();
}

void MainWindow::updateValues() {
    _valueTreeModel->update();
    
    ValueManager::instance().update();
}

void MainWindow::addChart(const std::string& name) {
    auto chart = new ChartWidget(name,this);
    
    QDockWidget* dockWidget = new QDockWidget(name.c_str(), this);
    dockWidget->setWidget(chart);

    addDockWidget(Qt::RightDockWidgetArea, dockWidget);
    connect(_timer, &QTimer::timeout, chart, &ChartWidget::updateChart);
}

MainWindow::~MainWindow() {
    delete ui;
}
