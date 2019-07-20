#pragma once
#ifndef NavigationPane_h__
#define NavigationPane_h__

#include "Database.h"
#include "uidef.h"
#include <QStringList>
#include <QDateTime>


class CPageTree;

// TODO: This may want to derive from QObject so it can send signals.  But
//       I'm not sure yet.

class CNavigationPane
{
public:
	CNavigationPane();
	virtual ~CNavigationPane(void);

	void SetControlPointers(CPageTree* pPageTree);

	void AddItem(	ENTITY_ID pageId,
					ENTITY_ID parentId,
					const QString& pageTitle,
					const QStringList& tagList,
					const QDateTime& modifiedTime);

	/*
	 *	Creates a new entry in the tree, and makes it editable
     *	so the user can enter a title.  Note that if the pageTitle parameter
     *  is specified, the new entry in the tree will not be editable, but will
     *  assume the label given by pageTitle.
	 *	@param pageAdd Indicates whether the item will be created as a child
	 *		of the current item in the tree, as a sibling of the current item,
	 *		or as a top-level item.
	 *	@param pageAddWhere Indicates where the page should be added.
     *  @param pageTitle If specified, indicates the title of the page to be added.  If
     *          this parameter is specified, the item will not be editable in the page tree.
	 */
    void NewItem(PAGE_ADD pageAdd, PAGE_ADD_WHERE pageAddWhere = ePageAddDefault, QString pageTitle = QString());

private:
	CPageTree*			m_pPageTree;
};

#endif // NavigationPane_h__
