#ifndef RICHTEXTEDIT_H
#define RICHTEXTEDIT_H

#include <QWidget>
#include <QTextDocument>
#include <QSignalMapper>
#include "CAbstractPageInterface.h"
#include "ui_RichTextEdit.h"

class CStyleManager;
class CRichTextEditorSettingsWidget;
class AbstractPluginServices;

class CRichTextEdit : public QWidget, public Ui_RichTextEditWidget, public CAbstractPageInterface
{
	Q_OBJECT

public:
    CRichTextEdit(CStyleManager *styleManager, AbstractPluginServices *pluginServices, QWidget *parent = 0);
	~CRichTextEdit();

    void initialize();

    /**
     * @brief Returns the editor widget for this editor.
     * @return
     */
    QWidget *editorWidget() { return this; }

	/*
	 *	Loads user font styles.
	 */
	void LoadStyles(const QString& styleFilePath);

	/*
	 *	Save user font styles.
	 */
	void SaveStyles(const QString& styleFilePath);

	/*
	 *	Populates the point size combo based on the font family specified
	 *		in the font combo.
	 */
	void PopulatePointSizesCombo();

	/*
	 *	Updates controls to match the character formatting of the current selection.
	 */
	void UpdateControls();

	QTextDocument*	document()					{ return ui.textEdit->document(); }
	QString html()								{ return ui.textEdit->document()->toHtml(); }
	void setDocument(QTextDocument* document);
	void insertHtml(const QString& text)		{ ui.textEdit->insertHtml(text); }
    void setHtml(const QString& text, QStringList imageNames = QStringList());
	void clear()								{ ui.textEdit->clear(); }
    void NewDocument();

	int FindClosestFontSize(int fontSize);

    /**
     * Applies the default font to the entire document.  This is usually only
     * used when a new document is created.
     */
    void SetDefaultFontToEntireDocument();

    /**
     * Sets the default font to be used for future documents.
     * @param font
     */
    void SetDefaultFont(QFont& font);

    /**
     * Convenience overload for setting the default font.
     * @param family
     * @param size
     */
    void SetDefaultFont(QString family, int size);

    /**
     * Returns the default font size for the text document.
     * @return
     */
    int getDefaultFontSize();

    /**
     * Returns the default font family for the text document.
     * @return
     */
    QString getDefaultFontFamily();

	bool IsModified();
	void SetDocumentModified(bool bModified);

	/*
	 *	Inserts a link to a notebook page.
	 */
	void InsertPageLink(int pageId);

	/*
	 *	Inserts a link to a web page.
	 */
	void InsertWebLink(const QString& urlStr, const QString& descriptionStr);

public slots:
	void on_textEdit_cursorPositionChanged();

	void on_boldButton_clicked();
	void on_italicButton_clicked();
	void on_underlineButton_clicked();
    void on_strikethroughButton_clicked();

	void on_leftAlignButton_clicked();
	void on_centerAlignButton_clicked();
	void on_rightAlignButton_clicked();

	void on_bulletTableInsertButton_clicked();
	void on_numberTableInsertButton_clicked();

	void on_fontCombo_activated(const QString& text);
	void on_sizeCombo_activated(const QString& text);

	void on_tableButton_clicked();

	void on_insertHLineButton_clicked();

	void on_styleButton_clicked();
    void on_indentLeftButton_clicked();
    void on_indentRightButton_clicked();

    void on_clearFormattingButton_clicked();

    void OnBulletStyleTriggered(int bulletId);

	void OnTextColorChanged(const QColor& color);
	void OnTextColorNoColor();
	void OnTextBackgroundChanged(const QColor& color);
	void OnBackgroundNoColor();
	void OnSelectionChanged();
	void OnTextChanged();

	void OnSearchButtonPressed();
	void OnFirstSearch();
	void OnSearchNext();
	void OnSearchPrev();

public:
    /*
     * CAbstractPageInterface overrides
     */
    virtual PAGE_TYPE documentType() { return kPageTypeUserText; }

    virtual void newDocument();

    virtual QString getPageContents();

    virtual void setDocumentModified(bool bModified);

    virtual bool isDocumentModified();

    virtual void enableEditing(bool bEnable);

    virtual void setPageContents(QString contents, QStringList imageNames);

    virtual QWidget *settingsWidget();

    virtual QString settingsGroupName() { return "Text Editor"; }

    virtual void settingsDialogAccepted(QWidget* pSettingsWidget);

signals:
    void TextModified();
    void GotoPage(ENTITY_ID pageId);
    void Message(QString messageStr);

protected:
	void InitStyleButton();
    void InitBulletStyleButton();

    /**
     * @brief LoadImagesIntoCache
     * @param imageNames
     *
     * Loads images from the database into the document's resource cache.
     */
    void LoadImagesIntoCache(QStringList imageNames);

private:
    Ui::RichTextEditWidget      ui;

    CStyleManager               *styleManager;
    AbstractPluginServices      *pluginServices;

    QMenu                   m_styleMenu;
    QSignalMapper           m_styleSignalMapper;

    QMenu                   m_bulletStyleMenu;
    QSignalMapper           m_bulletStyleSignalMapper;

    /*
     *	The m_bPageModified member is used only for sending the modified signal.
     *	It's purpose is to ensure that RTE_TextModified() signal is only sent
     *	once when the text is first modified, rather than continually after each
     *	character the user types.
     */
    bool                    m_bPageModified;

    QFont                   m_defaultFont;
};

#endif // RICHTEXTEDIT_H
