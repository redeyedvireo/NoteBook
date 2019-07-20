#ifndef CTASKREADER_H
#define CTASKREADER_H

#include <QXmlStreamReader>

class CToDoEditWidget;
class CToDoItem;


class CTaskReader
{
public:
    CTaskReader(CToDoEditWidget* pToDoWidget);

    bool readTasks(QIODevice* pDevice);

private:
    CToDoEditWidget             *m_pToDoWidget;
    QXmlStreamReader            m_reader;

    /**
     * @brief Reads all tasks from the XML.
     */
    void readTaskXml();

    /**
     * @brief readTask
     * @param pParent Parent under which the new task will be created.
     */
    CToDoItem *readTask(CToDoItem *pParent);

    /**
     * @brief Reads all tasks with the same parent (ie, sibling tasks).
     * @param pParent
     * @return
     */
    void readTasks(CToDoItem *pParent);

    /**
     * @brief Reads all subtasks with the same parent (siblings).
     * @param pParent
     */
    void readSubtasks(CToDoItem *pParent);
};

#endif // CTASKREADER_H
