#pragma once
#include "singleton.hpp"
#include <QtCore/qcontainerfwd.h>
#include <map>
#include <QVariant>
#include <string_view>
#include <optional>
#include <cstdint>
#include <QMap>

// valve = value

using ValueType = double;

class ValueManager : public Singleton<ValueManager>
{
    ValueManager() = default;
    friend class Singleton<ValueManager>;
public:
    // 用每次存取间隔的数据的平均值
    inline void set(const std::string& name, ValueType value){
        if(_values_cnt.find(name) == _values_cnt.end()){
            _values_cnt[name] = value;
            _values_sum[name] = 1;
        }
        else{
            if(_values_cnt[name] == 0){
                _values_sum[name] = 0;
            }
            _values_cnt[name] += value;
            _values_sum[name]++;
        }
    }

    inline std::optional<ValueType> get(const std::string& name){
        if(_values_cnt.find(name) != _values_cnt.end()){
            if(_values_cnt[name] == 0)
                return _values_sum[name];
            
            ValueType res = _values_sum[name] / _values_cnt[name];
            _values_sum[name] = res;
            _values_cnt[name] = 0;

            return res;
        }
        return std::nullopt;
    }

    inline QVector<std::string> nameList()const{
        return _values_sum.keys();
    }

private:
    QMap<std::string, ValueType> _values_sum;
    QMap<std::string, std::uint32_t> _values_cnt;
};

#define valueManager (ValueManager::instance())