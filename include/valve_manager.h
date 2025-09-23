#pragma once
#include "singleton.hpp"
#include <QVariant>
#include <optional>
#include <cstdint>
#include <QMap>
#include <QDateTime>
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
    QMap<quint32, ValueType> _valuesSum;
    QMap<quint32, std::uint32_t> _valuesCnt;

    QMap<quint32,QVector<TimeValuePair>> _valuesHistory;
    QMap<std::string,quint32> _nameMap;
    QMap<quint32,std::string> _idMap;
};

#define valueManager (ValueManager::instance())