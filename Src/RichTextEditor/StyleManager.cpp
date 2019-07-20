#include "stdafx.h"
#include "StyleManager.h"
#include <QFile>
#include <QTextStream>
#include <QDomElement>
#include <QDomNodeList>


/************************************************************************/
/* XML Node and Attribute Name Strings                                  */
/************************************************************************/

#define kXmlDocumentType	"StyleTypeDoc"

#define STYLE_DOC_NODE		"StyleDoc"
#define STYLE_DEF_NODE		"Style"
#define NAME_ATTR			"name"
#define DESCR_ATTR			"description"
#define ID_ATTR				"id"
#define IS_SPECIFIED_ATTR	"specified"
#define VALUE_ATTR			"value"

#define FONT_FAMILY_TAG		"fontfamily"
#define POINT_SIZE_TAG		"pointsize"
#define FG_COLOR_NONE_TAG	"fgcolornone"
#define FG_COLOR_TAG		"fgcolor"
#define BG_COLOR_NONE_TAG	"bgcolornone"
#define BG_COLOR_TAG		"bgcolor"
#define BOLD_TAG			"bold"
#define ITALIC_TAG			"italic"
#define UNDERLINE_TAG		"underline"
#define STRIKEOUT_TAG		"strikeout"

#define YES_VAL				"yes"
#define NO_VAL				"no"


//--------------------------------------------------------------------------
CStyleManager::CStyleManager(void)
{
	InitDefaultStyles();
}

//--------------------------------------------------------------------------
CStyleManager::~CStyleManager(void)
{
}

//--------------------------------------------------------------------------
void CStyleManager::InitDefaultStyles()
{
	m_styles.clear();
}

//--------------------------------------------------------------------------
void CStyleManager::ApplyStyle(QTextEdit* pTextEdit, int styleNum)
{
	if (! ValidStyleId(styleNum))
		return;

	if (pTextEdit == NULL)
		return;

	QTextCursor			selectionCursor(pTextEdit->textCursor());

	if (! selectionCursor.hasSelection())
		return;

	QTextCharFormat		selectionFormat;
	StyleDef			styleToApply = m_styles.value(styleNum);

	if (styleToApply.formatFlags != StyleDef::NoFormat)
	{
		// Apply font
		selectionFormat = selectionCursor.charFormat();

		if (styleToApply.formatFlags.testFlag(StyleDef::FontFamily))
		{
			selectionFormat.setFontFamily(styleToApply.strFontFamily);
		}

		if (styleToApply.formatFlags.testFlag(StyleDef::FontSize))
		{
			selectionFormat.setFontPointSize((qreal) styleToApply.fontPointSize);
		}

		if (styleToApply.formatFlags.testFlag(StyleDef::FGColorNone))
		{
			selectionFormat.clearForeground();
		}

		if (styleToApply.formatFlags.testFlag(StyleDef::FGColor))
		{
			selectionFormat.setForeground(QBrush(styleToApply.textColor));
		}

		if (styleToApply.formatFlags.testFlag(StyleDef::BGColorNone))
		{
			selectionFormat.clearBackground();
		}

		if (styleToApply.formatFlags.testFlag(StyleDef::BGColor))
		{
			selectionFormat.setBackground(QBrush(styleToApply.backgroundColor));
		}

		if (styleToApply.formatFlags.testFlag(StyleDef::Bold))
		{
			if (styleToApply.bIsBold)
				selectionFormat.setFontWeight(QFont::Bold);
			else
				selectionFormat.setFontWeight(QFont::Normal);
		}

		if (styleToApply.formatFlags.testFlag(StyleDef::Italic))
		{
			selectionFormat.setFontItalic(styleToApply.bIsItalic);
		}

		if (styleToApply.formatFlags.testFlag(StyleDef::Underline))
		{
			selectionFormat.setFontUnderline(styleToApply.bIsUnderline);
		}

		if (styleToApply.formatFlags.testFlag(StyleDef::Strikeout))
		{
			selectionFormat.setFontStrikeOut(styleToApply.bIsStrikeout);
		}

		// Apply the cursor to the document
		selectionCursor.setCharFormat(selectionFormat);
		pTextEdit->setTextCursor(selectionCursor);
	}
}

//--------------------------------------------------------------------------
QList<int> CStyleManager::GetStyleIds()
{
	return m_styles.keys();
}

//--------------------------------------------------------------------------
StyleDef CStyleManager::GetStyle( int styleNum )
{
	if (m_styles.contains(styleNum))
	{
		return m_styles.value(styleNum);
	}
	else
	{
		return StyleDef();
	}
}

//--------------------------------------------------------------------------
bool CStyleManager::ParseStyleItem(const QDomElement& styleElement, const QString& tagName, QString& attrValue)
{
	bool			bFound = false;
	QDomNodeList	eltList;

	eltList = styleElement.elementsByTagName(tagName);

	if (eltList.size() > 0)
	{
		// There should only be one such element
		QDomElement  styleElement = eltList.at(0).toElement();

		if (! styleElement.isNull())
		{
			bFound = true;

			if (styleElement.hasAttribute(VALUE_ATTR))
			{
				attrValue = styleElement.attribute(VALUE_ATTR);
			}
		}
	}

	return bFound;
}

