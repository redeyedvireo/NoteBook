#include "stdafx.h"
#include "notebook.h"
#include "AboutDialog.h"
#include "PrefsDialog.h"
#include "SetPassword.h"
#include "PageTitleList.h"
#include "PageCache.h"
#include "TagCache.h"
#include "PluginInterface.h"
#include "ImportFileDlg.h"
#include "Importer.h"
#include "Exporter.h"
#include "PageInfoDlg.h"
#include "ClickableLabel.h"
#include "ExportDlg.h"
#include "PluginServices.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif


#define kAppName		"NoteBook"


//--------------------------------------------------------------------------
CNoteBook::CNoteBook(CDatabase *db, QWidget *parent, Qt::WindowFlags flags)
    : m_db(db), QMainWindow(parent, flags)
{
	ui.setupUi(this);

    m_db->SetParent(this);

	m_notebookDir.setPath(QCoreApplication::applicationDirPath());
	m_notebookFilename = "";
    m_notebookLastFilename = "";
	m_currentPageId = kInvalidPageId;
	m_currentPageParentId = kInvalidPageId;
	m_bTagsModified = false;

    pluginServices = new PluginServices(this, m_db);

	m_navPane.SetControlPointers(ui.pageTree);

    m_editorManager = new CEditorManager(ui.editorStackedWidget);

	SetConnections();

	m_prefs.SetAppDefaults();

	ui.navigationTabWidget->setCurrentIndex(0);
	ui.auxNavigationTabWidget->setCurrentIndex(0);
}

//--------------------------------------------------------------------------
CNoteBook::~CNoteBook()
{
}

//--------------------------------------------------------------------------
void CNoteBook::SetConnections()
{
    connect(ui.pageTree, SIGNAL(PageSelected(ENTITY_ID)), this, SLOT(OnPageSelected(ENTITY_ID)));
    connect(ui.pageTree, SIGNAL(NewBlankPageCreated(CPageData)), this, SLOT(OnNewBlankPageCreated(CPageData)));
    connect(ui.pageTree, SIGNAL(PT_PageTitleChanged(CPageData)), this, SLOT(OnPageTitleChanged(CPageData)));
    connect(ui.pageTree, SIGNAL(PT_PageDeleted(ENTITY_ID)), this, SLOT(OnPageDeleted(ENTITY_ID)));
    connect(ui.pageTree, SIGNAL(PT_OnCreateNewPage()), this, SLOT(on_actionNew_Page_triggered()));
    connect(ui.pageTree, SIGNAL(PT_OnCreateNewToDoList()), this, SLOT(on_actionNew_To_Do_List_triggered()));
    connect(ui.pageTree, SIGNAL(PT_OnCreateNewFolder()), this, SLOT(on_actionNew_Folder_triggered()));
    connect(ui.pageTree, SIGNAL(PT_OnCreateNewTopLevelPage()), this, SLOT(on_actionNew_Top_Level_Page_triggered()));
    connect(ui.pageTree, SIGNAL(PT_OnCreateNewTopLevelFolder()), this, SLOT(on_actionNew_Top_Level_Folder_triggered()));
    connect(ui.pageTitleList, SIGNAL(PTL_PageSelected(ENTITY_ID)), this, SLOT(OnPageSelected(ENTITY_ID)));
    connect(ui.dateTree, SIGNAL(PageSelected(ENTITY_ID)), this, SLOT(OnPageSelected(ENTITY_ID)));
    connect(ui.tagList, SIGNAL(TL_PageSelected(ENTITY_ID)), this, SLOT(OnPageSelected(ENTITY_ID)));

    // Reflect signals from controls back to all controls
    connect(ui.pageTree, SIGNAL(NewBlankPageCreated(CPageData)), this, SIGNAL(MW_NewBlankPageCreated(CPageData)));
    connect(ui.pageTree, SIGNAL(PT_PageTitleChanged(CPageData)), this, SIGNAL(MW_PageTitleChanged(CPageData)));
    connect(ui.pageTree, SIGNAL(PT_PageDeleted(ENTITY_ID)), this, SIGNAL(MW_PageDeleted(ENTITY_ID)));
    connect(ui.favoritesWidget, SIGNAL(FW_PageSelected(ENTITY_ID)), this, SIGNAL(MW_PageSelected(ENTITY_ID)));
    connect(ui.recentlyViewedList, SIGNAL(PHW_PageSelected(ENTITY_ID)), this, SIGNAL(MW_PageSelected(ENTITY_ID)));
    connect(ui.searchWidget, SIGNAL(SW_PageSelected(ENTITY_ID)), this, SIGNAL(MW_PageSelected(ENTITY_ID)));
    connect(ui.pageTitleList, SIGNAL(PTL_PageSelected(ENTITY_ID)), this, SIGNAL(MW_PageSelected(ENTITY_ID)));
    connect(ui.dateTree, SIGNAL(PageSelected(ENTITY_ID)), this, SIGNAL(MW_PageSelected(ENTITY_ID)));
    connect(ui.tagList, SIGNAL(TL_PageSelected(ENTITY_ID)), this, SIGNAL(MW_PageSelected(ENTITY_ID)));

    connect(ui.tagsEdit, SIGNAL(textEdited(QString)), this, SLOT(OnTagsModified()));

    // Handle signals from the Page History Widget
    connect(ui.recentlyViewedList, SIGNAL(PHW_PageSelected(ENTITY_ID)), this, SLOT(OnPageSelected(ENTITY_ID)));

    // Handle signals from the Search Widget
    connect(ui.searchWidget, SIGNAL(SW_PageSelected(ENTITY_ID)), this, SLOT(OnPageSelected(ENTITY_ID)));

    // Handle signals from the Favorites Widget
    connect(ui.favoritesWidget, SIGNAL(FW_PageSelected(ENTITY_ID)), this, SLOT(OnPageSelected(ENTITY_ID)));
    connect(ui.favoritesWidget, SIGNAL(FW_PageDefavorited(ENTITY_ID)), this, SLOT(OnPageDefavorited(ENTITY_ID)));
    connect(ui.titleLabelWidget, SIGNAL(SetPageAsFavorite()), this, SLOT(on_actionAdd_Page_to_Favorites_triggered()));
    connect(ui.titleLabelWidget, SIGNAL(SetPageAsNonFavorite()), this, SLOT(OnRemovePageFromFavorites()));
    connect(ui.titleLabelWidget, SIGNAL(GotoPage(ENTITY_ID)), this, SLOT(OnPageSelected(ENTITY_ID)));
    connect(ui.titleLabelWidget, SIGNAL(GotoPage(ENTITY_ID)), ui.pageTree, SLOT(OnPageSelected(ENTITY_ID)));
}

