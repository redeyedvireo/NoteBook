#include <QMainWindow>
#include "../PageData.h"

class MockCNoteBook : public QMainWindow {
    Q_OBJECT

public:
    MockCNoteBook();
    virtual ~MockCNoteBook();

signals:
    void MW_NewBlankPageCreated(CPageData pageData);
    void MW_PageSaved(CPageData pageData);
    void MW_PageTitleChanged(CPageData pageData);
    void MW_PageDeleted(ENTITY_ID pageId);
    void MW_PageImported(CPageData pageData);
    void MW_PageUpdatedByImport(CPageData pageData);
    void MW_PageSelected(ENTITY_ID pageId);

};
