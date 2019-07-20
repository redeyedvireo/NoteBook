#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include <QDialog>
#include "Preferences.h"
#include "ui_PrefsDialog.h"

class CAbstractPageInterface;

class CPrefsDialog : public QDialog
{
	Q_OBJECT

public:
    CPrefsDialog(CPreferences& prefs, QList<CAbstractPageInterface*> pageInterfaceList, QWidget *parent = 0);
	~CPrefsDialog();

public slots:
	void OnAccept();

private:
	Ui::PrefsDialog		ui;

    // Hash that maps indices in the stacked widget to the associated abstract
    // page interface pointers
    QHash<int, CAbstractPageInterface*>      m_pageInterfaceHash;

	CPreferences&		m_curPrefs;		// This is a reference to the prefs passed in
};

#endif // PREFSDIALOG_H
