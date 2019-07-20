#ifndef TAGLIST_H
#define TAGLIST_H

#include "PageData.h"
#include <QListWidget>
#include <QMenu>
#include <QSignalMapper>


class CNoteBook;

class CTagList : public QListWidget
{
	Q_OBJECT

public:
	CTagList(QWidget *parent);
	virtual ~CTagList();

	void Initialize(CNoteBook *parent);

	void AddItem(const QString& tagStr);

	/*
	 *	Adds multiple items to the tree.  The tree is cleared first.
	 *	This function is used when adding items from a newly-opened NoteBook file.
	 */
	void AddItems(const PAGE_ID_TAG_CACHE& pageIdTagCache);

protected:
	void SetConnections(CNoteBook *parent);

	QListWidgetItem* FindItem(const QString& itemStr);

	void RemoveItem(const QString& itemStr);

public slots:
	void OnPageSaved(CPageData pageData);

private slots:
	void OnContextMenu(const QPoint& pos);
	void OnPageSelected(int pageId);
	void OnPageImported(CPageData pageData);
	void OnPageUpdatedByImport(CPageData pageData);

signals:
	void TL_PageSelected(ENTITY_ID pageId);

private:
	QMenu				m_contextMenu;
	QSignalMapper		m_signalMapper;
};

#endif // TAGLIST_H
