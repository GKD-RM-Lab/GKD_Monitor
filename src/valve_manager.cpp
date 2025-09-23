#include "valve_manager.h"
#include <QDateTime>

// ValueManager implementations

void ValueManager::updateValue(quint32 id, ValueType value) {
    if(_valuesCnt.find(id) == _valuesCnt.end()) {
        _valuesCnt[id] = 1;
        _valuesSum[id] = value;
    }
    else {
        if(_valuesCnt[id] == 0) {
            _valuesSum[id] = 0;
        }
        _valuesCnt[id]++;
        _valuesSum[id] += value;
    }
}

void ValueManager::updateValue(const std::string& name, ValueType value) {
    if(_nameMap.contains(name)) {
        updateValue(_nameMap[name], value);
    }
}

std::optional<ValueType> ValueManager::readValue(quint32 id) {
    if(_valuesCnt.find(id) != _valuesCnt.end()) {
        if(_valuesCnt[id] == 0)
            return _valuesSum[id];

        return _valuesSum[id] / _valuesCnt[id];
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

    for(auto name : _valuesSum.keys()) {
        if(_valuesCnt[name] != 0) {
            _valuesSum[name] /= _valuesCnt[name];
            _valuesCnt[name] = 0;
        }

        _valuesHistory[name].push_back({time, _valuesSum[name]});
    }
}

void ValueManager::registerName(const std::string& name, quint32 id) {
    _nameMap[name] = id;
    _idMap[id] = name;
    _valuesSum[id] = 0;
    _valuesCnt[id] = 0;
}

QVector<std::string> ValueManager::nameList() const {
    return _nameMap.keys();
}

bool ValueManager::hasName(const std::string& name) {
    return _nameMap.contains(name);
}

bool ValueManager::hasId(quint32 id) {
    return _valuesCnt.contains(id);
}

quint32 ValueManager::id(const std::string& name) {
    return _nameMap[name];
}

std::string ValueManager::name(quint32 id) {
    return _idMap[id];
}