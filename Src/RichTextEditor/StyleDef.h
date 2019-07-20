
#ifndef StyleDef_h__
#define StyleDef_h__

#include <QString>
#include <QColor>
#include <QList>

class StyleDef  
{
public:
	enum FormatFlag
	{
		NoFormat = 0x0,
		FontFamily = 0x1,
		FontSize = 0x2,
		FGColorNone = 0x4,
		FGColor = 0x8,
		BGColorNone = 0x10,
		BGColor = 0x20,
		Bold = 0x40,
		Italic = 0x80,
		Underline = 0x100,
		Strikeout = 0x0200
	};
	Q_DECLARE_FLAGS(FormatFlags, FormatFlag)

	StyleDef();
	StyleDef(	QString name,
				QString description = "",
				FormatFlags flags = NoFormat,
				QString fontFamily = "Helvetica",
				int pointSize = -1,
				QColor fgColor = QColor(Qt::black),
				QColor bgColor = QColor(Qt::white),
				bool bold = false,
				bool italic = false,
				bool underline = false,
				bool strikeout = false);

	/*
	 *	The formatFlags indicate which elments are specified by the style.
	 *	For example, if the Bold flag is set, that indicates that this
	 *	style dictates how bold is to be set (either on or off).  The
	 *	actual setting of whether bold is on or off is determined by
	 *	the bIsBold member.
	 */
	FormatFlags	formatFlags;

	QString		strName;
	QString		strDescription;
	QString		strFontFamily;
	int			fontPointSize;		// -1 indicates don't change size
	QColor		textColor;
	QColor		backgroundColor;
	bool		bIsBold;
	bool		bIsItalic;
	bool		bIsUnderline;
	bool		bIsStrikeout;

	void SetAllFormatFlags();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(StyleDef::FormatFlags)

typedef QHash<int, StyleDef>	StyleDefList;

#endif // StyleDef_h__
