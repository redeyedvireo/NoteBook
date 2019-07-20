#include "stdafx.h"
#include "SetPassword.h"
#include <QMessageBox>


//--------------------------------------------------------------------------
CSetPassword::CSetPassword(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.noPasswordButton, SIGNAL(clicked()), this, SLOT(OnNoPasswordClicked()));
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(OnOkClicked()));
}

//--------------------------------------------------------------------------
CSetPassword::~CSetPassword()
{

}

//--------------------------------------------------------------------------
bool CSetPassword::PasswordExists()
{
	return (ui.passwordEdit->text().length() > 0);
}

//--------------------------------------------------------------------------
QString CSetPassword::GetPassword()
{
	return ui.passwordEdit->text();
}
//--------------------------------------------------------------------------
void CSetPassword::OnNoPasswordClicked()
{
	ui.passwordEdit->clear();
	ui.reEnterPasswordEdit->clear();
	accept();
}

//--------------------------------------------------------------------------
void CSetPassword::OnOkClicked()
{
	// Verify that the two passwords match
	if (ui.passwordEdit->text() == ui.reEnterPasswordEdit->text())
	{
		accept();
		return;
	}

	QMessageBox::critical(this, "Password Mismatch", "The passwords don't match.");
}
