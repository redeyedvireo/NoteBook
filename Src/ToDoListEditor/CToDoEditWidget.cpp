#include <QCheckBox>
#include <QComboBox>
#include <QStandardItemModel>
#include <QDebug>
#include <QBuffer>
#include <QFile>
#include <QStack>
#include <QtMath>
#include <QMenu>
#include "CPriorityDelegate.h"
#include "CTaskWriter.h"
#include "CTaskReader.h"
#include "TaskDefs.h"
#include "CToDoEditWidget.h"
#include "CToDoItem.h"
#include "CToDoListEditorSettingsWidget.h"

#define kDoneColumnWidth        60
#define kPriorityColumnWidth    70


CToDoEditWidget::CToDoEditWidget(QWidget *parent)
    : QWidget(parent), m_bLoading(false), m_bSorting(true)
{
    ui.setupUi(this);

    m_pModel = new QStandardItemModel(0, 3, ui.treeView);

    m_singleRowHeight = 20;
    m_bModified = false;

    m_bAutosave = true;

    // Toolbar buttons
    m_bHideDoneTasks = false;
    ui.hideDoneTasksButton->setChecked(m_bHideDoneTasks);

    ui.treeView->setModel(m_pModel);
    ConfigureTree();
    SetConnections();
}

//--------------------------------------------------------------------------
void CToDoEditWidget::SetConnections()
{
}

//--------------------------------------------------------------------------
void CToDoEditWidget::ConfigureTree()
{
    m_pModel->setHorizontalHeaderLabels(QStringList() << "Done" << "Priority" << "Task");

    CPriorityDelegate   *pDelegate = new CPriorityDelegate();
    ui.treeView->setItemDelegateForColumn(kPriorityColumn, pDelegate);

    // Compute row height, based on the font used when creating new items
    QStandardItem   *pTempItem = new QStandardItem();
    QFontMetrics    fm(pTempItem->font());
    m_singleRowHeight = fm.lineSpacing() + 2;   // Adding a 2-pixel fudge factor, to ensure enough space
    //qDebug() << "Line spacing: " << m_singleRowHeight;

    ui.treeView->setWordWrap(true);

    ui.treeView->setColumnWidth(kDoneColumn, kDoneColumnWidth);
    ui.treeView->setColumnWidth(kPriorityColumn, kPriorityColumnWidth);

    // Center the text in the header
    QHeaderView     *pHeader = ui.treeView->header();

    pHeader->setSectionResizeMode(kDoneColumn, QHeaderView::Fixed);
    pHeader->setSectionResizeMode(kPriorityColumn, QHeaderView::Fixed);

    m_pModel->setHeaderData(kDoneColumn, Qt::Horizontal, Qt::AlignHCenter, Qt::TextAlignmentRole);
    m_pModel->setHeaderData(kPriorityColumn, Qt::Horizontal, Qt::AlignHCenter, Qt::TextAlignmentRole);

    ui.treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui.treeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));
    connect(m_pModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(HandleItemChanged(QStandardItem*)));
    connect(ui.treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(HandleItemClicked(QModelIndex)));
}

//--------------------------------------------------------------------------
void CToDoEditWidget::onCustomContextMenu(const QPoint &point)
{
    QModelIndex index = ui.treeView->indexAt(point);
    if (index.isValid())
    {
        QMenu	*menu = new QMenu();

        menu->addAction(tr("Delete task"), this, SLOT(on_deleteTaskButton_clicked()));

        menu->exec(ui.treeView->mapToGlobal(point));
    }
}

//--------------------------------------------------------------------------
void CToDoEditWidget::removeAllTasks()
{
    m_pModel->removeRows(0, m_pModel->rowCount());
}

//--------------------------------------------------------------------------
void CToDoEditWidget::on_newTaskButton_clicked()
{
    CreateNewTask();
}

//--------------------------------------------------------------------------
void CToDoEditWidget::on_newSubtaskButton_clicked()
{
    CreateNewSubtask();
}

//--------------------------------------------------------------------------
void CToDoEditWidget::on_deleteTaskButton_clicked()
{
    DeleteSelectedTask();
}

//--------------------------------------------------------------------------
void CToDoEditWidget::on_hideDoneTasksButton_clicked()
{
    m_bHideDoneTasks = ui.hideDoneTasksButton->isChecked();
    setDoneRowVisibility(m_bHideDoneTasks);
}

//--------------------------------------------------------------------------
void CToDoEditWidget::saveOrEmitModified()
{
    // Don't save if currently in the middle of loading
    if (m_bLoading)
    {
        return;
    }

    if (m_bAutosave)
    {
        // Save the task list
        emit SavePage();
    }
    else
    {
        // Auto-save not enabled.  Tell the main window that it has been modified.
        m_bModified = true;
        emit TextModified();
    }
}

