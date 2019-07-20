#ifndef DATEWIDGETITEM_H
#define DATEWIDGETITEM_H

#include <QTreeWidgetItem>

class CDateWidgetItem : public QTreeWidgetItem
{
public:
	CDateWidgetItem(QTreeWidgetItem *parent, const QDate& inDate);
	virtual ~CDateWidgetItem();

	bool operator<(const QTreeWidgetItem & other) const;
	bool operator==(const QDate& inDate);
	bool operator!=(const QDate& inDate);

private:
	QDate			m_date;
};

#endif // DATEWIDGETITEM_H