//--------------------------------------------------------------------------
void CNoteBook::initialize()
{
    LoadPlugins();
    InitPlugins();

    LoadSettings();
    UpdateRecentFilesMenu();

    ui.pageTree->Initialize(this, m_db);
    ui.pageTitleList->Initialize(this);
    ui.dateTree->Initialize(this);
    ui.tagList->Initialize(this);
    ui.recentlyViewedList->Initialize(this, m_db);
    ui.searchWidget->Initialize(this, m_db);
    ui.favoritesWidget->Initialize(this, m_db);
    ui.titleLabelWidget->Initialize(pluginServices);

    // Connect signals from editors
    QList<CAbstractPageInterface*>  allEditors = m_editorManager->getEditors();
    foreach (CAbstractPageInterface* pEditor, allEditors)
    {
        QWidget     *pEditorWidget = pEditor->editorWidget();

        connect(pEditorWidget, SIGNAL(TextModified()), this, SLOT(OnPageModified()));
        connect(pEditorWidget, SIGNAL(GotoPage(ENTITY_ID)), this, SLOT(OnPageClicked(ENTITY_ID)));
        connect(pEditorWidget, SIGNAL(GotoPage(ENTITY_ID)), this, SIGNAL(MW_PageSelected(ENTITY_ID)));
        connect(pEditorWidget, SIGNAL(Message(QString)), ui.messageLabel, SLOT(setText(QString)));
        connect(pEditorWidget, SIGNAL(SavePage()), this, SLOT(on_savePageButton_clicked()));
    }

    // Initialization done.  Now load NoteBook file, if appropriate.
    int  startupPref = m_prefs.GetIntPref("general-startupload");

    if (startupPref == CPreferences::eStartupLoadPreviousNoteBook)
    {
        // If there was a file opened previously, open it again.
        if (! m_notebookFilename.isEmpty())
        {
            OpenNotebookFile();
        }
        else
        {
            EnableDataEntry(false);
        }
    }
    else
    {
        EnableDataEntry(false);
    }
}

