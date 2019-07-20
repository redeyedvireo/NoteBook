#ifndef EXPORTDLG_H
#define EXPORTDLG_H

#include <QDialog>
#include <QDir>
#include "PageData.h"
#include "ui_ExportDlg.h"

class CDatabase;

class CExportDlg : public QDialog
{
	Q_OBJECT

public:
    CExportDlg(CDatabase *db, QWidget *parent = 0);
	~CExportDlg();

	enum
	{
		kRichText = 0,
		kPlainText
	};

	ENTITY_LIST GetPageList();

	/*
	 *	Indicates whether the user wants to export as rich text
	 *	or plain text.
	 *	@Return kRichText or kPlainText.
	 */
	int ExportFormat();

	/*
	 *	The path to the destination file.
	 */
	QString FilePath();

protected:
	void PopulatePages();
	void AddPageTitle(SHARED_CPAGEDATA pageData);
	ENTITY_ID GetPageIdForRow(int row);
	bool AnyPagesSelected();
	void UpdateOkButton();

private slots:
	void on_allButton_clicked();
	void on_noneButton_clicked();
	void on_pathEdit_textChanged();
	void on_browseButton_clicked();
	void on_pageListWidget_itemClicked(QListWidgetItem *item);

private:
	Ui::CExportDlg ui;

    CDatabase   *m_db;

	QDir		m_currentDir;
};

#endif // EXPORTDLG_H
