#pragma once
#include "singleton.hpp"
#include <QVariant>
#include <optional>
#include <cstdint>
#include <QMap>
#include <QDateTime>
#include <QtGlobal>
#include <string>

using ValueType = double;
using TimeValuePair = QPair<qint64,ValueType>;

class ValueManager : public Singleton<ValueManager>
{
    ValueManager() = default;
    friend class Singleton<ValueManager>;
public:
    // 用每次存取间隔的数据的平均值
    void updateValue(quint32 id, ValueType value);
    void updateValue(const std::string& name, ValueType value);
    std::optional<ValueType> readValue(quint32 id);
    std::optional<ValueType> readValue(const std::string& name);
    void update();
    void registerName(const std::string& name, quint32 id);
    QVector<std::string> nameList() const;
    bool hasName(const std::string& name);
    bool hasId(quint32 id);
    quint32 id(const std::string& name);
    std::string name(quint32 id);

private:
    struct ValueAccumulator {
        ValueType sum{0.0};
        ValueType compensation{0.0};
        ValueType lastAverage{0.0};
        quint64 count{0};
    };

    QMap<quint32, ValueAccumulator> _values;
    QMap<quint32,QVector<TimeValuePair>> _valuesHistory;
    QMap<std::string,quint32> _nameMap;
    QMap<quint32,std::string> _idMap;
};

#define valueManager (ValueManager::instance())