//--------------------------------------------------------------------------
bool CStyleManager::ParseBoolStyleItem(const QDomElement& styleElement, const QString& tagName, bool& boolValue)
{
	bool	bReturn = false;
	QString	attrValue;

	if (ParseStyleItem(styleElement, tagName, attrValue))
	{
		if (attrValue.compare(YES_VAL, Qt::CaseInsensitive) == 0)
			boolValue = true;
		else
			boolValue = false;

		bReturn = true;
	}

	return bReturn;
}

//--------------------------------------------------------------------------
bool CStyleManager::ParseIntStyleItem(const QDomElement& styleElement, const QString& tagName, int& intValue)
{
	bool	bReturn = false;
	QString	attrValue;

	if (ParseStyleItem(styleElement, tagName, attrValue))
	{
		intValue = attrValue.toInt(&bReturn);
	}

	return bReturn;
}

//--------------------------------------------------------------------------
bool CStyleManager::ParseColorStyleItem(const QDomElement& styleElement, const QString& tagName, QColor& colorValue)
{
	bool	bReturn = false;
	QString	attrValue;

	if (ParseStyleItem(styleElement, tagName, attrValue))
	{
		colorValue.setNamedColor(attrValue);
		bReturn = colorValue.isValid();
	}

	return bReturn;
}

//--------------------------------------------------------------------------
void CStyleManager::ParseStyle(StyleDef& curStyle, int& styleId, const QDomElement& styleElement)
{
	curStyle.strName = styleElement.attribute(NAME_ATTR);
	curStyle.strDescription = styleElement.attribute(DESCR_ATTR);

	QString		idStr;
	idStr = styleElement.attribute(ID_ATTR);
	styleId = idStr.toInt();

	bool		tempBool;

	if (ParseStyleItem(styleElement, FONT_FAMILY_TAG, curStyle.strFontFamily))
		curStyle.formatFlags |= StyleDef::FontFamily;

	if (ParseIntStyleItem(styleElement, POINT_SIZE_TAG, curStyle.fontPointSize))
		curStyle.formatFlags |= StyleDef::FontSize;

	if (ParseBoolStyleItem(styleElement, FG_COLOR_NONE_TAG, tempBool))
		curStyle.formatFlags |= StyleDef::FGColorNone;

	if (ParseColorStyleItem(styleElement, FG_COLOR_TAG, curStyle.textColor))
		curStyle.formatFlags |= StyleDef::FGColor;

	if (ParseBoolStyleItem(styleElement, BG_COLOR_NONE_TAG, tempBool))
		curStyle.formatFlags |= StyleDef::BGColorNone;

	if (ParseColorStyleItem(styleElement, BG_COLOR_TAG, curStyle.backgroundColor))
		curStyle.formatFlags |= StyleDef::BGColor;

	if (ParseBoolStyleItem(styleElement, BOLD_TAG, curStyle.bIsBold))
		curStyle.formatFlags |= StyleDef::Bold;

	if (ParseBoolStyleItem(styleElement, ITALIC_TAG, curStyle.bIsItalic))
		curStyle.formatFlags |= StyleDef::Italic;

	if (ParseBoolStyleItem(styleElement, UNDERLINE_TAG, curStyle.bIsUnderline))
		curStyle.formatFlags |= StyleDef::Underline;

	if (ParseBoolStyleItem(styleElement, STRIKEOUT_TAG, curStyle.bIsStrikeout))
		curStyle.formatFlags |= StyleDef::Strikeout;
}

//--------------------------------------------------------------------------
bool CStyleManager::LoadStyleDefs(QString filePath)
{
	QFile			inputFile(filePath);
	QDomDocument	docObj;

	if (! inputFile.open(QIODevice::ReadOnly))
		return false;

	if (! docObj.setContent(&inputFile))
	{
		inputFile.close();
		return false;
	}
	inputFile.close();

	// Get Style nodes
	QDomNodeList styleNodeList = docObj.elementsByTagName(STYLE_DEF_NODE);

	int		numStyles = styleNodeList.size();
	
	for (int i = 0; i < numStyles; i++)
	{
		int			styleId;
		QDomElement styleElement = styleNodeList.at(i).toElement();

		StyleDef		curStyle;
		ParseStyle(curStyle, styleId, styleElement);

		SetStyle(curStyle, styleId);
	}

	return true;
}

//--------------------------------------------------------------------------
void CStyleManager::CreateStyleItem(const QString& styleItemName,
									bool bIsSpecified,
									const QString& strValue,
									QDomDocument& doc,
									QDomElement& styleElt)
{
	QDomElement		elt = doc.createElement(styleItemName);
	styleElt.appendChild(elt);

	if (bIsSpecified)
	{
		// Set the value
		QDomAttr	valueAttr = doc.createAttribute(VALUE_ATTR);
		valueAttr.setValue(strValue);
		elt.setAttributeNode(valueAttr);
	}
}

