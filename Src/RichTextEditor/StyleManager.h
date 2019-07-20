#pragma once

#include "StyleDef.h"
#include <QTextEdit>
#include <QDomElement>
#include <QDomDocument>


// Built-in style enums
enum {
	eStyleNormal	= 0,			// "Normal" (ie default) style
	eStyleCode						// Style used for code (monospaced)
};

// Index of first user-defined style item
#define kUserStyleStartIndex	1


class CStyleManager
{
public:
	CStyleManager(void);
	~CStyleManager(void);

	bool LoadStyleDefs(QString filePath);
	bool SaveStyleDefs(QString filePath);

	/*
	 *	Adds a new style.
	 *	@return Returns its index.
	 */
	int AddStyle(StyleDef& newStyle);

	/*
	 *	Sets the style for the specified style ID.
	 */
	void SetStyle(StyleDef& style, int styleId);

	/*
	 *	Deletes the specified style ID.
	 */
	void DeleteStyle(int styleId);

	void ApplyStyle(QTextEdit* pTextEdit, int styleNum);

	int NumStyles()		{ return m_styles.size(); }

	/*
	 *	Returns a list of the style IDs.
	 */
	QList<int> GetStyleIds();

	/*
	 *	Returns true if the given style ID is a valid one.
	 */
	bool ValidStyleId(int styleId)	{ return m_styles.contains(styleId); }

	/*
	 *	Returns the specified style.
	 */
	StyleDef GetStyle(int styleNum);

private:
	StyleDefList		m_styles;

	void InitDefaultStyles();

	bool ParseStyleItem(const QDomElement& styleElement,
						const QString& tagName,
						QString& attrValue);

	bool ParseBoolStyleItem(const QDomElement& styleElement,
							const QString& tagName,
							bool& boolValue);

	bool ParseIntStyleItem(	const QDomElement& styleElement,
							const QString& tagName,
							int& intValue);

	bool ParseColorStyleItem(	const QDomElement& styleElement,
								const QString& tagName,
								QColor& colorValue);

	void ParseStyle(StyleDef& curStyle,  int& styleId, const QDomElement& styleElement);

	void CreateStyleItem(	const QString& styleItemName,
							bool bIsSpecified,
							const QString& strValue,
							QDomDocument& doc,
							QDomElement& styleElt);

	void CreateStyleItem(	const QString& styleItemName,
							bool bIsSpecified,
							bool bValue,
							QDomDocument& doc,
							QDomElement& styleElt);
};