//--------------------------------------------------------------------------
void CNoteBook::LoadSettings()
{
	QSettings	settingsObj(QSettings::IniFormat, QSettings::UserScope, kAppName, kAppName);

	// Most recent file
	settingsObj.beginGroup("files");

	if (settingsObj.contains("lastdirectory"))
	{
		QVariant	tempName = settingsObj.value("lastdirectory");
		m_notebookDir = tempName.toString();
	}

	if (settingsObj.contains("lastfile"))
	{
		QVariant	tempName = settingsObj.value("lastfile");
		m_notebookFilename = tempName.toString();
	}

	settingsObj.endGroup();

	// General settings
	settingsObj.beginGroup("general");

	if (settingsObj.contains("startupload"))
	{
		bool		bOk;
		QVariant	tempInt = settingsObj.value("startupload");
		m_prefs.SetIntPref("general-startupload", tempInt.toInt(&bOk));
	}

	settingsObj.endGroup();


	// Window size and position
	settingsObj.beginGroup("window");

	if (settingsObj.contains("size"))
	{
		resize(settingsObj.value("size").toSize());
	}

	if (settingsObj.contains("pos"))
	{
		move(settingsObj.value("pos").toPoint());
	}

	settingsObj.endGroup();

	// Get recent files list
	settingsObj.beginGroup("recentfiles");

	// Loop through the recent files
	m_recentFileList.clear();
	for (int curFileNum = 0; curFileNum < kMaxRecentFiles; curFileNum++)
	{
		QString		fileNumStr = QString("file%1").arg(curFileNum);

		if (settingsObj.contains(fileNumStr))
		{
			QVariant	tempName = settingsObj.value(fileNumStr);
			QString		fileString = tempName.toString();

			if (! fileString.isEmpty())
			{
				m_recentFileList.append(fileString);
			}
		}
		else
			break;
	}

#if QT_VERSION >= 0x040500
	// QStringList::removeDuplicates() requires Qt 4.5
	m_recentFileList.removeDuplicates();
#endif

	settingsObj.endGroup();

    // Give the plug-ins a chance to see the settings
    foreach (QObject* pPlugin, m_pluginList)
    {
        CPluginInterface*  pPluginInterface = qobject_cast<CPluginInterface *>(pPlugin);
        if (pPluginInterface != NULL)
        {
            pPluginInterface->loadSettings(settingsObj);
        }
    }
}

//--------------------------------------------------------------------------
void CNoteBook::SaveSettings()
{
	QSettings	settingsObj(QSettings::IniFormat, QSettings::UserScope, kAppName, kAppName);

	// Previous log file
	settingsObj.beginGroup("files");
	settingsObj.setValue("lastdirectory", m_notebookDir.absolutePath());
    settingsObj.setValue("lastfile", m_notebookLastFilename);
	settingsObj.endGroup();

	// General settings
	settingsObj.beginGroup("general");
	settingsObj.setValue("startupload", m_prefs.GetIntPref("general-startupload"));
	settingsObj.endGroup();

	// Window size and position
	settingsObj.beginGroup("window");
	settingsObj.setValue("size", size());
	settingsObj.setValue("pos", pos());
	settingsObj.endGroup();

	// Recent notebook files
	settingsObj.beginGroup("recentfiles");

	int curFileNum = 0;
	foreach (const QString& recentFile, m_recentFileList)
	{
		QString		fileNumStr = QString("file%1").arg(curFileNum++);
		settingsObj.setValue(fileNumStr, recentFile);
	}
	settingsObj.endGroup();

    // Give the plug-ins a chance to save their settings
    foreach (QObject* pPlugin, m_pluginList)
    {
        CPluginInterface*  pPluginInterface = qobject_cast<CPluginInterface *>(pPlugin);
        if (pPluginInterface != nullptr)
        {
            pPluginInterface->saveSettings(settingsObj);
        }
    }
}

//--------------------------------------------------------------------------
void CNoteBook::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

	CloseNotebookFile();

	SaveSettings();

    // Give plug-ins a chance to save data and gracefully shut down
    foreach (QObject* pPlugin, m_pluginList)
    {
        CPluginInterface*  pPluginInterface = qobject_cast<CPluginInterface *>(pPlugin);
            pPluginInterface->shutdown();
    }
}

//--------------------------------------------------------------------------
QString CNoteBook::GetSettingsDirectory(bool bCreateDirectory)
{
#if QT_VERSION >= 0x050000
    QDir		settingsDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
#else
    QDir		settingsDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
#endif

    // Make sure the settings path ends with a directory name corresponding to the application name
    if (settingsDir.dirName() != kAppName)
    {
        // It does not.  Add the application name to the path.
        settingsDir.setPath(settingsDir.absoluteFilePath(kAppName));
    }

    qDebug() << "CNoteBook::GetStyleFilePath: styles stored in " << settingsDir.absolutePath();

    if (! settingsDir.exists())
    {
        // The directory does not exist.
        if (bCreateDirectory)
        {
            settingsDir.mkpath(settingsDir.absolutePath());
        }
    }

    return settingsDir.absolutePath();
}

//----------------------------------------------------------------
void CNoteBook::AddFileToRecentFilesList(const QString& pathName)
{
	// Determine if the pathName already exists
	if (m_recentFileList.contains(pathName))
	{
		// The string is there, so remove it, and add it to the top
		m_recentFileList.removeOne(pathName);
	}

	if (m_recentFileList.size() == kMaxRecentFiles)
	{
		m_recentFileList.removeLast();
	}

	// Add new item to the top of the list
	m_recentFileList.prepend(pathName);

	UpdateRecentFilesMenu();
}

//----------------------------------------------------------------
void CNoteBook::UpdateRecentFilesMenu()
{
	QMenu		*pRecentMenu = ui.actionRecent_Notebooks->menu();

	if (pRecentMenu == NULL)
	{
		pRecentMenu = new (std::nothrow) QMenu();

		if (pRecentMenu != NULL)
		{
			ui.actionRecent_Notebooks->setMenu(pRecentMenu);
		}
	}

	if (pRecentMenu != NULL)
	{
		pRecentMenu->clear();

		foreach (QString recentFile, m_recentFileList)
		{
			QAction *pAction = pRecentMenu->addAction(recentFile);

			connect(pAction, SIGNAL(triggered()), this, SLOT(OnOpenRecentFile()));
		}
	}
}

