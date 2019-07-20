#ifndef CEDITORMANAGER_H
#define CEDITORMANAGER_H

#include <QHash>

class QWidget;
class QStackedWidget;
class CAbstractPageInterface;


class CEditorManager
{
public:
    /**
     * @brief Constructor.
     * @param pStackedWidget Stacked widget where the editors reside.
     */
    CEditorManager(QStackedWidget *pStackedWidget);

    /**
     * @brief Adds a new editor.
     * @param pEditorWidget Pointer to the editor widget.
     * @param pageType Page type for which this editor is designed to edit.
     * @return Returns the ID of the editor just added.
     */
    void addEditor(CAbstractPageInterface* pPageInterface, int pageType);

    /**
     * Activates (ie, "switches to") the editor that has been registered
     * to edit the given page type.
     * @param pageType
     */
    void activateEditor(int pageType);

    /**
     * @brief Returns the page type of the active editor.
     * @return Page type the active editor was designed to edit.
     */
    int getActiveEditorPageType();

    /**
     * @brief Returns the active editor.
     * @return
     */
    CAbstractPageInterface *getActiveEditor();

    /**
     * Returns the page interface pointer that has been registered to handle
     * the given page type.
     *
     * @param pageType
     * @return
     */
    CAbstractPageInterface *getEditorForPageType(int pageType);

    /**
     * Returns the editor widget that has been registered to handle the
     * given page type.
     *
     * @param pageType
     * @return
     */
    QWidget *getEditorWidgetForPageType(int pageType);

    /**
     * Returns a list of all editors.
     * @return
     */
    QList<CAbstractPageInterface*>  getEditors();

private:
    /**
     * Returns the index of the widget in the stacked widget that
     * has been registered to edit the given page type.
     * @param pageType
     * @return Returns the stacked widget index, or -1 if the given
     *          page type has not been registered.
     */
    int getStackedWidgetIndexForPageType(int pageType);

private:
    QStackedWidget      *m_pStackedWidget;

    // Maps page types to the index of the editor widget in the stacked widget.
    // Key: page type, value: stacked widget index
    QHash<int, int>     m_editorWidgetMap;

    // Maps page types to the corresponding abstract page interface pointers.
    // Note: the object that the page interface pointer points to is generically
    // called an "editor", but this is not to be confused with the "editor widget"
    // that is stored in the stacked widget.  However, it is likely that the "editor"
    // and "editor widget" are the same object, created with multiple-inheritance so
    // that it can be both a widget and an editor.
    //
    // Key: page type, value: abstract page type interface pointer
    QHash<int, CAbstractPageInterface*>      m_pageInterfaceMap;
};

#endif // CEDITORMANAGER_H
