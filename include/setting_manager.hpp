#pragma once
#include <QSettings>
#include <QString>
#include "singleton.hpp"

class ConfigManager : public Singleton<ConfigManager>{
public:

    template<typename T>
    static void setValue(const QString& key, const T& value) {
        instance().settings.setValue(key, value);
    }

    template<typename T>
    static T getValue(const QString& key, const T& defaultValue = T()) {
        return instance().settings.value(key, defaultValue).template value<T>();
    }

    static void removeKey(const QString& key) {
        instance().settings.remove(key);
    }

    static void clear() {
        instance().settings.clear();
    }

    static void sync() {
        instance().settings.sync();
    }

private:
    QSettings settings;
};
