#ifndef NOTEBOOK_H
#define NOTEBOOK_H

#include <QMainWindow>
#include <QDir>
#include "NavigationPane.h"
#include "Preferences.h"
#include "PageHistoryWidget.h"
#include "CEditorManager.h"
#include "ui_notebook.h"

// List of the QObjects returned from the plugin loader
typedef QList<QObject*>  PLUGIN_LIST;

#define kMaxRecentFiles	20

// Editor stacked widget constants
#define kRichTextEdit   0
#define kToDoEdit       1


class AbstractPluginServices;
class CDatabase;

class CNoteBook : public QMainWindow
{
	Q_OBJECT

public:
    CNoteBook(CDatabase *db, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~CNoteBook();

    /**
     * Performs application initialization.  This function must be called
     * at startup.
     */
    void initialize();

	ENTITY_ID GetCurrentPageId()	{ return m_currentPageId; }
	void SetCurrentPageId(ENTITY_ID pageId)		{ m_currentPageId = pageId; }

public slots:
	void on_actionNew_Notebook_triggered();
	void on_actionOpen_Notebook_triggered();
	void OnOpenRecentFile();
	void OnImportMenuItemTriggered();
	void on_savePageButton_clicked();
	void on_actionNew_Page_triggered();
    void on_actionNew_To_Do_List_triggered();
	void on_actionNew_Top_Level_Page_triggered();
	void on_actionNew_Folder_triggered();
	void on_actionNew_Top_Level_Folder_triggered();
	void on_actionClose_triggered();
	void on_actionSettings_triggered();
	void on_actionPage_Info_triggered();

	void on_actionAdd_Page_to_Favorites_triggered();

	void on_actionAbout_NoteBook_triggered();
	void on_actionAbout_Qt_triggered();

	void on_actionExport_triggered();
    void on_actionImportPage_triggered();

    void OnPageClicked(ENTITY_ID pageId);
	void OnPageSelected(ENTITY_ID pageId);
	void OnNewBlankPageCreated(CPageData pageData);
	void OnPageTitleChanged(CPageData pageData);
	void OnPageDeleted(ENTITY_ID pageId);
	void OnPageDefavorited(ENTITY_ID pageId);
	void OnRemovePageFromFavorites();

	void OnPageModified();
	void OnTagsModified();

signals:
	void MW_NewBlankPageCreated(CPageData pageData);
	void MW_PageSaved(CPageData pageData);
	void MW_PageTitleChanged(CPageData pageData);
	void MW_PageDeleted(ENTITY_ID pageId);
	void MW_PageImported(CPageData pageData);
	void MW_PageUpdatedByImport(CPageData pageData);
	void MW_PageSelected(ENTITY_ID pageId);

protected:
    friend class PluginServices;

	void LoadSettings();
	void SaveSettings();
	void SetConnections();
	void closeEvent(QCloseEvent *event);

    /**
     * Returns the location of a generic settings directory that all
     * editors/plug-ins can use to save configuration data.
     *
     * @param bCreateDirectory
     * @return
     */
    QString GetSettingsDirectory(bool bCreateDirectory);

	bool OpenNotebookFile();
	void CloseNotebookFile();
	bool PageModified();
    CAbstractPageInterface *currentPageInterface();

	/*
	 *	Checks if the user has made any changes to the current page, and if so,
	 *	asks if he would like to save it.
	 */
	void CheckSavePage();

    /*
     * Creates a new page.
     * @param pageTitle Title of the new page.  If this is not specified, the
     *          item will be editable in the Page tree so the user can enter
     *          a title.
     */
    void CreateNewPage(PAGE_TYPE pageType, QString pageTitle=QString());

    /**
     * Creates a new To Do list page.
     * @brief CreateNewToDoList
     * @param pageTitle Title of the new page.  If this is not specified, the
     *          item will be editable in the Page tree so the user can enter
     *          a title.
     */
    void CreateNewToDoList(QString pageTitle=QString());

	void ClearAllControls();
	void ClearPageEditControls();
	void EnableDataEntry(bool bEnable);
	void PopulateNavigationControls(const QString& pageOrderStr);
	void DisplayInitialPage();

	/*
	 *	Displays the page that was last displayed when the
	 *	Notebook was last closed.
	 */
	void DisplayLastEntry();

	void SetAppTitle();
    void DisplayPage(CPageData& thePage, QStringList imageNames = QStringList());

	void LoadPlugins();
	void InitPlugins();
    QMenu *CreateImportMenu();

	void AddFileToRecentFilesList(const QString& pathName);
	void UpdateRecentFilesMenu();

private:
	Ui::NoteBookClass ui;

    CDatabase           *m_db;

	/*
	 *	Path and name of the current notebook file
	 */
	QDir				m_notebookDir;
	QString				m_notebookFilename;
    QString             m_notebookLastFilename;     // Most-recently open file
	QStringList			m_recentFileList;

	bool				m_bTagsModified;

	ENTITY_ID			m_currentPageId;
	ENTITY_ID			m_currentPageParentId;		// Parent to be used when saving next page

	CNavigationPane		m_navPane;

	/*
	 *	Application preferences.
	 */
	CPreferences		m_prefs;

    /*
     * Editor manager
     */
    CEditorManager      *m_editorManager;

    /*
     * Plugin Services pointer
     */
    AbstractPluginServices *pluginServices;

	/*
	 *	Plugin tracking
	 */
    PLUGIN_LIST			m_pluginList;
};

#endif // NOTEBOOK_H
