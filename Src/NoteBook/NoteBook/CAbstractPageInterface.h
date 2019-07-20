#ifndef CABSTRACTPAGEINTERFACE
#define CABSTRACTPAGEINTERFACE

#include "PageData.h"
#include "AbstractPluginServices.h"

/*
 * This is an abstract class for interfacing with a NoteBook document.  All NoteBook editors
 * must subclass from this.
 */
class CAbstractPageInterface
{
public:
    /**
     * @brief Returns the document type for this editor.
     * @return Document type.
     */
    virtual PAGE_TYPE documentType() = 0;

    /**
     * @brief Returns the editor widget for this editor.
     * @return Editor widget
     */
    virtual QWidget* editorWidget() = 0;

    /**
     * Creates a new document.
     */
    virtual void newDocument() = 0;

    /**
     * @brief Returns the contents of the page as a QString.
     * @return
     */
    virtual QString getPageContents() = 0;

    /**
     * @brief Sets the page contents with a string.  It is up to the caller
     *        to supply a string that is appropriate for the given editor.
     *        For example, a RichTextEdit widget expects a string that is a
     *        block of HTML, a ToDoList expects a string that is XML containing
     *        task data.
     * @param contents Page contents, as a string.
     * @param imageNames Names of images used by this document.
     */
    virtual void setPageContents(QString contents, QStringList imageNames) = 0;

    /**
     * @brief Sets the modified flag for the document.
     * @param bModified
     */
    virtual void setDocumentModified(bool bModified) = 0;

    /**
     * @brief Returns true if the document has been modified.
     * @return True if modified, false otherwise.
     */
    virtual bool isDocumentModified() = 0;

    /**
     * @brief Enables or disabled the widget for editing.
     */
    virtual void enableEditing(bool bEnable) = 0;


    /*
     * Settings widget functions
     */

    /**
     * Creates and returns the settings widget for this page interface type.
     * If this page interface type has no settings widget, NULL should be returned.
     * The widget created will become parented to, and owned by, the settings dialog,
     * so this widget will need to be created every time this function is called.
     * This function should also populate the widget with the current values of the
     * settings for which it configures.
     *
     * @return Settings widget or NULL.
     */
    virtual QWidget *settingsWidget() = 0;

    /**
     * Returns the name that appears in the settings dialog for this settings widget.
     * If this page interface type has no settings widget, an empty string should be returned.
     *
     * @return Settings group name, or empty string.
     */
    virtual QString settingsGroupName() = 0;

    /**
     * Called when the user clicks OK in the settings dialog.
     * @param pSettingsWidget Settings widget that was previously passed in
     *          in the call to settingsWidget().
     */
    virtual void settingsDialogAccepted(QWidget* pSettingsWidget) = 0;
};

#endif // CABSTRACTPAGEINTERFACE

