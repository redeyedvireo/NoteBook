#include "ImportPageData.h"


//--------------------------------------------------------------------------
CImportPageData::CImportPageData(void)
{
}

//--------------------------------------------------------------------------
CImportPageData::~CImportPageData(void)
{
}

//--------------------------------------------------------------------------
CPageData CImportPageData::ToPageData()
{
	CPageData	pageData;

	pageData.m_pageId = m_pageId;
	pageData.m_parentId = m_parentId;
	pageData.m_title = m_title;
	pageData.m_pageType = m_pageType;
	pageData.m_contentString = m_contentString;
	pageData.m_tags = m_tags;
	pageData.m_previousTags = m_previousTags;
	pageData.m_modifiedDateTime = m_modifiedDateTime;
	pageData.m_createdDateTime = m_createdDateTime;
	pageData.m_numModifications = m_numModifications;
	pageData.m_additionalDataItems = m_additionalDataItems;

	return pageData;
}