//--------------------------------------------------------------------------
void CNoteBook::OnOpenRecentFile()
{
	if (QAction *action = qobject_cast<QAction*>(sender()))
	{
		QString		tempPathname = action->text();

		if (! tempPathname.isEmpty())
		{
			// TODO: This code is the same as in on_actionOpen_Notebook_triggered().
			//		Should refactor this into a single function.

			// If there is a notebook already open, close it first
            CloseNotebookFile();
            ClearAllControls();

			QFileInfo	fileInfo(tempPathname);

			m_notebookDir = fileInfo.dir();
			m_notebookFilename = fileInfo.fileName();

			if (OpenNotebookFile())
			{
				EnableDataEntry(true);
			}
		}
	}
}

//--------------------------------------------------------------------------
bool CNoteBook::OpenNotebookFile()
{
	if (! m_notebookFilename.isEmpty())
	{
		if (! m_notebookDir.exists(m_notebookFilename))
			return false;		// File does not exist

        m_db->OpenDatabase(m_notebookDir.absoluteFilePath(m_notebookFilename));

		// Determine if the database uses a password, and if so,
		// get the password from the user.
        if (m_db->IsPasswordProtected())
		{
			QString	pw;

			// This log file uses a password.  Get the password from the user.
			// Let the user try 3 times to enter it correctly.
			for (int userTry = 0; userTry < 3; userTry++)
			{
				bool	bOk;
				pw = QInputDialog::getText(this, "NoteBook - Enter Password",
					"Enter notebook password:", QLineEdit::Password, QString(), &bOk);

				if (! bOk)
				{
					// User clicked Cancel
                    m_db->CloseDatabase();
					return false;
				}

                if (m_db->PasswordMatch(pw))
					break;		// Correct password!

				// Password entered is incorrect
				if (userTry == 2)
				{
					// Third try and no match.  We're done.
                    m_db->CloseDatabase();
					return false;
				}
			}

			// Password match.  Save it in the database object in memory
			// (this does not store it on the disk.)
            m_db->SetPasswordInMemory(pw);
		}

		// Read the page order for the notebook (if one exists)
		QString		pageOrderStr;

        m_db->GetPageOrder(pageOrderStr);

		PopulateNavigationControls(pageOrderStr);

		// Set page history in the page history widget
		QString		pageHistoryStr;

        if (m_db->GetPageHistory(pageHistoryStr))
		{
			ui.recentlyViewedList->SetPageHistory(pageHistoryStr);
		}

		// Read favorites
		ENTITY_LIST		favoritePageIds;

        if (m_db->GetFavoritePages(favoritePageIds))
		{
			ui.favoritesWidget->AddPages(favoritePageIds);
		}

		// Display the entry that was displayed when the database was last closed
		DisplayLastEntry();

		DisplayInitialPage();

		SetAppTitle();
		AddFileToRecentFilesList(m_notebookDir.absoluteFilePath(m_notebookFilename));

        m_notebookLastFilename = m_notebookFilename;

		return true;
	}

	return false;
}

//--------------------------------------------------------------------------
void CNoteBook::CloseNotebookFile()
{
    if (m_db->IsDatabaseOpen())
	{
		CheckSavePage();

		// Save page history
		QString		pageHistoryStr = ui.recentlyViewedList->GetPageHistory();
        m_db->SetPageHistory(pageHistoryStr);

		// Save Page order
        ui.pageTree->WritePageOrderToDatabase();

        m_db->CloseDatabase();

        m_notebookFilename = "";
        SetAppTitle();
	}
}

//--------------------------------------------------------------------------
bool CNoteBook::PageModified()
{
    CAbstractPageInterface  *pDocumentEditor = currentPageInterface();

    if (pDocumentEditor)
    {
        return (pDocumentEditor->isDocumentModified() || m_bTagsModified);
    }
    else
        return false;
}

//--------------------------------------------------------------------------
void CNoteBook::OnPageModified()
{
	SetAppTitle();

	ui.savePageButton->setEnabled(true);
}

//--------------------------------------------------------------------------
void CNoteBook::OnTagsModified()
{
	m_bTagsModified = true;

	OnPageModified();
}

//--------------------------------------------------------------------------
void CNoteBook::CheckSavePage()
{
    if (PageModified())
    {
        // Ask user if he wants to save the log entry.
        QMessageBox::StandardButton result = QMessageBox::question(this, "NoteBook - Save Entry?",
                "The current page has not been saved.  Would you like to save it?",
                QMessageBox::Yes | QMessageBox::No);

        if (result == QMessageBox::Yes)
        {
            on_savePageButton_clicked();
        }
    }
}


/************************************************************************/
/* Slots                                                                */
/************************************************************************/

