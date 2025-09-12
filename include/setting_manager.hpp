#pragma once
#include <QSettings>
#include <QString>
#include "singleton.hpp"

class SettingManager : public Singleton<SettingManager>{
public:
    inline static void init(const QString& filePath){
        instance().settings.setPath(QSettings::IniFormat, QSettings::UserScope, filePath);
    }

    template<typename T>
    static void setValue(const QString& key, const T& value) {
        instance().settings.setValue(key, value);
    }

    template<typename T>
    static T getValue(const QString& key, const T& defaultValue = T()) {
        return instance().settings.value(key, defaultValue).template value<T>();
    }

    inline static void removeKey(const QString& key) {
        instance().settings.remove(key);
    }

    inline static void clear() {
        instance().settings.clear();
    }

    inline static void sync() {
        instance().settings.sync();
    }

private:
    QSettings settings;
};
