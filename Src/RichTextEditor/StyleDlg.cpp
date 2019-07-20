#include "stdafx.h"
#include "StyleDlg.h"
#include <QFontDialog>

#define kStyleResultString		tr("Style looks like this")


//--------------------------------------------------------------------------
CStyleDlg::CStyleDlg(const StyleDef& inStyleDef, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	PopulateDialog(inStyleDef);
	UpdateFontLabel();
	UpdateResultLabel();
	UpdateOkButton();

	connect(ui.styleNameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(UpdateOkButton()));
	connect(ui.bgColorToolButton, SIGNAL(ColorChanged(const QColor&)), this, SLOT(OnColorChanged()));
	connect(ui.fgColorToolButton, SIGNAL(ColorChanged(const QColor&)), this, SLOT(OnColorChanged()));
	connect(ui.bgColorToolButton, SIGNAL(NoColor()), this, SLOT(OnColorChanged()));
	connect(ui.fgColorToolButton, SIGNAL(NoColor()), this, SLOT(OnColorChanged()));
}

//--------------------------------------------------------------------------
CStyleDlg::~CStyleDlg()
{

}

//--------------------------------------------------------------------------
StyleDef CStyleDlg::GetStyle()
{
	StyleDef::FormatFlags	formatFlags;
	QColor					fgColor, bgColor;

	/*
	 *	For now, the style dialog is a simplified dialog, where every style item
	 *	will be specified.  Later on, an "Advanced" button will be added that will
	 *	(probably through a stacked widget) reveal additional (or a new set of)
	 *	controls that will allow the user to specify which style items are set
	 *	by this style.  (For example, a style could be created that only specifies
	 *	a font family and size.  In this case other font items would be left alone
	 *	when applying the style.)
	 */

	// Set format flags
	formatFlags |= StyleDef::FontFamily | StyleDef::FontSize | StyleDef::Bold
				| StyleDef::Italic | StyleDef::Underline | StyleDef::Strikeout;

	if (ui.fgColorToolButton->HasColor())
	{
		formatFlags |= StyleDef::FGColor;
		fgColor = ui.fgColorToolButton->GetColor();
	}
	else
	{
		formatFlags |= StyleDef::FGColorNone;
	}

	if (ui.bgColorToolButton->HasColor())
	{
		formatFlags |= StyleDef::BGColor;
		bgColor = ui.bgColorToolButton->GetColor();
	}
	else
	{
		formatFlags |= StyleDef::BGColorNone;
	}

	StyleDef	theStyle(	ui.styleNameEdit->text(),
							ui.descriptionEdit->text(),
							formatFlags,
							m_currentFont.family(),
							m_currentFont.pointSize(),
							fgColor,
							bgColor,
							m_currentFont.bold(),
							m_currentFont.italic(),
							m_currentFont.underline(),
							m_currentFont.strikeOut());

	return theStyle;
}

//--------------------------------------------------------------------------
void CStyleDlg::PopulateDialog(const StyleDef& inStyleDef)
{
	ui.styleNameEdit->setText(inStyleDef.strName);
	ui.descriptionEdit->setText(inStyleDef.strDescription);

	if (inStyleDef.formatFlags != StyleDef::NoFormat)
	{
		if (inStyleDef.formatFlags.testFlag(StyleDef::FontFamily))
		{
			m_currentFont.setFamily(inStyleDef.strFontFamily);
		}

		if (inStyleDef.formatFlags.testFlag(StyleDef::FontSize))
		{
			m_currentFont.setPointSize(inStyleDef.fontPointSize);
		}

		if (inStyleDef.formatFlags.testFlag(StyleDef::FGColorNone))
		{
			ui.fgColorToolButton->SetNoColor();
		}

		if (inStyleDef.formatFlags.testFlag(StyleDef::FGColor))
		{
			ui.fgColorToolButton->SetColor(inStyleDef.textColor);
		}

		if (inStyleDef.formatFlags.testFlag(StyleDef::BGColorNone))
		{
			ui.bgColorToolButton->SetNoColor();
		}

		if (inStyleDef.formatFlags.testFlag(StyleDef::BGColor))
		{
			ui.bgColorToolButton->SetColor(inStyleDef.backgroundColor);
		}

		if (inStyleDef.formatFlags.testFlag(StyleDef::Bold))
		{
			m_currentFont.setBold(inStyleDef.bIsBold);
		}

		if (inStyleDef.formatFlags.testFlag(StyleDef::Italic))
		{
			m_currentFont.setItalic(inStyleDef.bIsItalic);
		}

		if (inStyleDef.formatFlags.testFlag(StyleDef::Underline))
		{
			m_currentFont.setUnderline(inStyleDef.bIsUnderline);
		}

		if (inStyleDef.formatFlags.testFlag(StyleDef::Strikeout))
		{
			m_currentFont.setStrikeOut(inStyleDef.bIsStrikeout);
		}
	}
}

