#pragma once

#include <QString>
#include <QByteArray>
#include <QCryptographicHash>

class CEncrypter
{
public:
	CEncrypter(void);
	~CEncrypter(void);

	void SetPassword(const QString& pw);
	void ClearPassword();

	bool HasPassword();

	/*
	 *	Returns a hashed version of the password.
	 */
	QByteArray	HashedPassword();

	QByteArray	EncryptString(const QString& inString);
	QString		DecryptString(const QByteArray& inByteArray);

	/*
	 *	Static function to hash a password.
	 */
	static QByteArray HashPassword(const QString& inString);

private:
	QString				m_password;
	QByteArray			m_hashedPassword;
};
