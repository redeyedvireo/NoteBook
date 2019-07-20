#include "stdafx.h"
#include "ClickableLabel.h"


//--------------------------------------------------------------------------
CClickableLabel::CClickableLabel(QWidget *parent)
	: QLabel(parent)
{

}

//--------------------------------------------------------------------------
CClickableLabel::~CClickableLabel()
{
	m_iconMap.clear();
}

//--------------------------------------------------------------------------
void CClickableLabel::LoadIconForIndex( int index, const QString& iconPath )
{
	if (m_iconMap.contains(index))
	{
		// Remove the icon for this index
		m_iconMap.remove(index);
	}

	m_iconMap.insert(index, QPixmap(iconPath));
}

//--------------------------------------------------------------------------
void CClickableLabel::mousePressEvent( QMouseEvent *event )
{
	QLabel::mousePressEvent(event);

	if (event->button() == Qt::LeftButton)
	{
		emit LabelClicked();
	}
	else if (event->button() == Qt::RightButton)
	{
		emit LabelRightClicked();
	}

}

//--------------------------------------------------------------------------
void CClickableLabel::SetIcon( int index )
{
	if (m_iconMap.contains(index))
	{
		setPixmap(m_iconMap.value(index));
	}
}