//--------------------------------------------------------------------------
void CStyleManager::CreateStyleItem(const QString& styleItemName,
									bool bIsSpecified,
									bool bValue,
									QDomDocument& doc,
									QDomElement& styleElt )
{
	QString		valueStr = bValue ? YES_VAL : NO_VAL;

	CreateStyleItem(styleItemName, bIsSpecified, valueStr, doc, styleElt);
}

//--------------------------------------------------------------------------
bool CStyleManager::SaveStyleDefs(QString filePath)
{
	QDomDocument doc(kXmlDocumentType);
	QDomElement root = doc.createElement(STYLE_DOC_NODE);
	doc.appendChild(root);

	QList<int>	styleIds = GetStyleIds();

	foreach (int styleNum, styleIds)
	{
		if (styleNum < kUserStyleStartIndex)
			continue;		// Don't save the preset styles

		StyleDef	styleItem = m_styles.value(styleNum);

		QDomElement styleElt = doc.createElement(STYLE_DEF_NODE);
		root.appendChild(styleElt);

		QDomAttr	nameAttr = doc.createAttribute(NAME_ATTR);
		nameAttr.setValue(styleItem.strName);
		styleElt.setAttributeNode(nameAttr);

		QDomAttr	idAttr = doc.createAttribute(ID_ATTR);
		idAttr.setValue(QString::number(styleNum));
		styleElt.setAttributeNode(idAttr);

		QDomAttr	descriptionAttr = doc.createAttribute(DESCR_ATTR);
		descriptionAttr.setValue(styleItem.strDescription);
		styleElt.setAttributeNode(descriptionAttr);

		// Save each font attribute
		CreateStyleItem(FONT_FAMILY_TAG,
						styleItem.formatFlags.testFlag(StyleDef::FontFamily),
						styleItem.strFontFamily,
						doc,
						styleElt);

		CreateStyleItem(POINT_SIZE_TAG,
						styleItem.formatFlags.testFlag(StyleDef::FontSize),
						QString::number(styleItem.fontPointSize),
						doc,
						styleElt);


		CreateStyleItem(FG_COLOR_NONE_TAG,
						styleItem.formatFlags.testFlag(StyleDef::FGColorNone),
						YES_VAL,
						doc,
						styleElt);

		CreateStyleItem(FG_COLOR_TAG,
						styleItem.formatFlags.testFlag(StyleDef::FGColor),
						styleItem.textColor.name(),
						doc,
						styleElt);

		CreateStyleItem(BG_COLOR_NONE_TAG,
						styleItem.formatFlags.testFlag(StyleDef::BGColorNone),
						YES_VAL,
						doc,
						styleElt);

		CreateStyleItem(BG_COLOR_TAG,
						styleItem.formatFlags.testFlag(StyleDef::BGColor),
						styleItem.backgroundColor.name(),
						doc,
						styleElt);

		CreateStyleItem(BOLD_TAG,
						styleItem.formatFlags.testFlag(StyleDef::Bold),
						styleItem.bIsBold,
						doc,
						styleElt);

		CreateStyleItem(ITALIC_TAG,
						styleItem.formatFlags.testFlag(StyleDef::Italic),
						styleItem.bIsItalic,
						doc,
						styleElt);

		CreateStyleItem(UNDERLINE_TAG,
						styleItem.formatFlags.testFlag(StyleDef::Underline),
						styleItem.bIsUnderline,
						doc,
						styleElt);

		CreateStyleItem(STRIKEOUT_TAG,
						styleItem.formatFlags.testFlag(StyleDef::Strikeout),
						styleItem.bIsStrikeout,
						doc,
						styleElt);
	}

	QFile	outputFile(filePath);

	if (outputFile.open(QIODevice::WriteOnly))
	{

		QTextStream out(&outputFile);
		doc.save(out, 4);		// Indent of 4

		outputFile.close();

		return true;
	}
	else
	{
		return false;
	}
}

//--------------------------------------------------------------------------
int CStyleManager::AddStyle( StyleDef& newStyle )
{
	// Find an unused ID
	QList<int>	allIds = m_styles.keys();

	qSort(allIds);

    int		newStyleId = kUserStyleStartIndex;     // Starting ID when list is empty

    if (!allIds.isEmpty())
    {
        newStyleId = allIds.last() + 1;
    }

	m_styles.insert(newStyleId, newStyle);

	return newStyleId;
}

//--------------------------------------------------------------------------
void CStyleManager::SetStyle( StyleDef& style, int styleId )
{
	if (ValidStyleId(styleId))
	{
		m_styles.remove(styleId);
	}

	m_styles.insert(styleId, style);
}

//--------------------------------------------------------------------------
void CStyleManager::DeleteStyle( int styleId )
{
	if (ValidStyleId(styleId))
	{
		m_styles.remove(styleId);
	}
}
