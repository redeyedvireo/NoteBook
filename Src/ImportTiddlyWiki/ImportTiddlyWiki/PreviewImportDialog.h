#ifndef PREVIEWIMPORTDIALOG_H
#define PREVIEWIMPORTDIALOG_H

#include <QDialog>
#include "ImportPageData.h"
#include "ui_PreviewImportDialog.h"


class CPreviewImportDialog : public QDialog
{
	Q_OBJECT

public:
	CPreviewImportDialog(const IMPORT_PAGE_LIST& inPageList, QWidget *parent = 0);
	~CPreviewImportDialog();

	PAGE_LIST GetPages();

protected:
	void InitControls();
	void DisplayKeepEntry(int index);
	void DisplaySkipEntry(int index);
	void MovePageToSkipList(int index);
	void MovePageToKeepList(int index);
	void UpdateKeepList();
	void UpdateSkipList();

private slots:
	void on_keepHorizontalScrollBar_valueChanged(int index);
	void on_skipHorizontalScrollBar_valueChanged(int index);
	void on_addPageButton_clicked();
	void on_removePageButton_clicked();

private:
	Ui::PreviewImportDialog		ui;

	int							m_currentKeepIndex;
	int							m_currentSkipIndex;

	IMPORT_PAGE_LIST			m_skipList;		// Tiddlers to skip
	IMPORT_PAGE_LIST			m_keepList;		// Tiddlers to keep
};

#endif // PREVIEWIMPORTDIALOG_H
