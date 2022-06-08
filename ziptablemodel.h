#ifndef ZIPTABLEMODEL_H
#define ZIPTABLEMODEL_H

#include <QAbstractTableModel>
#include <QPair>
#include <QVector>

class ZipTableModel : public QAbstractTableModel
{
    QVector<QPair<QString, qlonglong>> _data;

public:
    ZipTableModel(const std::string& pathToZip);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

#endif // ZIPTABLEMODEL_H
