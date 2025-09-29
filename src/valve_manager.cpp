#include "valve_manager.h"
#include <QDateTime>

// ValueManager implementations

void ValueManager::updateValue(quint32 id, ValueType value) {
    auto &acc = _values[id];

    if (acc.count == 0) {
        acc.sum = 0.0;
        acc.compensation = 0.0;
    }

    const ValueType y = value - acc.compensation;
    const ValueType t = acc.sum + y;
    acc.compensation = (t - acc.sum) - y;
    acc.sum = t;
    ++acc.count;
}

void ValueManager::updateValue(const std::string& name, ValueType value) {
    if(_nameMap.contains(name)) {
        updateValue(_nameMap[name], value);
    }
}

std::optional<ValueType> ValueManager::readValue(quint32 id) {
    const auto it = _values.find(id);
    if(it != _values.end()) {
        const auto &acc = it.value();
        if(acc.count == 0) {
            return acc.lastAverage;
        }

        return (acc.sum + acc.compensation) / static_cast<ValueType>(acc.count);
    }
    return std::nullopt;
}

std::optional<ValueType> ValueManager::readValue(const std::string& name) {
    if(_nameMap.contains(name)) {
        return readValue(_nameMap[name]);
    }
    return std::nullopt;
}

void ValueManager::update() {
    qint64 time = QDateTime::currentMSecsSinceEpoch();

    for(auto it = _values.begin(); it != _values.end(); ++it) {
        const quint32 id = it.key();
        auto &acc = it.value();

        if(acc.count == 0) {
            continue;
        }

        const ValueType average = (acc.sum + acc.compensation) /
                                  static_cast<ValueType>(acc.count);
        acc.lastAverage = average;
        acc.sum = 0.0;
        acc.compensation = 0.0;
        acc.count = 0;

        _valuesHistory[id].push_back({time, acc.lastAverage});
    }
}

void ValueManager::registerName(const std::string& name, quint32 id) {
    _nameMap[name] = id;
    _idMap[id] = name;
    _values[id] = ValueAccumulator{};
}

QVector<std::string> ValueManager::nameList() const {
    return _nameMap.keys();
}

bool ValueManager::hasName(const std::string& name) {
    return _nameMap.contains(name);
}

bool ValueManager::hasId(quint32 id) {
    return _values.contains(id);
}

quint32 ValueManager::id(const std::string& name) {
    return _nameMap[name];
}

std::string ValueManager::name(quint32 id) {
    return _idMap[id];
}
