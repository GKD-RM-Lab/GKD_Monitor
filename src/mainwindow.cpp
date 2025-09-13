#include <QDockWidget>
#include <QTreeView>
#include <QtCore/qnamespace.h>
#include <QtCore/qobjectdefs.h>
#include <QTimer>
#include <QTime>
#include <QtWidgets/qdockwidget.h>

#include "value_tree_model.h"
#include "reciver.h"
#include "log_box.h"
#include "mainwindow.h"
#include "setting_manager.hpp"
#include "ui_mainwindow.h"
#include "valve_manager.h"
#include "chart_widget.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindow),
    _valueTreeModel(new ValueTreeModel(this)) {
    ui->setupUi(this);
    SettingManager::init("config.ini");

    this->_reciver = new Reciver(this);
    _reciver->listen(SettingManager::getValue<int>("port", 8080));
    _reciver->setMainWindow(this);
    
    // Create tree view
    QTreeView* treeView = new QTreeView();
    treeView->setModel(_valueTreeModel);
    treeView->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *actionVisualize = new QAction(tr("Visualize"), treeView);
    treeView->addAction(actionVisualize);
    connect(actionVisualize, &QAction::triggered, [this,treeView](bool checked){
        auto index = treeView->selectionModel()->currentIndex();
        if(!index.isValid())
          return;

        quint32 id = index.data(Qt::UserRole).toUInt();
        auto name = valueManager.name(id);

        if(valueManager.hasId(id))
            addChart(name);
    });

    connect(treeView, &QTreeView::doubleClicked, [this,treeView](const QModelIndex& index){ 
        if(!index.isValid())
          return;

        std::string name = index.data(Qt::UserRole).toString().toStdString();

        if(valueManager.hasName(name))
            addChart(name);
    });


    _logBox = new LogBox(this);
    auto logBoxDock = createDockWidget(_logBox,tr("Log"),false);
    logBoxDock->setDockLocation(Qt::DockWidgetArea::BottomDockWidgetArea);
    
    auto tvDock = createDockWidget(treeView,tr("Value Tree"),false);
    tvDock->setDockLocation(Qt::DockWidgetArea::LeftDockWidgetArea);

    _timer = new QTimer(this);
    _timer->setInterval(100);
    connect(_timer, &QTimer::timeout, this, &MainWindow::updateValues);

    auto t = new QTimer(this);

    valueManager.registerName("aaa.bbb", 1);
    valueManager.registerName("aaa.b1bb", 2);
    connect(t, &QTimer::timeout, [this](){
      valueManager.updateValue("aaa.bbb",sin(QTime::currentTime().msec()));
      valueManager.updateValue("aaa.b1bb",QTime::currentTime().msec()+1);

      _logBox->addText("111\n");
    });

    t->setInterval(10);
    t->start();

    _timer->start();
}

void MainWindow::updateValues() {
    _valueTreeModel->update();
    
    ValueManager::instance().update();
}

QDockWidget* MainWindow::createDockWidget(QWidget* widget,const QString& name,bool closable){
    QDockWidget* dock = new QDockWidget(name,this);
    dock->setWidget(widget);
    widget->setParent(dock);
    auto features = QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable;
    if(closable)
        features |= QDockWidget::DockWidgetClosable;

    dock->setFeatures(features);
    addDockWidget(Qt::RightDockWidgetArea,dock);

    return dock;
}

void MainWindow::addChart(const std::string& name) {
    auto chart = new ChartWidget(name,this);
    
    createDockWidget(chart, tr("Visualization for %1").arg(QString::fromStdString(name)));
    connect(_timer, &QTimer::timeout, chart, &ChartWidget::updateChart);
}

MainWindow::~MainWindow() {
    delete ui;
}
