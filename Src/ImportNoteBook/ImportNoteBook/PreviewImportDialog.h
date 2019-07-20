#ifndef PREVIEWIMPORTDIALOG_H
#define PREVIEWIMPORTDIALOG_H

#include <QDialog>
#include "../../NoteBook/NoteBook/PageData.h"
#include "ui_PreviewImportDialog.h"

class CImportDatabase;

class CPreviewImportDialog : public QDialog
{
	Q_OBJECT

public:
	CPreviewImportDialog(QWidget *parent, PAGE_HASH& pageHash, CImportDatabase *pDatabase);
	~CPreviewImportDialog();

	PAGE_LIST GetPages();

protected:
	void PopulatePages();
	void AddPageTitle(SHARED_CPAGEDATA pageData);
	ENTITY_ID GetPageIdForRow(int row);
	void DisplayPage(ENTITY_ID pageId);

private slots:
	void on_selectAllButton_clicked();
	void on_selectNoneButton_clicked();
	void on_listWidget_currentRowChanged(int currentRow);

private:
	Ui::CPreviewImportDialog ui;

	CImportDatabase			*m_pDatabase;
	PAGE_HASH				m_pageHash;
};

#endif // PREVIEWIMPORTDIALOG_H
