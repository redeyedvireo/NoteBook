#ifndef CTASKWRITER_H
#define CTASKWRITER_H

#include <QXmlStreamWriter>


class QStandardItemModel;


class CTaskWriter
{
public:
    CTaskWriter(QStandardItemModel* pModel);

    bool writeTasks(QIODevice* pDevice);

private:
    QStandardItemModel      *m_pModel;
    QXmlStreamWriter        m_writer;

    /**
     * @brief Writes the task and all its children.
     * @param pItem
     */
    void writeChildTasks(QStandardItem *pItem);

    /**
     * @brief Writes a task.
     * @param pParent Parent task (or the invisible root item, for top-level tasks)
     * @param row
     */
    void writeTask(QStandardItem *pParent, int row);
};

#endif // CTASKWRITER_H
