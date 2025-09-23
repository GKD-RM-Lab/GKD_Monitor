#pragma once
#include "valve_manager.h"
#include <QQueue>

class QAbstractAxis;
class QAbstractSeries;
class QLineSeries;
class QScatterSeries;
class QSplineSeries;
class QDateTimeAxis;
class QValueAxis;
class QChart;

class ChartBase : public QObject {
    Q_OBJECT
public:
    ChartBase(const std::string& name,QChart* chart,QObject *parent = nullptr);

    virtual QAbstractSeries* series() = 0;
    virtual QAbstractAxis* axisX() = 0;
    virtual QAbstractAxis* axisY() = 0;

    using TimeValuePair = QPair<qint64,ValueType>;

    virtual void update(QDateTime time,ValueType value) = 0;
    inline void setLastTime(qint64 time)
    {
        _lastTime = time;
    }

    inline qint64 lastTime() const
    {
        return _lastTime;
    }

    inline std::string name() const{return _name;}
    inline QChart* chart()const{return _chart;}

public slots:
    void updateValue();

private:
    std::string _name;
    quint32 _id;
    QQueue<TimeValuePair> _values;
    qint64 _lastTime;
    QChart* _chart;
};

class LineChart : public ChartBase{
    Q_OBJECT
public:
    LineChart(const std::string& name,QChart* chart,QObject *parent = nullptr);

    QAbstractSeries* series() override;
    QAbstractAxis* axisX() override;
    QAbstractAxis* axisY() override;

    void update(QDateTime time,ValueType value) override;
private:
    QLineSeries *_series;
    QDateTimeAxis *_axisX;
    QValueAxis *_axisY;
};

class SplineChart : public ChartBase{
    Q_OBJECT
public:
    SplineChart(const std::string& name,QChart* chart,QObject *parent = nullptr);

    QAbstractSeries* series() override;
    QAbstractAxis* axisX() override;
    QAbstractAxis* axisY() override;

    void update(QDateTime time,ValueType value) override;
private:
    QSplineSeries *_series;
    QDateTimeAxis *_axisX;
    QValueAxis *_axisY;
};

class ScatterChart : public ChartBase{
    Q_OBJECT
public:
    ScatterChart(const std::string& name,QChart* chart,QObject *parent = nullptr);

    QAbstractSeries* series() override;
    QAbstractAxis* axisX() override;
    QAbstractAxis* axisY() override;

    void update(QDateTime time,ValueType value) override;
private:
    QScatterSeries *_series;
    QDateTimeAxis *_axisX;
    QValueAxis *_axisY;
};