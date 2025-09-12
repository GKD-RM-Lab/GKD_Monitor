#include "chart_widget.h"
#include "charts.h"
#include "ui_chart_widget.h"
#include <QtCore/qnamespace.h>
#include <QtWidgets/qcombobox.h>
#include <utility>

ChartWidget::ChartWidget(std::string name,QWidget* parent)
    : QWidget(parent),
    _chartBase(nullptr)
{
    _ui = new Ui::ChartWidget;
    _ui->setupUi(this);

    _name = name;
    _ui->title->setText(QString::fromStdString(name));
    _ui->chart->setChart(new QChart());

    _ui->chart->chart()->setTitle(tr("Visualization for %1").arg(QString::fromStdString(name)));
    
    setChartType(Line);

    _ui->lastTime->setValue(_chartBase->lastTime() / 1000);
    connect(_ui->chartType,&QComboBox::currentIndexChanged,[this](int index){
        setChartType((ChartType)index);
    });

    connect(_ui->lastTime,&QSpinBox::valueChanged,[this](int value){
        _chartBase->setLastTime(value * 1000);
    });
}

void ChartWidget::updateChart(){
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
        default:
            std::unreachable();
    }

    chart->addAxis(_chartBase->axisX(),Qt::AlignBottom);
    chart->addAxis(_chartBase->axisY(),Qt::AlignLeft);

    _chartBase->series()->attachAxis(_chartBase->axisX());
    _chartBase->series()->attachAxis(_chartBase->axisY());
}
