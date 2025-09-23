#pragma once
#include <QWidget>
#include <QtCore/qobject.h>

namespace Ui{
    class ChartWidget;
}

class ChartBase;
class QChart;

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
    QChart *_chart;
    bool _stoped;
};