#include "stdafx.h"
#include "Encrypter.h"
#include "openssl/blowfish.h"



//--------------------------------------------------------------------------
CEncrypter::CEncrypter(void)
{
	m_password.clear();
	m_hashedPassword.clear();
}

//--------------------------------------------------------------------------
CEncrypter::~CEncrypter(void)
{
}

//--------------------------------------------------------------------------
void CEncrypter::SetPassword( const QString& pw )
{
	m_password = pw;

	// Hash the password
	QCryptographicHash	hashObj(QCryptographicHash::Sha1);
	hashObj.addData(m_password.toUtf8());
	m_hashedPassword = hashObj.result();
}

//--------------------------------------------------------------------------
void CEncrypter::ClearPassword()
{
	m_password.clear();
}

//--------------------------------------------------------------------------
bool CEncrypter::HasPassword()
{
	return (! m_password.isEmpty());
}

//--------------------------------------------------------------------------
QByteArray CEncrypter::HashedPassword()
{
	return m_hashedPassword;
}

//--------------------------------------------------------------------------
QByteArray CEncrypter::EncryptString( const QString& inString )
{
	QByteArray		sourceData = inString.toUtf8();
	QByteArray		destData;
	unsigned char	ivec[8];
	BF_KEY			key;
	int				turn = 0;

	// Set up the key
	BF_set_key(&key, m_password.length(), (const unsigned char*) m_password.toUtf8().constData());

	memset(ivec, 0, 8);

	// I'm assuming the output will be the same size as the input
	// though I don't know this for sure.
	destData.resize(sourceData.size());

	BF_cfb64_encrypt(	(const unsigned char*) sourceData.constData(),
						(unsigned char*) destData.data(),
						sourceData.size(),
						&key,
						ivec,
						&turn,
						BF_ENCRYPT);

	return destData;
}

//--------------------------------------------------------------------------
QString CEncrypter::DecryptString( const QByteArray& inByteArray )
{
	QByteArray		destData;
	unsigned char	ivec[8];
	BF_KEY			key;
	int				turn = 0;

	// Set up the key
	BF_set_key(&key, m_password.length(), (const unsigned char*) m_password.toUtf8().constData());

	memset(ivec, 0, 8);

	// I'm assuming the output will be the same size as the input
	// though I don't know this for sure.
	destData.resize(inByteArray.size());

	BF_cfb64_encrypt(	(const unsigned char*) inByteArray.constData(),
		(unsigned char*) destData.data(),
		inByteArray.size(),
		&key,
		ivec,
		&turn,
		BF_DECRYPT);

	// Convert destData to a string
	QString			unencryptedString(destData);

	return unencryptedString;
}

//--------------------------------------------------------------------------
QByteArray CEncrypter::HashPassword( const QString& inString )
{
	// Hash the password
	QCryptographicHash	hashObj(QCryptographicHash::Sha1);
	hashObj.addData(inString.toUtf8());
	return hashObj.result();
}