//--------------------------------------------------------------------------
void CToDoEditWidget::HandleItemClicked(QModelIndex index)
{
    QStandardItem*  pItem = m_pModel->itemFromIndex(index);
    CToDoItem   *pToDoItem = dynamic_cast<CToDoItem*>(pItem);

    if (pItem->column() == kDoneColumn)
    {
        bool    bDone = pToDoItem->isTaskDone();

        pToDoItem->crossOutTask(bDone);
        pToDoItem->markSubtasksAsDone(bDone, true);
        pToDoItem->updateParentDoneStatus();

        if (pToDoItem->isTaskDone())
        {
            // Set row visibility (this will hide subtasks as well)
            QStandardItem   *pRootItem = m_pModel->invisibleRootItem();
            ui.treeView->setRowHidden(pItem->row(), pRootItem->index(), m_bHideDoneTasks);
        }

        sortList();

        // Select the item
        ui.treeView->setCurrentIndex(pItem->index());

        saveOrEmitModified();
    }
}

//--------------------------------------------------------------------------
void CToDoEditWidget::HandleItemChanged(QStandardItem* pItem)
{
    if (pItem->column() == kDoneColumn)
    {
        // This will be handled by HandleItemClicked().
        return;
    }

    if (pItem->column() == kPriorityColumn)
    {
        // Priorties have changed: initiate a sort
        sortList();
    }
    else if (pItem->column() == kTaskColumn)
    {
        // Update the row height
        updateRowHeight(pItem->index());
    }

    // Select the item
    ui.treeView->setCurrentIndex(pItem->index());

    saveOrEmitModified();
}

//--------------------------------------------------------------------------
QString CToDoEditWidget::toString()
{
    QByteArray      xmlData;
    QBuffer         buffer;

    buffer.setBuffer(&xmlData);
    buffer.open(QIODevice::WriteOnly);

    CTaskWriter     taskWriter(m_pModel);

    taskWriter.writeTasks(&buffer);
    buffer.close();

    // DEBUG
    //qDebug() << "XML: " << xmlData;

    return QString(xmlData);
}

//--------------------------------------------------------------------------
void CToDoEditWidget::updateRowHeight(QModelIndex index)
{
    QStandardItem   *pTaskItem = m_pModel->itemFromIndex(index);

    // Calculate width of task text, using current font
    QFontMetrics    fm(pTaskItem->font());
    int             columnWidth = ui.treeView->columnWidth(kTaskColumn);

    // For some reason, QFontMetrics::width() doesn't return an accurate value of
    // the string width.  But using the average character width times the string
    // length seems to work pretty well.
    //float           stringWidth = fm.width(pTaskItem->text());
    float           stringWidth = pTaskItem->text().length() * fm.averageCharWidth();

    int             numTextRows = qCeil(stringWidth / columnWidth);

//    qDebug() << "Width of task column: " << columnWidth;
//    qDebug() << "Width of task text: " << stringWidth << " (" << pTaskItem->text() << ")";
//    qDebug() << "Number of rows required: " << numTextRows;

    QSize   hint = pTaskItem->sizeHint();

    hint.setHeight(m_singleRowHeight * numTextRows);

    pTaskItem->setSizeHint(hint);
}

//--------------------------------------------------------------------------
void CToDoEditWidget::newDocument()
{
    removeAllTasks();
}

//--------------------------------------------------------------------------
void CToDoEditWidget::sortList()
{
    if (m_bSorting)
    {
        m_pModel->sort(kPriorityColumn);
    }
}

//--------------------------------------------------------------------------
void CToDoEditWidget::setDoneRowVisibility(bool bVisible)
{
    int numRows = m_pModel->rowCount();
    QStandardItem   *pRootItem = m_pModel->invisibleRootItem();
    QModelIndex     rootIndex = pRootItem->index();

    for (int row = 0; row < numRows; row++)
    {
        CToDoItem   *pToDoItem = getTopLevelToDoItem(row, kDoneColumn);
        if (pToDoItem->isTaskDone())
        {
            ui.treeView->setRowHidden(row, rootIndex, bVisible);
        }
    }
}

//--------------------------------------------------------------------------
int CToDoEditWidget::getSelectedRow()
{
    if (ui.treeView->currentIndex().isValid())
    {
        return ui.treeView->currentIndex().row();
    }
    else
        return -1;
}

//--------------------------------------------------------------------------
CToDoItem* CToDoEditWidget::getTopLevelToDoItem(int row, int column)
{
    QStandardItem   *pItem = m_pModel->item(row, column);
    return dynamic_cast<CToDoItem*>(pItem);
}

