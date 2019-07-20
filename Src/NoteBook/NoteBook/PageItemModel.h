#ifndef PAGEITEMMODEL_H
#define PAGEITEMMODEL_H

#include <QStringListModel>
#include "PageData.h"


class PageItemModel : public QStringListModel
{
    Q_OBJECT

public:
    explicit PageItemModel(QObject *parent = 0);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void populateModel(PAGE_HASH pageHash);

    ENTITY_ID getPageId(const QModelIndex &index);
};

#endif // PAGEITEMMODEL_H
