#include "PageItemModel.h"


PageItemModel::PageItemModel(QObject *parent)
    : QStringListModel(parent)
{
}

//--------------------------------------------------------------------------
void PageItemModel::populateModel(PAGE_HASH pageHash)
{
    if (rowCount() > 0)
    {
        // Clear data and repopulate
        removeRows(0, rowCount());
    }

    insertRows(0, pageHash.size());

    QHashIterator<ENTITY_ID, SHARED_CPAGEDATA>	it(pageHash);

    int     row = 0;

    while (it.hasNext())
    {
        it.next();

        SHARED_CPAGEDATA	pageData = it.value();

        QModelIndex index = createIndex(row, 0);
        setData(index, pageData->m_title, Qt::EditRole);
        setData(index, pageData->m_pageId, Qt::UserRole);

        row++;
    }
}

//--------------------------------------------------------------------------
QVariant PageItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

//--------------------------------------------------------------------------
ENTITY_ID PageItemModel::getPageId(const QModelIndex &index)
{
    QVariant    result = data(index, Qt::UserRole);

    if (result.isValid())
    {
        return result.toUInt();
    }
    else
    {
        return kInvalidPageId;
    }
}