//--------------------------------------------------------------------------
CToDoItem* CToDoEditWidget::AddTask(bool bDone, int priority, QString taskText, bool bEmitSaveSignal, CToDoItem* pParent)
{
    QStandardItem   *pParentItem = NULL;

    if (pParent != NULL)
    {
        pParentItem = (QStandardItem*) pParent->getDoneItem();
    }
    else
    {
        pParentItem = m_pModel->invisibleRootItem();
    }

    CToDoItem   *pDoneItem = new CToDoItem("");
    CToDoItem   *pPriorityItem = new CToDoItem(QString::number(priority));
    CToDoItem   *pTaskItem = new CToDoItem(taskText);

    pDoneItem->setCheckable(true);
    pDoneItem->setCheckState(bDone ? Qt::Checked : Qt::Unchecked);
    pDoneItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    pPriorityItem->setData(QVariant(priority), Qt::DisplayRole);
    QList<QStandardItem*>   itemList;
    pPriorityItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    itemList.append(pDoneItem);
    itemList.append(pPriorityItem);
    itemList.append(pTaskItem);

    pParentItem->appendRow(itemList);

    pDoneItem->markTaskDone(bDone);

    sortList();

    // Note: when the application first starts, this function may not work right,
    // because the UI elements have not yet been sized.
    updateRowHeight(pTaskItem->index());

    saveOrEmitModified();

    return pTaskItem;
}

//--------------------------------------------------------------------------
void CToDoEditWidget::CreateNewTask()
{
    QStandardItem   *pTaskItem;

    pTaskItem = AddTask(false, 5, "Task description", true, NULL);

    QModelIndex     taskItemIndex = pTaskItem->index();

    // Select the task item
    ui.treeView->setCurrentIndex(taskItemIndex);

    // Start editing the task
    ui.treeView->edit(taskItemIndex);
}

//--------------------------------------------------------------------------
void CToDoEditWidget::CreateNewSubtask()
{
    int selectedRow = getSelectedRow();

    QStandardItem   *pRootItem = m_pModel->invisibleRootItem();

    CToDoItem       *pTask = dynamic_cast<CToDoItem*>(pRootItem->child(selectedRow, kDoneColumn));

    CToDoItem       *pTaskItem = AddTask(false, 5, "Task description", true, pTask);

    QModelIndex     taskItemIndex = pTaskItem->index();

    // Select the task item
    ui.treeView->setCurrentIndex(taskItemIndex);

    // Start editing the task
    ui.treeView->edit(taskItemIndex);
}

//--------------------------------------------------------------------------
void CToDoEditWidget::DeleteSelectedTask()
{
    int     selectedRow = getSelectedRow();

    if (selectedRow != -1)
    {
        QStandardItem   *pRootItem = m_pModel->invisibleRootItem();
        m_pModel->removeRow(selectedRow, pRootItem->index());

        saveOrEmitModified();
    }
}

//--------------------------------------------------------------------------
QString CToDoEditWidget::getPageContents()
{
    return toString();
}

//--------------------------------------------------------------------------
void CToDoEditWidget::setDocumentModified(bool bModified)
{
    m_bModified = bModified;
}

//--------------------------------------------------------------------------
bool CToDoEditWidget::isDocumentModified()
{
    return m_bModified;
}

//--------------------------------------------------------------------------
void CToDoEditWidget::enableEditing(bool bEnable)
{
    Q_UNUSED(bEnable);
}

//--------------------------------------------------------------------------
void CToDoEditWidget::setPageContents(QString contents, QStringList imageNames)
{
    Q_UNUSED(imageNames);

    CTaskReader     taskReader(this);
    QByteArray      xmlData;
    QBuffer         buffer;

    // Set the loading flag to disable saving the contents whenever the
    // task list changes.
    m_bLoading = true;

    // Start with a blank slate
    removeAllTasks();

    // Convert contents to a byte array
    xmlData = contents.toLatin1();

    buffer.setBuffer(&xmlData);
    buffer.open(QIODevice::ReadOnly);

    taskReader.readTasks(&buffer);
    buffer.close();

    setDocumentModified(false);

    setDoneRowVisibility(m_bHideDoneTasks);

    // Done loading
    m_bLoading = false;

    saveOrEmitModified();
}

//--------------------------------------------------------------------------
QWidget *CToDoEditWidget::settingsWidget()
{
    CToDoListEditorSettingsWidget   *pEditorSettingsWidget = new CToDoListEditorSettingsWidget();

    // Populate the values of the settings
    pEditorSettingsWidget->setAutoSave(m_bAutosave);

    return pEditorSettingsWidget;
}

//--------------------------------------------------------------------------
void CToDoEditWidget::settingsDialogAccepted(QWidget *pSettingsWidget)
{
    CToDoListEditorSettingsWidget   *pEditorSettingsWidget = dynamic_cast<CToDoListEditorSettingsWidget*>(pSettingsWidget);

    m_bAutosave = pEditorSettingsWidget->getAutoSave();
}
