#pragma once
#include "PageData.h"

class CImportPageData :
	public CPageData
{
public:
	CImportPageData(void);
	virtual ~CImportPageData(void);

	CPageData ToPageData();

	QString			m_pageModifier;		// Name of modifier
};

typedef QList<CImportPageData>		IMPORT_PAGE_LIST;