void CNoteBook::on_actionNew_Notebook_triggered()
{
	// If there is a notebook already open, close it first
    if (m_db->IsDatabaseOpen())
	{
		CheckSavePage();		// Save current page, if necessary.
        m_db->CloseDatabase();
		ClearAllControls();
	}

	QString		tempDbPathname;

	tempDbPathname = QFileDialog::getSaveFileName(this,
		"NoteBook - New Notebook File",
		m_notebookDir.absolutePath(),
		"Notebook files (*.nbk)");

	if (! tempDbPathname.isEmpty())
	{
		CSetPassword		pwDlg;

		pwDlg.exec();

		if (pwDlg.result() == QDialog::Accepted)
		{
			QString  password;

			password.clear();
			ClearAllControls();

			// Delete the file if it exists
			if (QFile::exists(tempDbPathname))
			{
				QFile::remove(tempDbPathname);
			}

			QFileInfo	fileInfo(tempDbPathname);

			m_notebookDir = fileInfo.dir();
			m_notebookFilename = fileInfo.fileName();
            m_notebookLastFilename = m_notebookFilename;

            AddFileToRecentFilesList(m_notebookDir.absoluteFilePath(m_notebookFilename));

            m_db->OpenDatabase(m_notebookDir.absoluteFilePath(m_notebookFilename));

			if (pwDlg.PasswordExists())
			{
				password = pwDlg.GetPassword();

                m_db->StorePassword(password);
			}

			SetAppTitle();
		}
	}
}

//--------------------------------------------------------------------------
void CNoteBook::on_actionOpen_Notebook_triggered()
{
	QString		tempDbPathname;

	CheckSavePage();		// Save current page, if necessary.

	tempDbPathname = QFileDialog::getOpenFileName(this,
		"NoteBook - Open NoteBook File",
		m_notebookDir.absolutePath(),
		"NoteBook files (*.nbk)");

	if (! tempDbPathname.isEmpty())
	{
		// If there is a notebook already open, close it first
        CloseNotebookFile();
        ClearAllControls();

		QFileInfo	fileInfo(tempDbPathname);

		m_notebookDir = fileInfo.dir();
		m_notebookFilename = fileInfo.fileName();

		if (OpenNotebookFile())
		{
			EnableDataEntry(true);
		}
	}
}

//--------------------------------------------------------------------------
void CNoteBook::OnImportMenuItemTriggered()
{
	QAction*	pAction = qobject_cast<QAction*>(sender());

	if (pAction != NULL)
	{
		QObject*	pPlugin = pAction->data().value<QObject*>();

		if (pPlugin != NULL)
		{
			CPluginInterface*  pPluginInterface = qobject_cast<CPluginInterface *>(pPlugin);
			if (pPluginInterface != NULL)
			{
				QString		importStr = pPluginInterface->MenuItemString();

				CImportFileDlg	importFileDlg(this);

				importFileDlg.exec();

				if (importFileDlg.result() == QDialog::Accepted)
				{
					QString			selectedFile(QString(importFileDlg.GetSelectedFile()));
					PAGE_LIST pageList = pPluginInterface->DoImport(this, selectedFile);

                    CImporter	importerObj(m_db, this);

					// TODO: May need new signals instead of MW_PageSaved().
					connect(&importerObj, SIGNAL(IMPORTER_PageAdded(CPageData)), this, SIGNAL(MW_PageImported(CPageData)));
					connect(&importerObj, SIGNAL(IMPORTER_PageUpdated(CPageData)), this, SIGNAL(MW_PageUpdatedByImport(CPageData)));

					importerObj.DoImport(pageList);
					EnableDataEntry(true);
				}
			}
		}
	}
}

//--------------------------------------------------------------------------
void CNoteBook::on_savePageButton_clicked()
{
	// Get ID of the log entry from the list control.  If it is a new log entry,
	// the ID will be kTempItemId.
	ENTITY_ID		pageId = GetCurrentPageId();

    CAbstractPageInterface  *pDocumentEditor = currentPageInterface();

	// TODO: The page type will eventually come from a control.  For now
	// user text is being hard-coded.
	CPageData		pageData;

    pageData.m_pageType = pDocumentEditor->documentType();

    pageData.m_title = ui.titleLabelWidget->GetPageTitleLabel();
    pageData.m_contentString = pDocumentEditor->getPageContents();
	pageData.m_tags = ui.tagsEdit->text();

	if (pageId != kInvalidPageId)
	{
		// First, get the page from the database so that the existing tags can be known.
		// The reason for obtaining the previous tags is to make it easier to determine
		// if the user has deleted any tags when saving.

		// TODO: When a more general Database Get command is written, use that to get
		//		just the tags.

		CPageData		prevData;
        if (! m_db->GetPage(pageId, prevData))
		{
			// If the page can't be obtained from the database, something is terribly wrong.
			Q_ASSERT(false);
		}

		pageData.m_pageId = pageId;
		pageData.m_bIsFavorite = prevData.m_bIsFavorite;

        if (m_db->UpdatePage(pageData))
		{
			// TODO: When the capability to fetch just certain fields through a
			//       QFlags object is implemented, this call to GetPage can
			//       be changed to retrieve just the fields needed to update
			//       the UI.
            m_db->GetPage(pageId, pageData);

			m_bTagsModified = false;
            pDocumentEditor->setDocumentModified(false);

			// Update UI
			pageData.m_previousTags = prevData.m_tags;		// Keep track of previous tags
			emit MW_PageSaved(pageData);

			SetAppTitle();

			// Disable the Save button
			ui.savePageButton->setEnabled(false);
		}
	}
	else
	{
		// The page was not found - should never happen
		Q_ASSERT(false);
	}
}


