#include "stdafx.h"
#include "StyleDef.h"


StyleDef::StyleDef()
{
	strName = "";
	strDescription = "";
	strFontFamily = "";
	fontPointSize = -1;
	textColor = QColor(Qt::black);
	backgroundColor = QColor(Qt::white);
	bIsBold = false;
	bIsItalic = false;
	bIsUnderline = false;

	formatFlags = NoFormat;						// Don't set format
}

StyleDef::StyleDef(	QString name,
					QString description,
					FormatFlags flags,
					QString fontFamily,
					int pointSize,
					QColor fgColor,
					QColor bgColor,
					bool bold,
					bool italic,
					bool underline,
					bool strikeout)
{
	strName = name;
	strDescription = description;
	strFontFamily = fontFamily;
	fontPointSize = pointSize;
	textColor = fgColor;
	backgroundColor = bgColor;
	bIsBold = bold;
	bIsItalic = italic;
	bIsUnderline = underline;
	bIsStrikeout = strikeout;

	formatFlags = flags;
}

//--------------------------------------------------------------------------
void StyleDef::SetAllFormatFlags()
{
	formatFlags |= StyleDef::FontFamily | StyleDef::FontSize | StyleDef::Bold
					| StyleDef::Italic | StyleDef::Underline | StyleDef::Strikeout
					| StyleDef::FGColorNone | StyleDef::BGColorNone;
}
