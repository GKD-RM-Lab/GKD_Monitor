#include "value_tree_model.h"
#include <QtCore/qalgorithms.h>
#include <QtCore/qnamespace.h>
#include <QtCore/qvariant.h>
#include <cstddef>
#include "valve_manager.h"

ValueTreeItem::ValueTreeItem(std::string_view name,const std::string& fullName,ValueTreeItem *parent)
    :_parent(parent),
    _name(name),
    _fullName(fullName)
{
}

ValueTreeItem::~ValueTreeItem()
{
    qDeleteAll(_children);
}


ValueTreeItem* ValueTreeItem::insert(std::string_view name_view,const std::string& fullName,QVariant value){
    auto sep = name_view.find('.');
    std::string name{name_view.substr(0,sep)};

    if(sep == std::string_view::npos){
        _children[name] = new ValueTreeItem(name,fullName,this);
        _children[name]->setValue(value);

        return _children[name];
    }else{
        if(_children.contains(name)){
            return _children[name]->insert(name_view.substr(sep+1),fullName,value);
        }else{
            _children[name] = new ValueTreeItem(name,fullName,this);
            return _children[name]->insert(name_view.substr(sep+1),fullName,value);
        }
    }
}

ValueTreeModel::ValueTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , _rootItem(new ValueTreeItem("",""))
{
}

ValueTreeModel::~ValueTreeModel()
{
    delete _rootItem;
}

QModelIndex ValueTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ValueTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<ValueTreeItem*>(parent.internalPointer());

    ValueTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex ValueTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ValueTreeItem *childItem = static_cast<ValueTreeItem*>(index.internalPointer());
    ValueTreeItem *parentItem = childItem->parent();

    if (parentItem == _rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int ValueTreeModel::rowCount(const QModelIndex &parent) const
{
    ValueTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<ValueTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int ValueTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

QVariant ValueTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::UserRole)
        return QVariant();

    ValueTreeItem *item = static_cast<ValueTreeItem*>(index.internalPointer());

    if(role == Qt::DisplayRole)
        switch (index.column()) {
        case NameColumn:
            return item->name();
        case ValueColumn:
            return item->value();
        default:
            return QVariant();
        }
    else return QString::fromStdString(item->fullName());
}

QVariant ValueTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case NameColumn:
            return tr("Name");
        case ValueColumn:
            return tr("Value");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

// AI 写的,看不懂,不过能用
void ValueTreeModel::update(){
    // Helper: get model index for an item under its parent
    auto indexForItem = [this](ValueTreeItem* item, int column) -> QModelIndex {
        if (!item) return QModelIndex();
        ValueTreeItem* parentItem = item->parent();
        QModelIndex parentIndex;
        if (parentItem && parentItem != _rootItem) {
            ValueTreeItem* grand = parentItem->parent();
            QModelIndex grandIndex;
            if (grand && grand != _rootItem) {
                grandIndex = createIndex(grand->row(), 0, grand);
            }
            parentIndex = index(parentItem->row(), 0, grandIndex);
        }
        return index(item->row(), column, parentIndex);
    };

    // Iterate all known names, inserting missing nodes and updating existing values.
    for (const auto& name : valueManager.nameList()) {
        size_t hash = std::hash<std::string>{}(name);

        // Fetch latest value
        QVariant value;
        if (auto v = valueManager.readValue(name))
            value = QVariant(*v);
        else
            value = QVariant();

        ValueTreeItem* item = nullptr;
        auto it = _itemMap.find(hash);
        if (it != _itemMap.end()) {
            // Existing leaf: update value and notify
            item = it.value();
            item->setValue(value);
            QModelIndex valIndex = indexForItem(item, ValueColumn);
            if (valIndex.isValid())
                emit dataChanged(valIndex, valIndex, {Qt::DisplayRole});
            continue;
        }

        // Need to create path nodes segment-by-segment with proper insert signals
        ValueTreeItem* parentItem = _rootItem;
        std::string_view full{name};
        while (!full.empty()) {
            auto pos = full.find('.');
            std::string segment{full.substr(0, pos)};
            bool isLast = (pos == std::string_view::npos);

            ValueTreeItem* child = parentItem->child(segment);
            if (!child) {
                // Determine insertion row under parent (sorted by key as in QMap)
                int insertRow = 0;
                int childCount = parentItem->childCount();
                for (int i = 0; i < childCount; ++i) {
                    auto* existing = parentItem->child(i);
                    if (segment < existing->name().toStdString())
                        break;
                    ++insertRow;
                }

                // Build parent index for beginInsertRows
                QModelIndex parentIndex;
                if (parentItem != _rootItem) {
                    ValueTreeItem* grand = parentItem->parent();
                    QModelIndex grandIndex;
                    if (grand && grand != _rootItem)
                        grandIndex = createIndex(grand->row(), 0, grand);
                    parentIndex = index(parentItem->row(), 0, grandIndex);
                }

                beginInsertRows(parentIndex, insertRow, insertRow);
                // Insert single-level node (set empty value for intermediate nodes)
                child = parentItem->insert(segment,name, isLast ? value : QVariant());
                endInsertRows();
            } else if (isLast) {
                // Leaf existed but not in map (hash collision avoidance); update value
                child->setValue(value);
            }

            parentItem = child;
            if (isLast) {
                item = child;
                break;
            }

            // Advance to next segment
            full.remove_prefix(pos + 1);
        }

        if (item) {
            _itemMap[hash] = item;
        }
    }
}