//--------------------------------------------------------------------------
void CNoteBook::on_actionNew_Page_triggered()
{
    CreateNewPage(kPageTypeUserText);
}

//--------------------------------------------------------------------------
void CNoteBook::on_actionNew_To_Do_List_triggered()
{
    CreateNewPage(kPageTypeToDoList);
}

//--------------------------------------------------------------------------
void CNoteBook::on_actionNew_Top_Level_Page_triggered()
{
    CheckSavePage();

	m_currentPageId = kInvalidPageId;
	m_currentPageParentId = kInvalidPageId;
	ClearPageEditControls();
	m_navPane.NewItem(eNewPage, ePageAddTopLevel);
}

//--------------------------------------------------------------------------
void CNoteBook::on_actionNew_Folder_triggered()
{
    CheckSavePage();

	m_currentPageId = kInvalidPageId;
	m_currentPageParentId = kInvalidPageId;
	ClearPageEditControls();
	m_navPane.NewItem(eNewFolder);
}

//--------------------------------------------------------------------------
void CNoteBook::on_actionNew_Top_Level_Folder_triggered()
{
    CheckSavePage();

	m_currentPageId = kInvalidPageId;
	m_currentPageParentId = kInvalidPageId;
	ClearPageEditControls();
	m_navPane.NewItem(eNewFolder, ePageAddTopLevel);
}

//--------------------------------------------------------------------------
void CNoteBook::on_actionClose_triggered()
{
	CloseNotebookFile();
	ClearAllControls();
	EnableDataEntry(false);
}

//--------------------------------------------------------------------------
void CNoteBook::on_actionSettings_triggered()
{
    CPrefsDialog	dlg(m_prefs, m_editorManager->getEditors(), this);

	dlg.exec();

	if (dlg.result() == QDialog::Accepted)
	{
		// Apply settings

		// TODO: What needs to be done here?
	}
}

//--------------------------------------------------------------------------
void CNoteBook::on_actionPage_Info_triggered()
{
	CPageData		pageData;

    if (m_db->GetPage(m_currentPageId, pageData))
	{
		CPageInfoDlg	dlg(pageData, this);

		dlg.exec();
	}
}

//--------------------------------------------------------------------------
void CNoteBook::on_actionAdd_Page_to_Favorites_triggered()
{
	if (m_currentPageId != kInvalidPageId)
	{
		ui.favoritesWidget->AddPage(m_currentPageId);

        m_db->SetPageFavoriteStatus(m_currentPageId, true);

		// Change favorites icon
        ui.titleLabelWidget->SetFavoritesIcon(true);
	}
}

//--------------------------------------------------------------------------
void CNoteBook::on_actionAbout_NoteBook_triggered()
{
	CAboutDialog	dlg(this);

	dlg.exec();
}

//--------------------------------------------------------------------------
void CNoteBook::on_actionAbout_Qt_triggered()
{
	qApp->aboutQt();
}

//--------------------------------------------------------------------------
void CNoteBook::on_actionExport_triggered()
{
    CExportDlg	dlg(m_db, this);

	dlg.exec();

	if (dlg.result() == QDialog::Accepted)
	{
		// User clicked Finish
		int				formatType = dlg.ExportFormat();
		ENTITY_LIST		pageIdList = dlg.GetPageList();
		QString			destPath = dlg.FilePath();

        CExporter		exporter(m_db, destPath, pageIdList);

		exporter.DoExport(formatType);
	}
}

//--------------------------------------------------------------------------
void CNoteBook::on_actionImportPage_triggered()
{
    QString     pathName;

    pathName = QFileDialog::getOpenFileName(this, tr("Import Page"), "", tr("Rich Text Pages (*.html)"));

    if (! pathName.isEmpty())
    {
        QString     pageTitle;

        pageTitle = QInputDialog::getText(this, tr("Page Name"), tr("Enter the name of the new page"));

        if (! pageTitle.isEmpty())
        {
            // Read the file.  This is assumed to be an HTML file containing the rich text for the page.
            QFile   inFile(pathName);

            if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                return;
            }

            QByteArray  fileContentArray;

            fileContentArray = inFile.readAll();

            inFile.close();

            QString     fileContents(fileContentArray);

            // Create a new page
            CreateNewPage(kPageTypeUserText, pageTitle);

            // Store the newly read page into the new NoteBook page
            // Note: It's OK to use ui.pageEdit to refer to the page, since the
            //       import function only imports text documents.
            CAbstractPageInterface  *pEditor = m_editorManager->getEditorForPageType(kPageTypeUserText);
            pEditor->setPageContents(fileContents, QStringList());

            // Save the page
            on_savePageButton_clicked();
        }
    }
}

