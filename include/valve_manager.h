#pragma once
#include "singleton.hpp"
#include <QtCore/qcontainerfwd.h>
#include <map>
#include <QVariant>
#include <string_view>
#include <optional>
#include <cstdint>
#include <iostream>
#include <QMap>
#include <QDateTime>

// valve = value

using ValueType = double;
using TimeValuePair = QPair<qint64,ValueType>;


// 图方便全inline了，这样不好
class ValueManager : public Singleton<ValueManager>
{
    ValueManager() = default;
    friend class Singleton<ValueManager>;
public:
    // 用每次存取间隔的数据的平均值
    inline void updateValue(const std::string& name, ValueType value){
        if(_valuesCnt.find(name) == _valuesCnt.end()){
            _valuesCnt[name] = 1;
            _valuesSum[name] = value;
        }
        else{
            if(_valuesCnt[name] == 0){
                _valuesSum[name] = 0;
            }
            _valuesCnt[name]++;
            _valuesSum[name] += value;
        }
    }

    inline std::optional<ValueType> readValue(const std::string& name){
        if(_valuesCnt.find(name) != _valuesCnt.end()){
            if(_valuesCnt[name] == 0)
                return _valuesSum[name];

            return _valuesSum[name] / _valuesCnt[name];
        }
        return std::nullopt;
    }

    inline void update(){
        qint64 time = QDateTime::currentMSecsSinceEpoch();

        for(const auto& name : _valuesSum.keys()){
            _valuesSum[name] /= _valuesCnt[name];
            _valuesCnt[name] = 0;

            _valuesHistory[name].push_back({time,_valuesSum[name]});
        }
    }

    inline QVector<std::string> nameList()const{
        return _valuesSum.keys();
    }

    inline bool hasName(const std::string& name){return _valuesCnt.contains(name);}

private:
    QMap<std::string, ValueType> _valuesSum;
    QMap<std::string, std::uint32_t> _valuesCnt;

    QMap<std::string,QVector<TimeValuePair>> _valuesHistory;
};

#define valueManager (ValueManager::instance())