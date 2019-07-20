#include "CToDoItem.h"


//--------------------------------------------------------------------------
CToDoItem::CToDoItem()
    : QStandardItem()
{

}

//--------------------------------------------------------------------------
CToDoItem::CToDoItem(const QString &text)
    : QStandardItem(text)
{

}

//--------------------------------------------------------------------------
bool CToDoItem::isTaskDone() const
{
    CToDoItem const  *pDoneItem = getDoneItem();
    return pDoneItem->checkState() == Qt::Checked;
}

//--------------------------------------------------------------------------
int CToDoItem::priority() const
{
    CToDoItem const *pPriorityItem = getPriorityItem();
    QVariant    priorityData = pPriorityItem->data(Qt::DisplayRole);
    return priorityData.toInt();
}

//--------------------------------------------------------------------------
void CToDoItem::updateParentDoneStatus()
{
    QStandardItem   *pParent = parent();

    if (pParent != NULL)
    {
        CToDoItem   *pToDoParent = dynamic_cast<CToDoItem*>(pParent);

        // Get the current done status; if this status hasn't changed, no need
        // to update anything.
        bool    bCurrentDoneStatus = pToDoParent->isTaskDone();

        // Determine whether there is "consensus" among the child tasks (ie,
        // whether they are all done or not done.
        int numRows = pToDoParent->rowCount();
        if (numRows > 0)
        {
            bool bFirstChildDone = pToDoParent->getChildTask(0)->isTaskDone();
            bool bConsensus = true;

            for (int i = 1; i < numRows; i++)
            {
                if (pToDoParent->getChildTask(i)->isTaskDone() != bFirstChildDone)
                {
                    // At least one child differs, so no consensus
                    bConsensus = false;
                    break;
                }
            }

            // Determine the parent's new done status
            bool bNewDoneStatus = bConsensus ? bFirstChildDone : false;

            if (bNewDoneStatus != bCurrentDoneStatus)
            {
                // The status has changed.  Must update it.
                pToDoParent->markTaskDone(bNewDoneStatus);
                pToDoParent->crossOutTask(bNewDoneStatus);
                pToDoParent->updateParentDoneStatus();      // Update ancestors
            }
        }
    }
}

//--------------------------------------------------------------------------
void CToDoItem::markTaskDone(bool bDone)
{
    // Mark all subtasks as done, too
    CToDoItem   *pDoneItem = (CToDoItem*) getDoneItem();

    pDoneItem->setCheckState(bDone ? Qt::Checked : Qt::Unchecked);
}

//--------------------------------------------------------------------------
void CToDoItem::markSubtasksAsDone(bool bDone, bool bCrossOutSubtasks)
{
    CToDoItem   *pDoneItem = (CToDoItem*) getDoneItem();

    int     numRows = pDoneItem->rowCount();

    for (int row = 0; row < numRows; row++)
    {
        CToDoItem   *pChildItem = pDoneItem->getChildTask(row);
        pChildItem->markTaskDone(bDone);

        if (bCrossOutSubtasks)
        {
            pChildItem->crossOutTask(bDone);
        }
    }
}

//--------------------------------------------------------------------------
void CToDoItem::crossOutTask(bool bDoCrossOut)
{
    CToDoItem   *pTaskItem = (CToDoItem*) getTaskItem();

    QFont   font = pTaskItem->font();
    font.setStrikeOut(bDoCrossOut);
    pTaskItem->setFont(font);
}

//--------------------------------------------------------------------------
CToDoItem *CToDoItem::getChildTask(int row)
{
    QStandardItem   *pChildItem = child(row, kDoneColumn);
    return dynamic_cast<CToDoItem*>(pChildItem);
}

//--------------------------------------------------------------------------
CToDoItem const *CToDoItem::getSibling(int col) const
{
    CToDoItem       *pSibling = NULL;

    if (column() == col)
    {
        // Nothing to do
        return this;
    }
    else
    {
        QModelIndex     siblingIndex = model()->sibling(row(), col, index());
        pSibling = dynamic_cast<CToDoItem*>(model()->itemFromIndex(siblingIndex));
    }

    return pSibling;
}

//--------------------------------------------------------------------------
bool CToDoItem::operator<(const QStandardItem &other) const
{
    /*
     * The sort order places done items as the lowest priority, so that
     * the list will place all non-done items before all done items,
     * and within each group (done and non-done), items will be sorted
     * by priority.
     */

    const CToDoItem   &otherItem = dynamic_cast<const CToDoItem&>(other);

    bool    thisItemIsDone = isTaskDone();
    bool    otherItemIsDone = otherItem.isTaskDone();

    int     thisItemPriority = priority();
    int     otherItemPriority = otherItem.priority();

    if (thisItemIsDone != otherItemIsDone)
    {
        // If the "doneness" is different, than whichever one is done
        // has the lesser value (ie, is higher priority).
        return !thisItemIsDone;
    }
    else
    {
        // If both items are either done or not done, then priority
        // is the determining factor.
        return thisItemPriority < otherItemPriority;
    }
}