//--------------------------------------------------------------------------
CAbstractPageInterface *CNoteBook::currentPageInterface()
{
    return m_editorManager->getActiveEditor();
}

//--------------------------------------------------------------------------
void CNoteBook::OnPageClicked(ENTITY_ID pageId)
{
    OnPageSelected((ENTITY_ID) pageId);
}

//--------------------------------------------------------------------------
void CNoteBook::OnPageSelected(ENTITY_ID pageId)
{
    CheckSavePage();

    CPageData   pageData;

    if (m_db->GetPage(pageId, pageData))
    {
        m_currentPageId = pageId;

        QStringList     imageNames;
        m_db->GetImageNamesForPage(m_currentPageId, imageNames);

        m_bTagsModified = false;

        DisplayPage(pageData, imageNames);

        // Add page to the page history
        ui.recentlyViewedList->AddItem(pageId, pageData.m_title);
    }
}

//--------------------------------------------------------------------------
void CNoteBook::OnNewBlankPageCreated( CPageData pageData )
{
    m_currentPageId = pageData.m_pageId;
    m_currentPageParentId = pageData.m_parentId;

    EnableDataEntry(true);

    // Add page to the page history
    ui.recentlyViewedList->AddItem(m_currentPageId, pageData.m_title);

    DisplayPage(pageData);
}

//--------------------------------------------------------------------------
void CNoteBook::OnPageTitleChanged(CPageData pageData)
{
	if (m_currentPageId == pageData.m_pageId)
	{
		// Update edit pane to display the new name, num modifications, and modification date.
        ui.titleLabelWidget->SetPageTitleLabel(pageData.m_title);
	}
}

//--------------------------------------------------------------------------
void CNoteBook::OnPageDeleted( ENTITY_ID pageId )
{
	if (m_currentPageId == pageId)
	{
		ClearPageEditControls();
	}
}

//--------------------------------------------------------------------------
void CNoteBook::OnPageDefavorited(ENTITY_ID pageId)
{
    m_db->SetPageFavoriteStatus(pageId, false);

	if (m_currentPageId == pageId)
	{
        ui.titleLabelWidget->SetFavoritesIcon(false);
	}
}

//--------------------------------------------------------------------------
void CNoteBook::OnRemovePageFromFavorites()
{
	OnPageDefavorited(m_currentPageId);
	ui.favoritesWidget->RemovePage(m_currentPageId);
}


/************************************************************************/
/* UI                                                                   */
/************************************************************************/

void CNoteBook::CreateNewPage(PAGE_TYPE pageType, QString pageTitle)
{
    PAGE_ADD            typeOfItem = eNewPage;

    CheckSavePage();

    m_currentPageId = kInvalidPageId;
    m_currentPageParentId = kInvalidPageId;

    ClearPageEditControls();

    if (pageType == kPageTypeUserText)
    {
        typeOfItem = eNewPage;
    }
    else
    {
        typeOfItem = eNewToDoListPage;
    }

    m_navPane.NewItem(typeOfItem, ePageAddDefault, pageTitle);
}

//--------------------------------------------------------------------------
void CNoteBook::ClearAllControls()
{
	// Clear data from all controls
	ClearPageEditControls();

	ui.pageTree->clear();
	ui.pageTitleList->clear();
	ui.dateTree->clear();
	ui.tagList->clear();
	ui.recentlyViewedList->clear();
	ui.searchWidget->Clear();
	ui.favoritesWidget->Clear();

	// TODO: If any additional controls are added, they must be cleared here.
}

//--------------------------------------------------------------------------
void CNoteBook::ClearPageEditControls()
{
    ui.titleLabelWidget->clear();

    CAbstractPageInterface  *pCurrentEditor = m_editorManager->getActiveEditor();

    pCurrentEditor->newDocument();
	ui.tagsEdit->clear();
}

//--------------------------------------------------------------------------
void CNoteBook::EnableDataEntry(bool bEnable)
{
	// Enable controls that allow data entry
    CAbstractPageInterface  *pDocumentEditor = currentPageInterface();

    pDocumentEditor->enableEditing(bEnable);
	ui.tagsEdit->setEnabled(bEnable);
}

//--------------------------------------------------------------------------
void CNoteBook::PopulateNavigationControls(const QString& pageOrderStr)
{
	PAGE_HASH				pageHash;
	PAGE_ID_TAG_CACHE		pageIdTagHash;

    m_db->GetPageList(pageHash);
	CPageCache::Instance()->AddPages(pageHash);

    m_db->GetTagList(pageIdTagHash);
	CTagCache::Instance()->AddTags(pageIdTagHash);

//	ui.pageTree->AddItems(pageHash);
	ui.pageTree->AddItemsNew(pageHash, pageOrderStr);
	ui.pageTitleList->AddItems(pageHash);
	ui.dateTree->AddItems(pageHash);
	ui.tagList->AddItems(pageIdTagHash);
}

