#ifndef SETPASSWORD_H
#define SETPASSWORD_H

#include <QDialog>
#include "ui_SetPassword.h"

class CSetPassword : public QDialog
{
	Q_OBJECT

public:
	CSetPassword(QWidget *parent = 0);
	~CSetPassword();

	bool PasswordExists();
	QString GetPassword();

public slots:
	void OnNoPasswordClicked();
	void OnOkClicked();

private:
	Ui::SetPasswordDlg		ui;
};

#endif // SETPASSWORD_H
