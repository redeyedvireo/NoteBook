#include <QXmlStreamWriter>
#include <QStandardItem>
#include <QDebug>
#include "CTaskReader.h"
#include "TaskDefs.h"
#include "CToDoEditWidget.h"
#include "CToDoItem.h"


//--------------------------------------------------------------------------
CTaskReader::CTaskReader(CToDoEditWidget *pToDoWidget)
    : m_pToDoWidget(pToDoWidget)
{
}

//--------------------------------------------------------------------------
bool CTaskReader::readTasks(QIODevice *pDevice)
{
    m_reader.setDevice(pDevice);

    if (m_reader.readNextStartElement())
    {
        if (m_reader.name() == kXmlDocTag)
        {
            readTaskXml();
        }
        else
        {
            qWarning() << "CTaskReader: XML is not a tasks document";
        }

        return !m_reader.error();
    }

    return false;
}

//--------------------------------------------------------------------------
void CTaskReader::readTaskXml()
{
    CToDoItem   *pParent = NULL;

    readTasks(pParent);
}

//--------------------------------------------------------------------------
void CTaskReader::readTasks(CToDoItem *pParent)
{
    // Read all sibling tasks
    while (! m_reader.atEnd())
    {
        m_reader.readNext();
        if (m_reader.isStartElement())
        {
            if (m_reader.name() == kTaskTag)
            {
                CToDoItem   *pTaskParent;
                pTaskParent = readTask(pParent);
            }
        }
    }
}

//--------------------------------------------------------------------------
void CTaskReader::readSubtasks(CToDoItem *pParent)
{
    CToDoItem   *pTaskParent;

    while (true)
    {
        if (m_reader.isStartElement())
        {
            if (m_reader.name() == kTaskTag)
            {
                pTaskParent = readTask(pParent);

                // Not sure about this just yet.  We need to advance past the nested end </task> tag.
                m_reader.readNext();
            }
        }
        else
        {
            // We're done at this point
            return;
        }
    }
}

//--------------------------------------------------------------------------
CToDoItem *CTaskReader::readTask(CToDoItem* pParent)
{
    // Assume that the reader is currently pointing at the <task> element.
    QXmlStreamAttributes    allAttrs = m_reader.attributes();
    bool                    bDone = false;      // Task completed flag
    int                     priority = 5;       // Task priority
    QString                 taskText;           // Text of the task item
    CToDoItem               *pTaskItem;         // New Task

    if (allAttrs.hasAttribute(kDoneAttr))
    {
        bDone = allAttrs.value(kDoneAttr) == "1" ? true : false;
    }

    if (allAttrs.hasAttribute(kPriorityAttr))
    {
        QString priorityStr = allAttrs.value(kPriorityAttr).toString();
        priority = priorityStr.toInt();
    }

    if (m_reader.readNextStartElement())
    {
        if (m_reader.name() == kTaskTextTag)
        {
            taskText = m_reader.readElementText();

            // Now create the task item.
            // Note: don't save the document here; it will be saved at the end,
            //       by the calling function.
            pTaskItem = m_pToDoWidget->AddTask(bDone, priority, taskText, false, pParent);
        }
    }

    // Advance to the end tag
    m_reader.readNext();

    if (m_reader.isStartElement())
    {
        readSubtasks(pTaskItem);        // Recursive call, with the newly-created task as the parent
    }

    return pTaskItem;
}
