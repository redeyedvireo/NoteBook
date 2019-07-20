#ifndef EXISTINGDBENTRYDIALOG_H
#define EXISTINGDBENTRYDIALOG_H

#include <QDialog>
#include "ui_ExistingDbEntryDialog.h"
#include "PageData.h"


class CExistingDbEntryDialog : public QDialog
{
	Q_OBJECT

public:
	CExistingDbEntryDialog(const QString& existingTitle, QWidget *parent = 0);
	~CExistingDbEntryDialog();

	DecisionType ChosenDecision();
	bool DoThisAlways();

private:
	Ui::ExistingDbEntryDialog ui;
};

#endif // EXISTINGDBENTRYDIALOG_H