//--------------------------------------------------------------------------
void CNoteBook::DisplayInitialPage()
{
	// Display a page
}

//--------------------------------------------------------------------------
void CNoteBook::DisplayLastEntry()
{
	// Display the entry that was displayed when the database was last closed

	ENTITY_ID	mostRecentlyViewedPageId = ui.recentlyViewedList->GetMostRecentlyViewedPage();

    if (mostRecentlyViewedPageId == kInvalidPageId)
	{
		// Just display the first entry in the database (if any)
        mostRecentlyViewedPageId = 1;
	}

    OnPageSelected(mostRecentlyViewedPageId);
    emit MW_PageSelected(mostRecentlyViewedPageId);     // Notify widgets that this page is being selected
}

//--------------------------------------------------------------------------
void CNoteBook::SetAppTitle()
{
	QString		strWindowTitle;

	if (! m_notebookFilename.isEmpty())
	{
		strWindowTitle = QString("NoteBook - %1").arg(m_notebookDir.absoluteFilePath(m_notebookFilename));

        if (PageModified())
        {
            strWindowTitle += "*";		// Add asterisk to indicate the page has been modified
        }
    }
	else
	{
		strWindowTitle = QString("NoteBook");
	}


	setWindowTitle(strWindowTitle);
}

//--------------------------------------------------------------------------
void CNoteBook::DisplayPage(CPageData& thePage, QStringList imageNames)
{
    CAbstractPageInterface  *pDocumentEditor;

    ui.titleLabelWidget->SetPageTitleLabel(thePage.m_title);

    // TODO: Since the page types are already fixed, plug-in types will
    //       need to be fixed as well.
    m_editorManager->activateEditor(thePage.m_pageType);
    pDocumentEditor = m_editorManager->getActiveEditor();

    if (!pDocumentEditor)
        return;

    if (thePage.m_pageType == kPageFolder)
    {
        pDocumentEditor->setPageContents(thePage.m_title, QStringList());
    }
    else
    {
        if (thePage.m_contentString.isEmpty())
        {
            pDocumentEditor->newDocument();
        }
        else
        {
            pDocumentEditor->setPageContents(thePage.m_contentString, imageNames);
        }
    }

	if (thePage.m_tags.isEmpty())
	{
		ui.tagsEdit->clear();
	}
	else
	{
		ui.tagsEdit->setText(thePage.m_tags);
	}

    ui.titleLabelWidget->SetFavoritesIcon(thePage.IsFavorite());

	SetAppTitle();		// To update "modified" indicator

	// Disable the Save button
	ui.savePageButton->setEnabled(false);
}



/************************************************************************/
/* P L U G I N   M A N A G E M E N T                                    */
/************************************************************************/

void CNoteBook::LoadPlugins()
{
    QDir pluginsDir(qApp->applicationDirPath());
#if defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin)
        {
            CPluginInterface*  pPlugin = qobject_cast<CPluginInterface *>(plugin);
            if (pPlugin != NULL)
            {
                if (pluginLoader.isLoaded())
                {
                    m_pluginList.append(plugin);
                }
                else
                {
                    qWarning() << "Could not load plug-in: " << fileName;
                    // TODO: Log this (when log files are supported)
                }
            }
        }
    }
}

//--------------------------------------------------------------------------
void CNoteBook::InitPlugins()
{
    QMenu*	pImportMenu = ui.menuImport;

    foreach (QObject* pPlugin, m_pluginList)
    {
        CPluginInterface*  pPluginInterface = qobject_cast<CPluginInterface *>(pPlugin);
        if (pPluginInterface != NULL)
        {
            pPluginInterface->initialize(this, pluginServices);

            if (pPluginInterface->capabilities().testFlag(CPluginInterface::Import))
            {
                QAction*	pAction = pImportMenu->addAction(QString("%1...").arg(pPluginInterface->MenuItemString()),
                                                                this,
                                                                SLOT(OnImportMenuItemTriggered()));

                // Add plugin's QObject to action data
                QVariant	tempVariant;
                qVariantSetValue(tempVariant, pPlugin);
                pAction->setData(tempVariant);
            }
            else if (pPluginInterface->capabilities().testFlag(CPluginInterface::Edit))
            {
                CAbstractPageInterface      *pPageInterface = pPluginInterface->getPageInterface();

                if (pPageInterface != NULL)
                {
                    int     pageType = pPluginInterface->getPageType();

                    m_editorManager->addEditor(pPageInterface, pageType);
                }
            }
            else
            {
                qWarning() << "Unknown plug-in type: " << pPlugin->objectName();
                // TODO: Log this (when log files are supported)
            }
        }
    }
}
