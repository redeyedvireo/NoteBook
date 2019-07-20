#include "stdafx.h"
#include "SelectStyleDlg.h"
#include "StyleDlg.h"
#include <QMessageBox>


//--------------------------------------------------------------------------
CSelectStyleDlg::CSelectStyleDlg(CStyleManager *styleManager, QWidget *parent)
	: m_styleManager(styleManager), QDialog(parent)
{
	ui.setupUi(this);

	LoadStyles();

	ui.styleList->setCurrentRow(0);
}

//--------------------------------------------------------------------------
CSelectStyleDlg::~CSelectStyleDlg()
{

}

//--------------------------------------------------------------------------
void CSelectStyleDlg::LoadStyles()
{
    QList<int>	styleIds = m_styleManager->GetStyleIds();

	foreach (int styleId, styleIds)
	{
        AddStyle(m_styleManager->GetStyle(styleId).strName, styleId);
	}
}

//--------------------------------------------------------------------------
void CSelectStyleDlg::on_styleList_currentRowChanged( int currentRow )
{
    ui.descriptionEdit->setText(m_styleManager->GetStyle(GetStyleIdForRow(currentRow)).strDescription);
}

//--------------------------------------------------------------------------
int CSelectStyleDlg::GetSelectedStyle()
{
	return GetStyleIdForRow(ui.styleList->currentRow());
}

//--------------------------------------------------------------------------
void CSelectStyleDlg::AddStyle(const QString& styleName, int styleId)
{
	QListWidgetItem		*pItem = new (std::nothrow) QListWidgetItem(styleName);

	if (pItem != NULL)
	{
		pItem->setData(Qt::UserRole, QVariant(styleId));
		ui.styleList->addItem(pItem);
	}
}

//--------------------------------------------------------------------------
int CSelectStyleDlg::GetStyleIdForRow(int row)
{
	QListWidgetItem		*pItem = ui.styleList->item(row);

	QVariant			itemVar = pItem->data(Qt::UserRole);

	return itemVar.toInt();
}

//--------------------------------------------------------------------------
QString CSelectStyleDlg::GetStyleNameForRow(int row)
{
	QListWidgetItem		*pItem = ui.styleList->item(row);

	if (pItem != NULL)
		return pItem->text();
	else
		return QString();
}

//--------------------------------------------------------------------------
void CSelectStyleDlg::on_newButton_clicked()
{
	StyleDef	styleDef;

	styleDef.SetAllFormatFlags();
	styleDef.strFontFamily = QApplication::font().family();

	CStyleDlg	dlg(styleDef, this);

	dlg.exec();

	if (dlg.result() == QDialog::Accepted)
	{
		styleDef = dlg.GetStyle();

        int		styleId = m_styleManager->AddStyle(styleDef);

		AddStyle(styleDef.strName, styleId);
	}
}

//--------------------------------------------------------------------------
void CSelectStyleDlg::on_deleteButton_clicked()
{
	int		curRow = ui.styleList->currentRow();

	if (curRow != -1)
	{
		int		styleId = GetStyleIdForRow(curRow);

		QMessageBox::StandardButton	response;

		response = QMessageBox::question(this,
										tr("Delete Style"),
										QString(tr("Delete style: %1?")).arg(GetStyleNameForRow(curRow)),
										QMessageBox::Yes | QMessageBox::No);
		if (response == QMessageBox::Yes)
		{
			// Delete the style
            m_styleManager->DeleteStyle(styleId);
			ui.styleList->takeItem(curRow);
		}
	}
}

//--------------------------------------------------------------------------
void CSelectStyleDlg::on_editButton_clicked()
{
	int		curRow = ui.styleList->currentRow();

	if (curRow != -1)
	{
		int		styleId = GetStyleIdForRow(curRow);

        StyleDef	styleDef = m_styleManager->GetStyle(styleId);

		CStyleDlg	dlg(styleDef, this);

		dlg.exec();

		if (dlg.result() == QDialog::Accepted)
		{
			styleDef = dlg.GetStyle();

            m_styleManager->SetStyle(styleDef, styleId);
		}
	}
}