//--------------------------------------------------------------------------
QString CStyleDlg::OnOrOffToString(const QString& styleItem, bool bOn)
{
	return bOn ? styleItem : QString("no %1").arg(styleItem);
}

//--------------------------------------------------------------------------
void CStyleDlg::on_fontButton_clicked()
{
	QFont	selectedFont;
	bool	bOkButtonPressed = false;

	selectedFont = QFontDialog::getFont(&bOkButtonPressed, m_currentFont, this, tr("Select Font"));

	if (bOkButtonPressed)
	{
		m_currentFont = selectedFont;

		UpdateFontLabel();
		UpdateResultLabel();
	}
}

//--------------------------------------------------------------------------
void CStyleDlg::UpdateFontLabel()
{
	QStringList	styleItemList;
	QString		styleDescription;

	styleItemList << m_currentFont.family();
	styleItemList << QString("%1 pt.").arg(QString::number(m_currentFont.pointSize()));

	if (m_currentFont.bold())
	{
		styleItemList << OnOrOffToString(tr("bold"), m_currentFont.bold());
	}

	if (m_currentFont.italic())
	{
		styleItemList << OnOrOffToString(tr("italic"), m_currentFont.italic());
	}

	if (m_currentFont.underline())
	{
		styleItemList << OnOrOffToString(tr("underline"), m_currentFont.underline());
	}

	if (m_currentFont.strikeOut())
	{
		styleItemList << OnOrOffToString(tr("strikeout"), m_currentFont.strikeOut());
	}

	styleDescription = styleItemList.join(", ");

	ui.fontLabel->setText(styleDescription);
}

//--------------------------------------------------------------------------
void CStyleDlg::UpdateResultLabel()
{
	QStringList	styleElementList;
	QString		styleSheetStr;
	QString		fontStr;

	if (m_currentFont.bold())
		fontStr.append(" bold");

	if (m_currentFont.italic())
		fontStr.append(" italic");

	if (m_currentFont.underline())
		fontStr.append(" underline");

	if (m_currentFont.strikeOut())
		fontStr.append(" strikeout");

	if (ui.bgColorToolButton->HasColor())
	{
		styleElementList.append(QString("background-color: %1").arg(ui.bgColorToolButton->GetColor().name()));
	}

	if (ui.fgColorToolButton->HasColor())
	{
		styleElementList.append(QString("color: %1").arg(ui.fgColorToolButton->GetColor().name()));
	}

	styleElementList.append(QString("font: %1 \"%2\"").arg(fontStr).arg(m_currentFont.family()));
	styleElementList.append(QString("font-size: %1px").arg(m_currentFont.pointSize()));

	styleSheetStr = styleElementList.join("; ");
	styleSheetStr.append(";");

	ui.sampleLabel->setStyleSheet(styleSheetStr);

	ui.sampleLabel->setText(kStyleResultString);
}

//--------------------------------------------------------------------------
void CStyleDlg::OnColorChanged()
{
	UpdateResultLabel();
}

//--------------------------------------------------------------------------
void CStyleDlg::UpdateOkButton()
{
	ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(! ui.styleNameEdit->text().isEmpty());
}
