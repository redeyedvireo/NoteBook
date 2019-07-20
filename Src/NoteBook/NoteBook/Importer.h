#ifndef Importer_h__
#define Importer_h__

#include "PageData.h"
#include <QWidget>

class CDatabase;

class CImporter : public QObject
{
	Q_OBJECT

public:
    CImporter(CDatabase *db, QWidget* parent);
	~CImporter(void);

	void DoImport(PAGE_LIST& logEntryList);

signals:
	void IMPORTER_PageAdded(CPageData pageData);
	void IMPORTER_PageUpdated(CPageData pageData);

protected:
	void DuplicateEntryQuery(const QString& pageTitle, DecisionType& outUserDecision, bool& outDoAlways);

private:
	QWidget			*m_pParent;
    CDatabase       *m_db;
};

#endif // Importer_h__
