#pragma once

#include <QString>
#include <QVariant>


typedef QMap<QString, QVariant>	PREF_LIST;

class CPreferences
{
public:
	CPreferences(void);
	~CPreferences(void);

	enum {
			eStartupDoNothing = 0,
			eStartupLoadPreviousNoteBook
	};

	void SetAppDefaults(void);

	int GetIntPref(const QString& prefName);
	void SetIntPref(const QString& prefName, int value);

	bool GetBoolPref(const QString& prefName);
	void SetBoolPref(const QString& prefName, bool value);

	QString GetStringPref(const QString& prefName);
	void SetStringPref(const QString& prefName, const QString& prefValue);

private:
	PREF_LIST		m_prefs;
};
