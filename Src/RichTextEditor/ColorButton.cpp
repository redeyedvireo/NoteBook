#include "stdafx.h"
#include "ColorButton.h"
#include <QPainter>
#include <QColorDialog>


//--------------------------------------------------------------------------
CColorButton::CColorButton( QWidget* pParent )
: QToolButton(pParent), m_NoColorAction(&m_menu)
{
	m_bColorRectFillsButton = false;
	m_bHasColor = false;
	SetColor(QColor(128, 128, 128));	// Set some default color

	setPopupMode(QToolButton::MenuButtonPopup);

	m_NoColorAction.setText("No Color");

	m_menu.clear();
	m_menu.addAction(&m_NoColorAction);

	setMenu(&m_menu);

	connect(this, SIGNAL(clicked()), this, SLOT(ShowColorDialog()));
	connect(&m_NoColorAction, SIGNAL(triggered()), this, SLOT(OnNoColorActionTriggered()));
}

//--------------------------------------------------------------------------
CColorButton::~CColorButton()
{
}

//--------------------------------------------------------------------------
void CColorButton::paintEvent( QPaintEvent *event )
{
	QToolButton::paintEvent(event);

	QPainter	painter(this);
	QRect		colorRect(6, 15, 10, 4);

	if (m_bColorRectFillsButton)
	{
		colorRect = rect();
		colorRect.setRect(colorRect.x() + 2, colorRect.y() + 2, colorRect.width() - 4, colorRect.height() - 4);
	}

	// Paint the active color in the widget
	if (m_bHasColor)
	{
		painter.fillRect(colorRect, m_color);
	}
	else
	{
		// Paint an outline
		painter.setPen(QColor("black"));
		colorRect.setBottom(colorRect.bottom() - 1);
		colorRect.setRight(colorRect.right() - 1);
		painter.drawRect(colorRect);
	}
}

//--------------------------------------------------------------------------
void CColorButton::SetColor( const QColor& color )
{
	QString  styleStr;

	m_color = color;
	m_bHasColor = true;
	update();
}

//--------------------------------------------------------------------------
void CColorButton::ShowColorDialog()
{
    QColor	color = QColorDialog::getColor(m_color);

	if (color.isValid())
	{
		// If the user clicks Cancel, the returned color will not be valid
		SetColor(color);

		// Send signal to parent indicating that the color has changed.
		emit ColorChanged(color);
	}
}

//--------------------------------------------------------------------------
bool CColorButton::HasColor()
{
	return m_bHasColor;
}

//--------------------------------------------------------------------------
void CColorButton::OnNoColorActionTriggered()
{
	m_bHasColor = false;
	update();

	emit NoColor();
}

//--------------------------------------------------------------------------
void CColorButton::SetNoColor()
{
	m_bHasColor = false;

	update();
}
