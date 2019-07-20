#include "stdafx.h"
#include "ImportFileDlg.h"
#include <QCompleter>
#include <QDirModel>
#include <QFileDialog>


//--------------------------------------------------------------------------
CImportFileDlg::CImportFileDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	// Set up a QCompleter for the edit control
	QCompleter*	tempCompleter = new (std::nothrow) QCompleter(this);
	if (tempCompleter != NULL)
	{
		QDirModel*	pTempDirModel = new (std::nothrow) QDirModel(tempCompleter);

		if (pTempDirModel != NULL)
		{
			tempCompleter->setModel(pTempDirModel);
			ui.fileChooseEdit->setCompleter(tempCompleter);
		}
	}
}

//--------------------------------------------------------------------------
CImportFileDlg::~CImportFileDlg()
{

}

//--------------------------------------------------------------------------
void CImportFileDlg::SetNameFilter( const QString& nameFilter )
{
	m_nameFilter = nameFilter;
}

//--------------------------------------------------------------------------
void CImportFileDlg::on_browseButton_clicked()
{
	QFileDialog		fileDialog(this, "Import File");

	fileDialog.setFileMode(QFileDialog::ExistingFile);
	fileDialog.setNameFilter(m_nameFilter);

	if (fileDialog.exec())
	{
		ui.fileChooseEdit->setText(fileDialog.selectedFiles()[0]);
	}
}

//--------------------------------------------------------------------------
void CImportFileDlg::on_importButton_clicked()
{
	m_selectedFilePath = ui.fileChooseEdit->text();

	accept();
}
