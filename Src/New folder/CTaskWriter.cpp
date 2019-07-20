#include <QStandardItemModel>
#include "CTaskWriter.h"
#include "TaskDefs.h"


//--------------------------------------------------------------------------
CTaskWriter::CTaskWriter(QStandardItemModel *pModel)
    : m_pModel(pModel)
{

}

//--------------------------------------------------------------------------
bool CTaskWriter::writeTasks(QIODevice *pDevice)
{
    m_writer.setDevice(pDevice);

    m_writer.writeStartDocument();
    m_writer.writeDTD("<!DOCTYPE tasks>");
    m_writer.writeStartElement(kXmlDocTag);
    m_writer.writeAttribute("version", "1.0");

    QStandardItem   *pRootItem = m_pModel->invisibleRootItem();

    writeChildTasks(pRootItem);

    m_writer.writeEndDocument();
    return true;
}

//--------------------------------------------------------------------------
void CTaskWriter::writeChildTasks(QStandardItem* pItem)
{
    int numTasks = pItem->rowCount();
    for (int task = 0; task < numTasks; task++)
    {
        writeTask(pItem, task);
    }
}

//--------------------------------------------------------------------------
void CTaskWriter::writeTask(QStandardItem* pParent, int row)
{
    m_writer.writeStartElement(kTaskTag);

    // Done flag
    QStandardItem   *pDoneItem = pParent->child(row, kDoneColumn);
    bool    bDone = pDoneItem->checkState() == Qt::Checked ? true : false;

    m_writer.writeAttribute(kDoneAttr, bDone ? "1" : "0");

    // Priority
    QStandardItem   *pPriorityItem = pParent->child(row, kPriorityColumn);
    int     priority = pPriorityItem->data(Qt::DisplayRole).toInt();

    m_writer.writeAttribute(kPriorityAttr, QString::number(priority));

    // Task text
    QStandardItem   *pTaskItem = pParent->child(row, kTaskColumn);
    QString         taskText = pTaskItem->text();

    // TODO: If the task contains rich text, that text will need to be
    //       pulled out somehow.
    m_writer.writeTextElement(kTaskTextTag, taskText);

    // Write child tasks (note: the done column is where children are attached)
    if (pDoneItem->hasChildren())
    {
        writeChildTasks(pDoneItem);
    }

    m_writer.writeEndElement();
}
