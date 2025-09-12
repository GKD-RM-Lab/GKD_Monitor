#pragma once
#include <QAbstractItemModel>
#include <QMap>
#include <QObject>
#include <QtCore/qvariant.h>
#include <cstddef>
#include <string_view>

class ValueTreeItem{
public:
    ValueTreeItem(std::string_view name,const std::string& fullName,ValueTreeItem* parent = nullptr);
    ~ValueTreeItem();

    inline void setValue(const QVariant& value){_value = value;}
    inline QVariant value()const{
        return _value;
    }

    inline ValueTreeItem* child(const std::string& name) const{
        if(_children.contains(name))
            return _children[name];
        return nullptr;
    }

    inline ValueTreeItem* child(int row) const{
        if(row >= 0 && row < _children.size())
            return _children.values().at(row);
        return nullptr;
    }

    inline int childCount() const{
        return _children.size();
    }

    inline int row() const{
        if(_parent)
            return _parent->_children.values().indexOf(const_cast<ValueTreeItem*>(this));
        return 0;
    }

    inline ValueTreeItem* parent() const{
        return _parent;
    }

    inline QString name() const{
        if(_parent){
            for(auto it = _parent->_children.constBegin(); it != _parent->_children.constEnd(); ++it){
                if(it.value() == this)
                    return QString::fromStdString(it.key());
            }
        }
        return QString();
    }

    inline std::string fullName()const{return _fullName;}

    ValueTreeItem* insert(std::string_view name_view,const std::string& fullName,QVariant value);
private:
    QMap<std::string,ValueTreeItem*> _children;
    ValueTreeItem* _parent;
    QVariant _value;
    std::string _fullName;
    std::string _name;
};

class ValueTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Column {
        NameColumn = 0,
        ValueColumn,
        ColumnCount
    };

    explicit ValueTreeModel(QObject *parent = nullptr);
    ~ValueTreeModel();

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public slots:
    void update();

private:
    ValueTreeItem* _rootItem;
    QMap<size_t,ValueTreeItem*> _itemMap;
};