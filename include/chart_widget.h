#pragma once
#include "charts.h"
#include <QChartView>
#include <QtCore/qobject.h>

namespace Ui{
    class ChartWidget;
}

class ChartWidget : public QWidget{
    Q_OBJECT
public:
    ChartWidget(std::string name,QWidget* parent = nullptr);

    enum ChartType{
        Line,
        Spline,
        Scatter
    };

public slots:
    void updateChart();
    void setChartType(ChartType type);

private:
    std::string _name;
    Ui::ChartWidget* _ui;
    ChartBase * _chartBase;
};