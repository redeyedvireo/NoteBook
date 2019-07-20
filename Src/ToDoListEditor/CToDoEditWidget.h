#ifndef CTODOEDITWIDGET_H
#define CTODOEDITWIDGET_H

#include <QObject>
#include <QWidget>
#include "CAbstractPageInterface.h"
#include "ui_ToDoEdit.h"

class QStandardItemModel;
class QStandardItem;
class CToDoItem;
class AbstractPluginServices;


/*
 * TODO: it might be desirable, when I get to the point of refactoring for the plug-in interface,
 * to make the CAbstractPageInterface-derived object a separate object.  This object would
 * contain a pointer to the widget (the CToDoEditWidget in this case), and would have a function,
 * QWidget* widget();
 * that would return the pointer to this widget.
 */

class CToDoEditWidget : public QWidget, public Ui_ToDoEditWidget, public CAbstractPageInterface
{
    Q_OBJECT
public:
    explicit CToDoEditWidget(QWidget *parent = 0);

    /**
     * @brief Returns the editor widget for this editor.
     * @return
     */
    virtual QWidget *editorWidget() { return this; }

    /**
     * @brief Adds a task.
     * @param bDone
     * @param priority
     * @param taskText
     * @param bEmitSaveSignal If true, the document will either be saved, or marked as
     *        "dirty", depending on the auto-save setting.
     * @param pParent Item under which the new task will be created.
     * @return Returns a pointer to the task item of the created task.
     */
    CToDoItem *AddTask(bool bDone, int priority, QString taskText, bool bEmitSaveSignal = true, CToDoItem *pParent = NULL);

    bool isAutoSaved()  { return m_bAutosave; }

    void setAutoSave(bool autoSave)    { m_bAutosave = autoSave; }

private:
    void SetConnections();

    /**
     * @brief Removes all tasks from the tree.
     */
    void removeAllTasks();

    /**
     * @brief Creates a new top-level task.
     */
    void CreateNewTask();

    /**
     * @brief Creates a new subtask of the currently selected task.
     */
    void CreateNewSubtask();

    /**
     * @brief Deletes the selected task.
     */
    void DeleteSelectedTask();

    /**
     * @brief Configures the tree.
     */
    void ConfigureTree();

    /**
     * @brief Returns the contents of the document as a string.
     * @return String containing the document contents.
     */
    QString toString();

    /**
     * @brief Sorts the tasks by priority, then by "doneness".
     *
     * Sorting is tricky.  We want all the done tasks at the bottom, and the
     * remaining sorted by priority.
     */
    void sortList();

    /**
     * @brief Sets the visibility of all done rows.
     * @param bVisible
     */
    void setDoneRowVisibility(bool bVisible);

    /**
     * @brief Returns the selected row, or -1 if no row is selected.
     * @return Returns the selected row, or -1.
     */
    int getSelectedRow();

    /**
     * @brief Returns the desired top-level item.
     * @param row
     * @return
     */
    CToDoItem *getTopLevelToDoItem(int row, int column);

    /**
     * @brief Either saves the task list, or emits signals to indicate the
     * task list has been modified.
     */
    void saveOrEmitModified();

    /**
     * @brief Sets an appropriate row height, based on the contents.
     * @param row
     */
    void updateRowHeight(QModelIndex index);

public:
    /*
     * CAbstractPageInterface overrides
     */
    virtual PAGE_TYPE documentType() { return kPageTypeToDoList; }

    virtual void newDocument();

    virtual QString getPageContents();

    virtual void setDocumentModified(bool bModified);

    virtual bool isDocumentModified();

    virtual void enableEditing(bool bEnable);

    virtual void setPageContents(QString contents, QStringList imageNames);

    virtual QWidget *settingsWidget();

    virtual QString settingsGroupName() { return "To Do List"; }

    virtual void settingsDialogAccepted(QWidget* pSettingsWidget);

signals:
    void TextModified();
    void SavePage();               // Requests the application to save the page

protected slots:
    void onCustomContextMenu(const QPoint &point);
    void on_hideDoneTasksButton_clicked();
    void on_newTaskButton_clicked();
    void on_newSubtaskButton_clicked();
    void on_deleteTaskButton_clicked();
    void HandleItemChanged(QStandardItem *pItem);
    void HandleItemClicked(QModelIndex index);

private:
    Ui::ToDoEditWidget		ui;

    QStandardItemModel      *m_pModel;
    bool                    m_bModified;        // True if document modified
    bool                    m_bLoading;         // True if loading a task list from the database
    bool                    m_bSorting;         // True if the list should be sorted
    int                     m_singleRowHeight;  // Row height, in pixels of a single row

    // Flags from the toolbar
    bool                    m_bAutosave;        // If true, the list will be saved after every change
    bool                    m_bHideDoneTasks;   // If true, Done tasks will be hidden
};

#endif // CTODOEDITWIDGET_H
