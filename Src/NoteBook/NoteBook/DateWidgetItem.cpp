#include "stdafx.h"
#include "DateWidgetItem.h"

const int kDateWidgetType = QTreeWidgetItem::UserType + 1;


//--------------------------------------------------------------------------
CDateWidgetItem::CDateWidgetItem(QTreeWidgetItem *parent, const QDate& inDate)
	: QTreeWidgetItem(parent, kDateWidgetType)
{
	setText(0, inDate.toString(Qt::TextDate));		// TODO: Change to a better format
	m_date = inDate;
}

CDateWidgetItem::~CDateWidgetItem()
{
}

//--------------------------------------------------------------------------
bool CDateWidgetItem::operator<(const QTreeWidgetItem & other) const
{
	const CDateWidgetItem&		tempOther = dynamic_cast<const CDateWidgetItem&>(other);
	return m_date < tempOther.m_date;
}

//--------------------------------------------------------------------------
bool CDateWidgetItem::operator==( const QDate& inDate )
{
	return inDate == m_date;
}

//--------------------------------------------------------------------------
bool CDateWidgetItem::operator!=( const QDate& inDate )
{
	return inDate != m_date;
}
