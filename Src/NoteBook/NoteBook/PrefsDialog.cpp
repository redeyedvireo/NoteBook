#include "stdafx.h"
#include "PrefsDialog.h"
#include "CAbstractPageInterface.h"


CPrefsDialog::CPrefsDialog(CPreferences& prefs, QList<CAbstractPageInterface *> pageInterfaceList, QWidget *parent)
	: m_curPrefs(prefs), QDialog(parent)
{
	ui.setupUi(this);

	connect(this, SIGNAL(accepted()), this, SLOT(OnAccept()));

    ui.listWidget->setCurrentRow(0);

	// Set initial dialog values from m_curPrefs
	switch (m_curPrefs.GetIntPref("general-startupload"))
	{
	case CPreferences::eStartupDoNothing:
		ui.emptyWorkspaceRadio->setChecked(true);
		break;
	case CPreferences::eStartupLoadPreviousNoteBook:
		ui.loadPreviousNotebookRadio->setChecked(true);
		break;
	}

    // Construct settings widgets
    foreach (CAbstractPageInterface* pPageInterface, pageInterfaceList)
    {
        QWidget *pSettingsWidget = pPageInterface->settingsWidget();
        QString settingsGroupName = pPageInterface->settingsGroupName();

        if (pSettingsWidget == NULL || settingsGroupName.isEmpty())
            continue;

        int stackedWidgetIndex = ui.stackedWidget->addWidget(pSettingsWidget);
        ui.listWidget->addItem(settingsGroupName);

        m_pageInterfaceHash.insert(stackedWidgetIndex, pPageInterface);
    }
}

//--------------------------------------------------------------------------
CPrefsDialog::~CPrefsDialog()
{

}

//--------------------------------------------------------------------------
void CPrefsDialog::OnAccept()
{
	int		loadPref;

	if (ui.emptyWorkspaceRadio->isChecked())
	{
		loadPref = CPreferences::eStartupDoNothing;
	}
	else
	{
		loadPref = CPreferences::eStartupLoadPreviousNoteBook;
	}

	m_curPrefs.SetIntPref("general-startupload", loadPref);

    // Traverse through the widgets, and call the associated settingsDialogAccepted function.
    QHashIterator<int, CAbstractPageInterface*>     iter(m_pageInterfaceHash);
    while (iter.hasNext())
    {
        iter.next();

        int index = iter.key();
        CAbstractPageInterface  *pInterface = iter.value();

        QWidget *pWidget = ui.stackedWidget->widget(index);

        pInterface->settingsDialogAccepted(pWidget);
    }
}
