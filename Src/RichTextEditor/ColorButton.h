// Header for CColorButton

#ifndef ColorButton_h__
#define ColorButton_h__

#include <QToolButton>
#include <QMenu>
#include <QAction>

/*
 *	Tri-state button: this is a button that has an additional state that
 *	indicates "no color".
 */
class CColorButton : public QToolButton
{
	Q_OBJECT

public:
	CColorButton(QWidget* pParent);
	~CColorButton();

	/*
	 *	Sets the condition of "No color", which means that the text takes
	 *	the default colors of the document.
	 */
	void SetNoColor();

	void SetColor(const QColor& color);
	QColor GetColor()	{ return m_color; }

	bool HasColor();

	/*
	 *	Used to indicate that the color swatch should fill the entire button.
	 */
	void SetColorSwatchFillsButton(bool bFills = true)	{ m_bColorRectFillsButton = bFills; }

private slots:
	void ShowColorDialog();
	void OnNoColorActionTriggered();

signals:
	/*
	 *	Signal sent when the color has changed.
	 *	@param color The new color.
	 */
	void ColorChanged(const QColor& color);

	/*
	 *	Signal sent when user changes to "No Color"
	 */
	void NoColor();

protected:
	void paintEvent(QPaintEvent *event);

private:
	/*
	 *	Indicates if the control is currently expressing a color, as opposed
	 *	to being in a "no color" state.  A "no color" state indicates that
	 *	the text in the document should just take on the default colors of the
	 *	edit control.
	 */
	bool		m_bHasColor;
	QColor		m_color;
	QMenu		m_menu;
	QAction		m_NoColorAction;

	/*
	 *	If this is true, the color rectangle will fill the entire button (aside
	 *	from a small bit of padding around the edges.)
	 */
	bool		m_bColorRectFillsButton;
};

#endif // ColorButton_h__
