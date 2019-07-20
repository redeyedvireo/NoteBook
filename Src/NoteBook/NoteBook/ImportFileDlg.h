#ifndef IMPORTFILEDLG_H
#define IMPORTFILEDLG_H

#include <QDialog>
#include "ui_ImportFileDlg.h"

class CImportFileDlg : public QDialog
{
	Q_OBJECT

public:
	CImportFileDlg(QWidget *parent);
	~CImportFileDlg();

	void SetNameFilter(const QString& nameFilter);

	QString GetSelectedFile()	{ return m_selectedFilePath; }

private slots:
	void on_browseButton_clicked();
	void on_importButton_clicked();

private:
	Ui::ImportFileDlg	ui;

	QString				m_nameFilter;
	QString				m_selectedFilePath;
};

#endif // IMPORTFILEDLG_H
