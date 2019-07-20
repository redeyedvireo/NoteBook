#include "stdafx.h"
#include "ExistingDbEntryDialog.h"


CExistingDbEntryDialog::CExistingDbEntryDialog(const QString& existingTitle, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.dateLabel->setText(QString("An entry already exists with the title: %1").arg(existingTitle));
}

CExistingDbEntryDialog::~CExistingDbEntryDialog()
{

}

//--------------------------------------------------------------------------
DecisionType CExistingDbEntryDialog::ChosenDecision()
{
	if (ui.appendButton->isChecked())
		return kAppendEntry;
	else if (ui.overwriteButton->isChecked())
		return kOverwriteEntry;
	else
		return kSkipEntry;
}

//--------------------------------------------------------------------------
bool CExistingDbEntryDialog::DoThisAlways()
{
	return ui.doAlwaysCheckBox->isChecked();
}
