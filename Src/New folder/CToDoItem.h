#ifndef CTODOITEM_H
#define CTODOITEM_H

#include <QStandardItem>
#include "TaskDefs.h"

class CToDoItem : public QStandardItem
{
public:
    CToDoItem();
    CToDoItem(const QString & text);

    virtual ~CToDoItem() {}

    /**
     * @brief Returns true if the task is done, false otherwise.
     * @return
     */
    bool isTaskDone() const;

    /**
     * @brief Returns the priority of the task.
     * @return
     */
    int priority() const;

    /**
     * @brief Updates the done status of the parent item, based
     *        on the done status of all its children.
     */
    void updateParentDoneStatus();

    /**
     * @brief Marks a task as done.
     *
     * All subtasks will be marked the same way.
     */
    void markTaskDone(bool bDone);

    /**
     * @brief Marks subtasks as done.
     * @param bDone
     */
    void markSubtasksAsDone(bool bDone, bool bCrossOutSubtasks = true);

    /**
     * @brief "Crosses-out" a task.  That is, sets the strike out property
     *         of the text of its task column.
     */
    void crossOutTask(bool bDoCrossOut);

    /**
     * @brief Returns the specified child task (the Done column is returned).
     * @param row
     * @return Returns the specified child task, or NULL if the specified
     *         child does not exist.
     */
    CToDoItem* getChildTask(int row);

    /**
     * @brief Returns the desired sibling.
     * @param column
     * @return Sibling
     *
     * Returns the item's sibling in the specified column.
     */
    CToDoItem const * getSibling(int col) const;

    CToDoItem const * getDoneItem() const       { return getSibling(kDoneColumn); }
    CToDoItem const * getPriorityItem() const   { return getSibling(kPriorityColumn); }
    CToDoItem const * getTaskItem() const       { return getSibling(kTaskColumn); }

    /**
     * @brief operator <
     * @param other
     * @return Returns true if this item is less than other
     *
     * Used for sorting.  Sorting is based first on done/not-done, and then
     * on priority.
     */
    virtual bool operator<(const QStandardItem & other) const;
};

#endif // CTODOITEM_H
