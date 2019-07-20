#include "RichTextEdit.h"
#include "CTextTable.h"
#include "SelectStyleDlg.h"
#include "TableFormatDlg.h"
#include "CRichTextEditorSettingsWidget.h"
#include "AbstractPluginServices.h"

#include <QTextList>
#include <QDebug>


//--------------------------------------------------------------------------
CRichTextEdit::CRichTextEdit(CStyleManager *styleManager, AbstractPluginServices *pluginServices, QWidget *parent)
    : styleManager(styleManager), pluginServices(pluginServices), QWidget(parent)
{
	ui.setupUi(this);

    ui.textEdit->setPluginServicesPointer(pluginServices);
    ui.textEdit->setStyleManagerPointer(styleManager);

	// Connect signals
	connect(ui.textColorButton, SIGNAL(ColorChanged(const QColor&)), this, SLOT(OnTextColorChanged(const QColor&)));
	connect(ui.textColorButton, SIGNAL(NoColor()), this, SLOT(OnTextColorNoColor()));
	connect(ui.textBackgroundButton, SIGNAL(ColorChanged(const QColor&)), this, SLOT(OnTextBackgroundChanged(const QColor&)));
	connect(ui.textBackgroundButton, SIGNAL(NoColor()), this, SLOT(OnBackgroundNoColor()));
	connect(ui.textEdit, SIGNAL(selectionChanged()), this, SLOT(OnSelectionChanged()));
	connect(ui.textEdit, SIGNAL(textChanged()), this, SLOT(OnTextChanged()));
	connect(ui.searchButton, SIGNAL(clicked(bool)), this, SLOT(OnSearchButtonPressed()));
	connect(ui.searchHideButton, SIGNAL(clicked(bool)), ui.richTextEditSearchWidget, SLOT(hide()));
	connect(ui.searchEdit, SIGNAL(returnPressed()), this, SLOT(OnFirstSearch()));
	connect(ui.nextButton, SIGNAL(clicked(bool)), this, SLOT(OnSearchNext()));
	connect(ui.prevButton, SIGNAL(clicked(bool)), this, SLOT(OnSearchPrev()));

    connect(ui.textEdit, SIGNAL(CTE_GotoPage(ENTITY_ID)), this, SIGNAL(GotoPage(ENTITY_ID)));
    connect(ui.textEdit, SIGNAL(CTE_Message(QString)), this, SIGNAL(Message(QString)));
    connect(ui.textEdit, SIGNAL(CTE_IndentList()), this, SLOT(on_indentRightButton_clicked()));
    connect(ui.textEdit, SIGNAL(CTE_OutdentList()), this, SLOT(on_indentLeftButton_clicked()));
    connect(ui.textEdit, SIGNAL(CTE_TableFormat()), this, SLOT(on_tableButton_clicked()));

    connect(&m_styleSignalMapper, SIGNAL(mapped(int)), ui.textEdit, SLOT(OnStyleTriggered(int)));
    connect(&m_bulletStyleSignalMapper, SIGNAL(mapped(int)), this, SLOT(OnBulletStyleTriggered(int)));
}

//--------------------------------------------------------------------------
CRichTextEdit::~CRichTextEdit()
{
}

