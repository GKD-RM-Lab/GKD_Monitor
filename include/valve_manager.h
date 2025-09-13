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


// 图方便全inline了，这样不好
class ValueManager : public Singleton<ValueManager>
{
    ValueManager() = default;
    friend class Singleton<ValueManager>;
public:
    // 用每次存取间隔的数据的平均值
    inline void updateValue(quint32 id, ValueType value){
        if(_valuesCnt.find(id) == _valuesCnt.end()){
            _valuesCnt[id] = 1;
            _valuesSum[id] = value;
        }
        else{
            if(_valuesCnt[id] == 0){
                _valuesSum[id] = 0;
            }
            _valuesCnt[id]++;
            _valuesSum[id] += value;
        }
    }

    inline void updateValue(const std::string& name, ValueType value){
        if(_nameMap.contains(name)){
            updateValue(_nameMap[name],value);
        }
    }


    inline std::optional<ValueType> readValue(quint32 id){
        if(_valuesCnt.find(id) != _valuesCnt.end()){
            if(_valuesCnt[id] == 0)
                return _valuesSum[id];

            return _valuesSum[id] / _valuesCnt[id];
        }
        return std::nullopt;
    }

    inline std::optional<ValueType> readValue(const std::string& name){
        if(_nameMap.contains(name)){
            return readValue(_nameMap[name]);
        }
        return std::nullopt;
    }

    inline void update(){
        qint64 time = QDateTime::currentMSecsSinceEpoch();

        for(auto name : _valuesSum.keys()){
            _valuesSum[name] /= _valuesCnt[name];
            _valuesCnt[name] = 0;

            _valuesHistory[name].push_back({time,_valuesSum[name]});
        }
    }

    inline void registerName(const std::string& name, quint32 id){
        _nameMap[name] = id;
        _idMap[id] = name;
        _valuesSum[id] = 0;
        _valuesCnt[id] = 0;
    }

    inline QVector<std::string> nameList()const{
        return _nameMap.keys();
    }

    inline bool hasName(const std::string& name){return _nameMap.contains(name);}
    inline bool hasId(quint32 id){return _valuesCnt.contains(id);}
    inline quint32 id(const std::string& name){return _nameMap[name];}

    inline std::string name(quint32 id){return _idMap[id];}

private:
    QMap<quint32, ValueType> _valuesSum;
    QMap<quint32, std::uint32_t> _valuesCnt;

    QMap<quint32,QVector<TimeValuePair>> _valuesHistory;
    QMap<std::string,quint32> _nameMap;
    QMap<quint32,std::string> _idMap;
};

#define valueManager (ValueManager::instance())