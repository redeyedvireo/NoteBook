#include "stdafx.h"
#include "Preferences.h"

//--------------------------------------------------------------------------
CPreferences::CPreferences(void)
{
}

//--------------------------------------------------------------------------
CPreferences::~CPreferences(void)
{
}

//--------------------------------------------------------------------------
void CPreferences::SetAppDefaults( void )
{
	SetIntPref("general-startupload", eStartupLoadPreviousNoteBook);
	SetIntPref("editor-defaultfontsize", 10);
}

//--------------------------------------------------------------------------
int CPreferences::GetIntPref( const QString& prefName )
{
	int		retVal = 0;
	bool	bOk;

	if (m_prefs.contains(prefName))
	{
		retVal = m_prefs.value(prefName).toInt(&bOk);
		Q_ASSERT(bOk);
	}

	return retVal;
}

//--------------------------------------------------------------------------
void CPreferences::SetIntPref( const QString& prefName, int value )
{
	if (m_prefs.contains(prefName))
	{
		m_prefs[prefName] = value;
	}
	else
	{
		m_prefs.insert(prefName, QVariant(value));
	}
}

//--------------------------------------------------------------------------
bool CPreferences::GetBoolPref( const QString& prefName )
{
	bool	retVal = false;

	if (m_prefs.contains(prefName))
	{
		retVal = m_prefs.value(prefName).toBool();
	}

	return retVal;
}

//--------------------------------------------------------------------------
void CPreferences::SetBoolPref( const QString& prefName, bool value )
{
	if (m_prefs.contains(prefName))
	{
		m_prefs[prefName] = value;
	}
	else
	{
		m_prefs.insert(prefName, QVariant(value));
	}
}

//--------------------------------------------------------------------------
QString CPreferences::GetStringPref( const QString& prefName )
{
	QString		retVal;

	if (m_prefs.contains(prefName))
	{
		retVal = m_prefs.value(prefName).toString();
	}

	return retVal;
}

//--------------------------------------------------------------------------
void CPreferences::SetStringPref( const QString& prefName, const QString& prefValue )
{
	if (m_prefs.contains(prefName))
	{
		m_prefs[prefName] = prefValue;
	}
	else
	{
		m_prefs.insert(prefName, QVariant(prefValue));
	}
}