//--------------------------------------------------------------------------
void CRichTextEdit::initialize()
{
    InitStyleButton();
    InitBulletStyleButton();
    PopulatePointSizesCombo();

    ui.textColorButton->SetColor(QColor("Black"));
    // For text background color, need to specify no background color.

    // Disable style button at first.  It will be enabled whenever there
    // is a selection.
    ui.styleButton->setEnabled(false);

    // Hide the search button at first
    ui.richTextEditSearchWidget->hide();

    m_bPageModified = ui.textEdit->document()->isModified();

    // *** Need a function to apply the current settings of all controls to the document.

    // Set various flags
    Qt::TextInteractionFlags	textInteractionFlags = ui.textEdit->textInteractionFlags();
    ui.textEdit->setTextInteractionFlags(textInteractionFlags | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
}

//--------------------------------------------------------------------------
void CRichTextEdit::InitStyleButton()
{
	m_styleMenu.clear();

    QList<int>		styleIds = styleManager->GetStyleIds();

	foreach(int styleId, styleIds)
	{
		QAction		*pAction;

        pAction = m_styleMenu.addAction(styleManager->GetStyle(styleId).strName);

		m_styleSignalMapper.setMapping(pAction, styleId);

		connect(pAction, SIGNAL(triggered()), &m_styleSignalMapper, SLOT(map()));
	}

	ui.styleButton->setMenu(&m_styleMenu);
}

//--------------------------------------------------------------------------
void CRichTextEdit::InitBulletStyleButton()
{
    m_bulletStyleMenu.clear();

    QAction		*pAction;

    QStringList bulletTypes;

    bulletTypes << "Solid circle" << "Hollow circle" << "Solid square" <<
                   "Decimal numbers" << "Lower-case Latin characters" <<
                   "Upper-case Latin characters" << "Lower-case Roman numerals" <<
                   "Upper-case Roman numerals";
    int     id = QTextListFormat::ListDisc;

    foreach(const QString& bulletStr, bulletTypes)
    {
        pAction = m_bulletStyleMenu.addAction(bulletStr);

        m_bulletStyleSignalMapper.setMapping(pAction, id);

        connect(pAction, SIGNAL(triggered()), &m_bulletStyleSignalMapper, SLOT(map()));

        id--;
    }


    ui.bulletTableInsertButton->setMenu(&m_bulletStyleMenu);
    ui.numberTableInsertButton->setMenu(&m_bulletStyleMenu);
}

//--------------------------------------------------------------------------
void CRichTextEdit::PopulatePointSizesCombo()
{
	QFontDatabase	fontDatabase;
	QString			curFontFamily = ui.fontCombo->currentText();

	ui.sizeCombo->clear();
	QList<int>	fontSizeList = fontDatabase.pointSizes(curFontFamily);

	foreach (int curFontSize, fontSizeList)
	{
		QString		fontSizeStr = QString("%1").arg(curFontSize);
		ui.sizeCombo->addItem(fontSizeStr);
	}
}

//--------------------------------------------------------------------------
int CRichTextEdit::FindClosestFontSize(int fontSize)
{
	int	curFontSize;
	int  index = ui.sizeCombo->findText(QString("%1").arg(fontSize));

	if (index != -1)
	{
		// Found the given font size in the combo box, so just return it
		return fontSize;
	}

	// The given font size was not found, so search for the first size that is
	// larger than the given font size
	for (int i = 0; i < ui.fontCombo->count(); i++)
	{
		bool	bOk;

		QString  fontSizeStr = ui.sizeCombo->itemText(i);
		if (fontSizeStr.isEmpty())
			continue;

		curFontSize = fontSizeStr.toInt(&bOk, 10);

		Q_ASSERT(bOk);
		if (bOk)
		{
			if (curFontSize >= fontSize)
			{
				return curFontSize;
			}
		}
	}

	// fontSize is larger than any font in the combo box.  In this
	// case, return the largest font in the combo box.
	return curFontSize;
}

//--------------------------------------------------------------------------
void CRichTextEdit::SetDefaultFontToEntireDocument()
{
	QTextCursor			selectionCursor(ui.textEdit->textCursor());
	QTextCharFormat		selectionFormat, tempCharFormat;

	// Select entire document
	selectionCursor.select(QTextCursor::Document);

	selectionFormat = selectionCursor.charFormat();

    tempCharFormat.setFont(m_defaultFont);
	selectionCursor.mergeCharFormat(tempCharFormat);

	ui.textEdit->setTextCursor(selectionCursor);

    QTextDocument	*pDoc = ui.textEdit->document();

    pDoc->setDefaultFont(m_defaultFont);

    UpdateControls();
}

//--------------------------------------------------------------------------
void CRichTextEdit::SetDefaultFont(QFont &font)
{
    m_defaultFont = font;

    QTextDocument	*pDoc = ui.textEdit->document();

    pDoc->setDefaultFont(m_defaultFont);
}

//--------------------------------------------------------------------------
void CRichTextEdit::SetDefaultFont(QString family, int size)
{
    // Set the default font
    QFont   tempFont;

    tempFont.setFamily(family);
    tempFont.setPointSize(size);

    SetDefaultFont(tempFont);
}

//--------------------------------------------------------------------------
int CRichTextEdit::getDefaultFontSize()
{
    return ui.textEdit->document()->defaultFont().pointSize();
}

//--------------------------------------------------------------------------
QString CRichTextEdit::getDefaultFontFamily()
{
    return ui.textEdit->document()->defaultFont().family();
}

//--------------------------------------------------------------------------
bool CRichTextEdit::IsModified()
{
	QTextDocument	*pDoc = ui.textEdit->document();
	return pDoc->isModified();
}

//--------------------------------------------------------------------------
void CRichTextEdit::SetDocumentModified(bool bModified)
{
	QTextDocument	*pDoc = ui.textEdit->document();
	pDoc->setModified(bModified);
	m_bPageModified = bModified;
}

//--------------------------------------------------------------------------
void CRichTextEdit::NewDocument()
{
	ui.textEdit->clear();
	SetDocumentModified(false);

    // Set the document's font
    SetDefaultFontToEntireDocument();
}

//--------------------------------------------------------------------------
void CRichTextEdit::setDocument( QTextDocument* document )
{
	ui.textEdit->setDocument(document);
	SetDocumentModified(false);
}

//--------------------------------------------------------------------------
void CRichTextEdit::setHtml( const QString& text, QStringList imageNames )
{
	ui.textEdit->clear();

    // Store images in the document's image cache
    LoadImagesIntoCache(imageNames);

    ui.textEdit->insertHtml(text);
	SetDocumentModified(false);
}

//--------------------------------------------------------------------------
void CRichTextEdit::UpdateControls()
{
    QTextCursor         selectionCursor = ui.textEdit->textCursor();
    QTextCharFormat     selectionFormat;

	selectionFormat = selectionCursor.charFormat();

	QString		fontFamily = selectionFormat.fontFamily();
	int  index = ui.fontCombo->findText(fontFamily);
	if (index != -1)
	{
		ui.fontCombo->setCurrentIndex(index);
	}

	qreal		fontSize = selectionFormat.fontPointSize();
	QString		fontSizeStr = QString("%1").arg(fontSize);
	index = ui.sizeCombo->findText(fontSizeStr);
	if (index != -1)
	{
		ui.sizeCombo->setCurrentIndex(index);
	}

	ui.boldButton->setChecked(selectionFormat.fontWeight() == QFont::Bold);
	ui.italicButton->setChecked(selectionFormat.fontItalic());
	ui.underlineButton->setChecked(selectionFormat.fontUnderline());
    ui.strikethroughButton->setChecked(selectionFormat.fontStrikeOut());

	QTextBlockFormat	curBlockFormat = selectionCursor.blockFormat();
	int					alignmentVal = curBlockFormat.alignment();

	alignmentVal &= 0x0000000f;		// Mask off other flags

	switch (alignmentVal)
	{
		case Qt::AlignLeft:
			ui.leftAlignButton->setChecked(true);
			break;

		case Qt::AlignHCenter:
			ui.centerAlignButton->setChecked(true);
			break;

		case Qt::AlignRight:
			ui.rightAlignButton->setChecked(true);
			break;
	}

	QBrush	textBrush = selectionFormat.foreground();
	if (textBrush.isOpaque())
		ui.textColorButton->SetColor(textBrush.color());
	else
		ui.textColorButton->SetNoColor();

	QBrush	bgBrush = selectionFormat.background();
	if (bgBrush.isOpaque())
		ui.textBackgroundButton->SetColor(bgBrush.color());
	else
		ui.textBackgroundButton->SetNoColor();
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_boldButton_clicked()
{
	QTextCursor			selectionCursor(ui.textEdit->textCursor());
	QTextCharFormat		selectionFormat, tempCharFormat;

	selectionFormat = selectionCursor.charFormat();

	if (selectionFormat.fontWeight() != QFont::Bold)
		tempCharFormat.setFontWeight(QFont::Bold);
	else
		tempCharFormat.setFontWeight(QFont::Normal);

	selectionCursor.mergeCharFormat(tempCharFormat);

	ui.textEdit->setTextCursor(selectionCursor);
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_italicButton_clicked()
{
	QTextCursor			selectionCursor(ui.textEdit->textCursor());
	QTextCharFormat		selectionFormat, tempCharFormat;

	selectionFormat = selectionCursor.charFormat();

	tempCharFormat.setFontItalic(! selectionFormat.fontItalic());
	selectionCursor.mergeCharFormat(tempCharFormat);

	ui.textEdit->setTextCursor(selectionCursor);
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_underlineButton_clicked()
{
	QTextCursor			selectionCursor(ui.textEdit->textCursor());
	QTextCharFormat		selectionFormat, tempCharFormat;

	selectionFormat = selectionCursor.charFormat();

	tempCharFormat.setFontUnderline(! selectionFormat.fontUnderline());
	selectionCursor.mergeCharFormat(tempCharFormat);

	ui.textEdit->setTextCursor(selectionCursor);
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_strikethroughButton_clicked()
{
    QTextCursor			selectionCursor(ui.textEdit->textCursor());
    QTextCharFormat		selectionFormat, tempCharFormat;

    selectionFormat = selectionCursor.charFormat();

    tempCharFormat.setFontStrikeOut(! selectionFormat.fontStrikeOut());
    selectionCursor.mergeCharFormat(tempCharFormat);

    ui.textEdit->setTextCursor(selectionCursor);
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_fontCombo_activated( const QString& text )
{
	PopulatePointSizesCombo();

	QTextCursor			selectionCursor(ui.textEdit->textCursor());
	QTextCharFormat		selectionFormat, tempCharFormat;

	selectionFormat = selectionCursor.charFormat();

	tempCharFormat.setFontFamily(text);
	selectionCursor.mergeCharFormat(tempCharFormat);

	ui.textEdit->setTextCursor(selectionCursor);
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_sizeCombo_activated( const QString& text )
{
	int					fontSize;
	bool				bOk;
	QTextCursor			selectionCursor(ui.textEdit->textCursor());
	QTextCharFormat		selectionFormat, tempCharFormat;

	fontSize = text.toInt(&bOk, 10);

	if (bOk)
	{
		selectionFormat = selectionCursor.charFormat();

		tempCharFormat.setFontPointSize((qreal) fontSize);
		selectionCursor.mergeCharFormat(tempCharFormat);

		ui.textEdit->setTextCursor(selectionCursor);
	}
}

//--------------------------------------------------------------------------
void CRichTextEdit::OnTextColorChanged(const QColor& color)
{
	QTextCursor			selectionCursor(ui.textEdit->textCursor());
	QTextCharFormat		selectionFormat, tempCharFormat;

	selectionFormat = selectionCursor.charFormat();

	tempCharFormat.setForeground(QBrush(color));
	selectionCursor.mergeCharFormat(tempCharFormat);

	ui.textEdit->setTextCursor(selectionCursor);
}

//--------------------------------------------------------------------------
void CRichTextEdit::OnTextColorNoColor()
{
	QTextCharFormat		selectionFormat;
	QTextCursor			selectionCursor(ui.textEdit->textCursor());

	selectionFormat = selectionCursor.charFormat();

	// NOTE: This approach will cause all text in the selection to take on
	// all characteristics of the selectionFormat.  It has the effect of
	// removing formatting changes within the block.  Unfortunately,
	// mergeCharFormat does not work when clearing a property.
	selectionFormat.clearForeground();
	selectionCursor.setCharFormat(selectionFormat);

  	ui.textEdit->setTextCursor(selectionCursor);
}

//--------------------------------------------------------------------------
void CRichTextEdit::OnTextBackgroundChanged(const QColor& color)
{
	QTextCursor			selectionCursor(ui.textEdit->textCursor());
	QTextCharFormat		selectionFormat, tempCharFormat;

	selectionFormat = selectionCursor.charFormat();

	tempCharFormat.setBackground(QBrush(color));
	selectionCursor.mergeCharFormat(tempCharFormat);

	ui.textEdit->setTextCursor(selectionCursor);
}

//--------------------------------------------------------------------------
void CRichTextEdit::OnBackgroundNoColor()
{
	QTextCursor			selectionCursor(ui.textEdit->textCursor());
	QTextCharFormat		selectionFormat;

	selectionFormat = selectionCursor.charFormat();

	// NOTE: This approach will cause all text in the selection to take on
	// all characteristics of the selectionFormat.  It has the effect of
	// removing formatting changes within the block.  Unfortunately,
	// mergeCharFormat does not work when clearing a property.
	selectionFormat.clearBackground();
	selectionCursor.setCharFormat(selectionFormat);

	ui.textEdit->setTextCursor(selectionCursor);
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_textEdit_cursorPositionChanged()
{
	UpdateControls();
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_leftAlignButton_clicked()
{
	QTextCursor			selectionCursor(ui.textEdit->textCursor());
	QTextBlockFormat	selectionFormat;

	selectionFormat = selectionCursor.blockFormat();

	selectionFormat.setAlignment(Qt::AlignLeft);
	selectionCursor.setBlockFormat(selectionFormat);

	ui.textEdit->setTextCursor(selectionCursor);
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_centerAlignButton_clicked()
{
	QTextCursor			selectionCursor(ui.textEdit->textCursor());
	QTextBlockFormat	selectionFormat;

	selectionFormat = selectionCursor.blockFormat();

	selectionFormat.setAlignment(Qt::AlignCenter);
	selectionCursor.setBlockFormat(selectionFormat);

	ui.textEdit->setTextCursor(selectionCursor);
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_rightAlignButton_clicked()
{
	QTextCursor			selectionCursor(ui.textEdit->textCursor());
	QTextBlockFormat	selectionFormat;

	selectionFormat = selectionCursor.blockFormat();

	selectionFormat.setAlignment(Qt::AlignRight);
	selectionCursor.setBlockFormat(selectionFormat);

	ui.textEdit->setTextCursor(selectionCursor);
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_bulletTableInsertButton_clicked()
{
	QTextListFormat		newListFormat;
	QTextCursor			selectionCursor(ui.textEdit->textCursor());

	newListFormat.setIndent(1);
	newListFormat.setStyle(QTextListFormat::ListDisc);

	selectionCursor.createList(newListFormat);
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_numberTableInsertButton_clicked()
{
	QTextListFormat		newListFormat;
	QTextCursor			selectionCursor(ui.textEdit->textCursor());

	newListFormat.setIndent(1);
	newListFormat.setStyle(QTextListFormat::ListDecimal);

	selectionCursor.createList(newListFormat);
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_tableButton_clicked()
{
	CTableFormatDlg			dlg(this);
	int						result;
	QTextCursor				selectionCursor(ui.textEdit->textCursor());
	QTextTableFormat		tableFormat;
    QTextFrameFormat        frameFormat;
    QTextLength             tableWidth;
	QVector<QTextLength>	columnConstraints;
    CTextTable              *pTextTable = NULL;

    if (CTextTable::CursorInTable(selectionCursor))
	{
        pTextTable = CTextTable::fromCursor(selectionCursor);
        tableFormat = pTextTable->textTableFormat();

		// Initialize dialog controls with data from the table
        frameFormat = pTextTable->textFrameFormat();
        tableWidth = frameFormat.width();

		// Header color
        QBrush	bgBrush = pTextTable->background();
		dlg.SetBackgroundColor(bgBrush.color());
    }
    else
    {
        if (selectionCursor.hasSelection())
        {
            // Convert the selected text to a table
            CTextTable::selectionToTable(selectionCursor);
            return;
        }
        else
        {
            // New table
            dlg.SetBackgroundColor(QColor("white"));
        }
    }

    dlg.SetTable(pTextTable);       // If pTable is NULL, the dialog sets a default format

	result = dlg.exec();

	if (result == QDialog::Accepted)
	{
		int		rows = dlg.Rows();
		int		columns = dlg.Columns();
		QColor	bgColor = dlg.BackgroundColor();

        if (pTextTable == NULL)
		{
			// The cursor is not within a table - insert a new one
            pTextTable = CTextTable::createAtCursor(selectionCursor, rows, columns);
		}
		else
		{
			// The cursor is within a table - allow the user to change its formatting.

            if (rows != pTextTable->rows() || columns != pTextTable->columns())
			{
                pTextTable->resize(rows, columns);
			}
		}

        columnConstraints = dlg.GetColumnConstraints();

        pTextTable->setColumnConstraints(columnConstraints);
        pTextTable->setBackground(QBrush(bgColor));
	}
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_insertHLineButton_clicked()
{
	QTextCursor			selectionCursor(ui.textEdit->textCursor());

	selectionCursor.insertHtml("<hr /><br>");
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_styleButton_clicked()
{
	// Bring up style dialog.
    CSelectStyleDlg		dlg(styleManager, this);
	int					result;

	result = dlg.exec();

	if (result == QDialog::Accepted)
	{
		// Apply the selected style
        styleManager->ApplyStyle(ui.textEdit, dlg.GetSelectedStyle());
	}

	InitStyleButton();
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_indentLeftButton_clicked()
{
    ui.textEdit->ReduceSelectionIndent();
}

//-------------------------------------------------------------------------
void CRichTextEdit::on_indentRightButton_clicked()
{
    ui.textEdit->IncreaseSelectionIndent();
}

//--------------------------------------------------------------------------
void CRichTextEdit::on_clearFormattingButton_clicked()
{
    QTextCursor			selectionCursor(ui.textEdit->textCursor());
    QTextCharFormat		selectionFormat, tempCharFormat;
    QTextBlockFormat    currentBlockFormat;

    selectionFormat = selectionCursor.charFormat();
    currentBlockFormat = selectionCursor.blockFormat();

    // Ensure lines are single-spaced
    currentBlockFormat.setLineHeight(0, QTextBlockFormat::SingleHeight);
    currentBlockFormat.setTopMargin(0);
    currentBlockFormat.setBottomMargin(0);
    selectionCursor.setBlockFormat(currentBlockFormat);

    QFont   defaultFont = ui.textEdit->document()->defaultFont();

    tempCharFormat.setFont(defaultFont);

    // Clear colors
    selectionFormat.clearBackground();
    selectionFormat.clearForeground();

    selectionCursor.setCharFormat(tempCharFormat);

    ui.textEdit->setTextCursor(selectionCursor);
}

//--------------------------------------------------------------------------
void CRichTextEdit::OnBulletStyleTriggered(int bulletId)
{
    QTextCursor			selectionCursor(ui.textEdit->textCursor());
    QTextList           *pList = selectionCursor.currentList();

    if (pList != nullptr)
    {
        QTextListFormat     listFormat = pList->format();

        if (selectionCursor.hasSelection())
        {
            // Indent the lines that are selected
            listFormat.setStyle(QTextListFormat::Style(bulletId));
        }
        else
        {
            // Indent the line containing the cursor
            listFormat.setStyle(QTextListFormat::Style(bulletId));
        }
        pList->setFormat(listFormat);
    }
}

//--------------------------------------------------------------------------
void CRichTextEdit::OnSelectionChanged()
{
	QTextCursor		selCursor = ui.textEdit->textCursor();
	ui.styleButton->setEnabled(selCursor.hasSelection());
}

//--------------------------------------------------------------------------
void CRichTextEdit::OnTextChanged()
{
	if (! m_bPageModified)
	{
		// This is the first time the document has changed since it was last
        // saved (or created), so send the TextModified() signal.
        emit TextModified();
		m_bPageModified = true;
	}
}

//--------------------------------------------------------------------------
void CRichTextEdit::OnSearchButtonPressed()
{
	ui.richTextEditSearchWidget->show();
	ui.searchEdit->setFocus(Qt::MouseFocusReason);
}

//--------------------------------------------------------------------------
void CRichTextEdit::OnFirstSearch()
{
	ui.textEdit->moveCursor(QTextCursor::Start);
	OnSearchNext();
}

//--------------------------------------------------------------------------
void CRichTextEdit::OnSearchNext()
{
	QString		searchText = ui.searchEdit->text();

	ui.textEdit->setFocus(Qt::MouseFocusReason);
	if (!ui.textEdit->find(searchText, 0))		// Find forward
	{
		// If it didn't find it, wrap around and try again
		ui.textEdit->moveCursor(QTextCursor::Start);
		ui.textEdit->find(searchText, 0);
	}
}

//--------------------------------------------------------------------------
void CRichTextEdit::OnSearchPrev()
{
	QString		searchText = ui.searchEdit->text();

	ui.textEdit->setFocus(Qt::MouseFocusReason);
	if (!ui.textEdit->find(searchText, QTextDocument::FindBackward))
	{
		// If it didn't find it, wrap around and try again
		ui.textEdit->moveCursor(QTextCursor::End);
		ui.textEdit->find(searchText, QTextDocument::FindBackward);
    }
}

//--------------------------------------------------------------------------
void CRichTextEdit::newDocument()
{
    NewDocument();
}

//--------------------------------------------------------------------------
void CRichTextEdit::InsertPageLink( int pageId )
{
	ui.textEdit->InsertPageLink(pageId);
}

//--------------------------------------------------------------------------
void CRichTextEdit::InsertWebLink( const QString& urlStr, const QString& descriptionStr )
{
    ui.textEdit->InsertWebLink(urlStr, descriptionStr, true);
}

//--------------------------------------------------------------------------
void CRichTextEdit::LoadImagesIntoCache(QStringList imageNames)
{
    foreach (QString imageName, imageNames)
    {
        QPixmap     pixmap = pluginServices->GetImage(imageName);

        if (!pixmap.isNull())
        {
            ui.textEdit->document()->addResource(QTextDocument::ImageResource, QUrl(imageName), QVariant(pixmap));
        }
    }
}

//--------------------------------------------------------------------------
QString CRichTextEdit::getPageContents()
{
    return html();
}

//--------------------------------------------------------------------------
void CRichTextEdit::setDocumentModified(bool bModified)
{
    SetDocumentModified(bModified);
}

//--------------------------------------------------------------------------
bool CRichTextEdit::isDocumentModified()
{
    return IsModified();
}

//--------------------------------------------------------------------------
void CRichTextEdit::enableEditing(bool bEnable)
{
    setEnabled(bEnable);
}

//--------------------------------------------------------------------------
void CRichTextEdit::setPageContents(QString contents, QStringList imageNames)
{
    setHtml(contents, imageNames);
}

//--------------------------------------------------------------------------
QWidget *CRichTextEdit::settingsWidget()
{
    // Create settings widget
    CRichTextEditorSettingsWidget *pEditorSettingsWidget = new CRichTextEditorSettingsWidget();

    // Populate the values of the settings
    pEditorSettingsWidget->setDefaultFont(m_defaultFont);

    return pEditorSettingsWidget;
}

//--------------------------------------------------------------------------
void CRichTextEdit::settingsDialogAccepted(QWidget *pSettingsWidget)
{
    CRichTextEditorSettingsWidget   *pEditorSettingsWidget = dynamic_cast<CRichTextEditorSettingsWidget*>(pSettingsWidget);

    QFont   font = pEditorSettingsWidget->getDefaultFont();

    SetDefaultFont(font);
}
