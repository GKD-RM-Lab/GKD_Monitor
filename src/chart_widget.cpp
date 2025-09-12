#include <QtCore/qnamespace.h>
#include <QtWidgets/qcombobox.h>

#include "chart_widget.h"
#include "charts.h"
#include "ui_chart_widget.h"

ChartWidget::ChartWidget(std::string name,QWidget* parent)
    : QWidget(parent),
    _chartBase(nullptr)
{
    _ui = new Ui::ChartWidget;
    _ui->setupUi(this);

    _name = name;
    _ui->title->setText(tr("Visualization for %1").arg(QString::fromStdString(name)));

    _chart = new QChart();
    _ui->chart->setChart(_chart);
    _chart->setTitle(tr("Visualization for %1").arg(QString::fromStdString(name)));
    _ui->chart->setRenderHint(QPainter::Antialiasing);
    
    setChartType(Line);

    _ui->lastTime->setValue(_chartBase->lastTime() / 1000);
    connect(_ui->chartType,&QComboBox::currentIndexChanged,[this](int index){
        setChartType((ChartType)index);
    });

    connect(_ui->lastTime,&QSpinBox::valueChanged,[this](int value){
        _chartBase->setLastTime(value * 1000);
    });

    _stoped = false;
    connect(_ui->stop,&QToolButton::clicked,[this](){
        if(_stoped){
            _ui->stop->setText(tr("Stop"));
        }
        else _ui->stop->setText(tr("Resume"));

        _stoped = !_stoped;
    });
}

void ChartWidget::updateChart(){
    if(!_stoped)
        _chartBase->updateValue();
}

void ChartWidget::setChartType(ChartType type)
{
    auto chart = _ui->chart->chart();
    if(_chartBase){
        chart->removeAllSeries();
        chart->removeAxis(_chartBase->axisX());
        chart->removeAxis(_chartBase->axisY());
        _chartBase->deleteLater();
    }
    switch(type){
        case Line:
            _chartBase = new LineChart(_name,chart,this);
            break;
        case Spline:
            _chartBase = new SplineChart(_name,chart,this);
            break;
        case Scatter:
            _chartBase = new ScatterChart(_name,chart,this);
            break;
    }

    chart->addAxis(_chartBase->axisX(),Qt::AlignBottom);
    chart->addAxis(_chartBase->axisY(),Qt::AlignLeft);

    _chartBase->series()->attachAxis(_chartBase->axisX());
    _chartBase->series()->attachAxis(_chartBase->axisY());
}
