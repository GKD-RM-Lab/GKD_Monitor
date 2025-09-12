#include <QDateTime>
#include <QLineSeries>
#include <QScatterSeries>
#include <QSplineSeries>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QtCharts/qabstractaxis.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qnamespace.h>
#include <QtCore/qtypes.h>
#include <QChart>

#include "charts.h"
#include "valve_manager.h"
ChartBase::ChartBase(const std::string &name,QChart* chart,QObject *parent)
    : _name(name)
    , _lastTime(60000)
    ,QObject(parent),
    _chart(chart)
{
}

void ChartBase::updateValue(){ 
    auto opt = valueManager.readValue(_name);
    if (!opt.has_value()) return;
    
    ValueType value = opt.value();

    this->update(QDateTime::currentDateTime(),value);
}

// LineChart
LineChart::LineChart(const std::string &name,QChart* chart,QObject *parent)
    : ChartBase(name,chart,parent)
{
    _series = new QLineSeries(this);
    _series->setName(QString::fromStdString(name));
    _axisX = new QDateTimeAxis(this);
    _axisY = new QValueAxis(this);

    _axisY->setTitleText(QString::fromStdString(name));
    _axisX->setTitleText(tr("Time"));
    
    chart->addSeries(_series);
}

QAbstractSeries *LineChart::series()
{
    return _series;
}

void LineChart::update(QDateTime time,ValueType value){
    int cnt = 0;
    auto points = _series->points();

    while(cnt < points.count() && points[cnt].x() + lastTime() < time.toMSecsSinceEpoch())
        cnt++;

    _series->removePoints(0, cnt);
    _series->append((qreal)time.toMSecsSinceEpoch(), value);

    auto currentDateTime = QDateTime::currentDateTime();
    _axisX -> setRange(currentDateTime.addMSecs(-lastTime()),currentDateTime);
    _axisX -> setFormat("hh:mm:ss:zzz");
    _axisX->setTickCount(5);

    // Adjust Y axis to fit current data
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::lowest();
    const auto curPoints = _series->points();
    for (const auto &p : curPoints) {
        if (p.y() < minY) minY = p.y();
        if (p.y() > maxY) maxY = p.y();
    }
    if (curPoints.isEmpty()) {
        minY = 0.0;
        maxY = 1.0;
    } else if (minY == maxY) {
        minY -= 1.0;
        maxY += 1.0;
    }
    // Add a small margin
    const qreal margin = (maxY - minY) * 0.1;
    _axisY->setRange(minY - margin, maxY + margin);
} 

QAbstractAxis *LineChart::axisX()
{
    return _axisX;
}

QAbstractAxis *LineChart::axisY()
{
    return _axisY;
}

SplineChart::SplineChart(const std::string &name,QChart* chart,QObject *parent)
    : ChartBase(name,chart,parent)
{
    _series = new QSplineSeries(this);
    _series->setName(QString::fromStdString(name));
    _axisX = new QDateTimeAxis(this);
    _axisY = new QValueAxis(this);

    _axisY->setTitleText(QString::fromStdString(name));
    _axisX->setTitleText(tr("Time"));
    
    chart->addSeries(_series);
}

QAbstractSeries *SplineChart::series()
{
    return _series;
}

void SplineChart::update(QDateTime time,ValueType value){
    int cnt = 0;
    auto points = _series->points();

    while(cnt < points.count() && points[cnt].x() + lastTime() < time.toMSecsSinceEpoch())
        cnt++;

    _series->removePoints(0, cnt);
    _series->append((qreal)time.toMSecsSinceEpoch(), value);

    auto currentDateTime = QDateTime::currentDateTime();
    _axisX -> setRange(currentDateTime.addMSecs(-lastTime()),currentDateTime);
    _axisX -> setFormat("hh:mm:ss:zzz");
    _axisX->setTickCount(5);

    // Adjust Y axis to fit current data
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::lowest();
    const auto curPoints = _series->points();
    for (const auto &p : curPoints) {
        if (p.y() < minY) minY = p.y();
        if (p.y() > maxY) maxY = p.y();
    }
    if (curPoints.isEmpty()) {
        minY = 0.0;
        maxY = 1.0;
    } else if (minY == maxY) {
        minY -= 1.0;
        maxY += 1.0;
    }
    // Add a small margin
    const qreal margin = (maxY - minY) * 0.1;
    _axisY->setRange(minY - margin, maxY + margin);
} 

QAbstractAxis *SplineChart::axisX()
{
    return _axisX;
}

QAbstractAxis *SplineChart::axisY()
{
    return _axisY;
}
ScatterChart::ScatterChart(const std::string &name,QChart* chart,QObject *parent)
    : ChartBase(name,chart,parent)
{
    _series = new QScatterSeries(this);
    _series->setName(QString::fromStdString(name));
    _axisX = new QDateTimeAxis(this);
    _axisY = new QValueAxis(this);

    _axisY->setTitleText(QString::fromStdString(name));
    _axisX->setTitleText(tr("Time"));

    _series->setMarkerSize(5);
    _series->setPen(Qt::NoPen);
    
    chart->addSeries(_series);
}

QAbstractSeries *ScatterChart::series()
{
    return _series;
}

void ScatterChart::update(QDateTime time,ValueType value){
    int cnt = 0;
    auto points = _series->points();

    while(cnt < points.count() && points[cnt].x() + lastTime() < time.toMSecsSinceEpoch())
        cnt++;

    _series->removePoints(0, cnt);
    _series->append((qreal)time.toMSecsSinceEpoch(), value);

    auto currentDateTime = QDateTime::currentDateTime();
    _axisX -> setRange(currentDateTime.addMSecs(-lastTime()),currentDateTime);
    _axisX -> setFormat("hh:mm:ss:zzz");
    _axisX->setTickCount(5);

    // Adjust Y axis to fit current data
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::lowest();
    const auto curPoints = _series->points();
    for (const auto &p : curPoints) {
        if (p.y() < minY) minY = p.y();
        if (p.y() > maxY) maxY = p.y();
    }
    if (curPoints.isEmpty()) {
        minY = 0.0;
        maxY = 1.0;
    } else if (minY == maxY) {
        minY -= 1.0;
        maxY += 1.0;
    }
    // Add a small margin
    const qreal margin = (maxY - minY) * 0.1;
    _axisY->setRange(minY - margin, maxY + margin);
} 

QAbstractAxis *ScatterChart::axisX()
{
    return _axisX;
}

QAbstractAxis *ScatterChart::axisY()
{
    return _axisY;
}