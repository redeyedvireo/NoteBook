#include <QObject>
#include <QApplication>
#include <QWidget>
#include <QFile>
#include "mockNoteBook.h"
#include "../PageTree.h"
#include "../Database.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;

#define kTestDatabase   "testdb.db"
#define kDbPassword     "dbpassword"


class SubclassedPageTree : public CPageTree {
public:
    SubclassedPageTree(QWidget *pWidget) : CPageTree(pWidget) {}
    virtual ~SubclassedPageTree() {}

    void UpdateParent(ENTITY_ID pageId, ENTITY_ID newParentId) {
        CPageTree::UpdateParent(pageId, newParentId);
    }

};

::testing::AssertionResult QStringsAreEqual(const QString& str1, const QString& str2) {
    if (str1 == str2)
        return ::testing::AssertionSuccess();
    else
        return ::testing::AssertionFailure() << qPrintable(str1) << " != " << qPrintable(str2);
}

namespace {
// Fixture for testing CPageTree

class PageTreeTest : public ::testing::Test {
public:
    virtual void SetUp() {
        int     argc = 0;
        char    *argv = 0;
        pObject = new QObject();
        pApp = new QApplication(argc, &argv);
        widget = new QWidget();
        notebook = new MockCNoteBook();
        db = new CDatabase(pObject);
        pageTree = new SubclassedPageTree(notebook);
        db->OpenDatabase(kTestDatabase);
        pageTree->Initialize(notebook, db);
    }

    virtual void TearDown() {
        db->CloseDatabase();
        delete pageTree;
        delete db;
        delete notebook;
        delete widget;
        delete pApp;
        delete pObject;
        deleteDatabase();
    }

    /**
     * Deletes the testing database (kTestDatabase).
     */
    void deleteDatabase() {
        if (QFile::exists(kTestDatabase))
        {
            QFile::remove(kTestDatabase);
        }
    }

    QWidget             *widget;
    MockCNoteBook       *notebook;
    SubclassedPageTree  *pageTree;
    CDatabase           *db;
    QObject             *pObject;
    QApplication        *pApp;
};

TEST_F(PageTreeTest, PageOrderSavedWhenPageCreated) {
    pageTree->NewItem(eNewPage, ePageAddDefault, "Title");

    QString     retrievedPageOrderString;

    EXPECT_TRUE(db->GetPageOrder(retrievedPageOrderString));

    //std::cout << "retrievedPageOrderString: " << qPrintable(retrievedPageOrderString) << endl;
    EXPECT_FALSE(retrievedPageOrderString.isEmpty());
}

TEST_F(PageTreeTest, PageOrderSavedWhenPageDeleted) {
    pageTree->NewItem(eNewPage, ePageAddDefault, "Title 1");        // Page ID 1
    pageTree->NewItem(eNewPage, ePageAddDefault, "Title 2");        // Page ID 2
    pageTree->NewItem(eNewPage, ePageAddDefault, "Title 3");        // Page ID 3

    pageTree->OnPageDeleted(2);

    QString     retrievedPageOrderString;

    EXPECT_TRUE(db->GetPageOrder(retrievedPageOrderString));

    EXPECT_TRUE(QStringsAreEqual(QString("1,3"), retrievedPageOrderString));
}

TEST_F(PageTreeTest, PageOrderSavedWhenPageMoved) {
    pageTree->NewItem(eNewPage, ePageAddDefault, "Title 1");        // Page ID 1
    pageTree->NewItem(eNewPage, ePageAddDefault, "Title 2");        // Page ID 2
    pageTree->NewItem(eNewPage, ePageAddDefault, "Title 3");        // Page ID 3

    // Simulate the user dragging page 2 to the first spot
    QTreeWidgetItem *pItem = pageTree->takeTopLevelItem(1);
    EXPECT_FALSE(NULL == pItem);

    pageTree->insertTopLevelItem(0, pItem);

    pageTree->UpdateParent(2, kInvalidPageId);

    QString     retrievedPageOrderString;

    EXPECT_TRUE(db->GetPageOrder(retrievedPageOrderString));
    //std::cout << "retrievedPageOrderString: " << qPrintable(retrievedPageOrderString) << endl;

    EXPECT_TRUE(QStringsAreEqual(QString("2,1,3"), retrievedPageOrderString));
}

TEST_F(PageTreeTest, PageOrderSavedWhenFolderCreated) {
    pageTree->NewItem(eNewFolder, ePageAddDefault, "New Folder");

    QString     retrievedPageOrderString;

    EXPECT_TRUE(db->GetPageOrder(retrievedPageOrderString));

    EXPECT_FALSE(retrievedPageOrderString.isEmpty());
}

TEST_F(PageTreeTest, PageOrderSavedWhenFolderMoved) {
    pageTree->NewItem(eNewFolder, ePageAddTopLevel, "Folder 1");        // Page ID 1
    pageTree->NewItem(eNewFolder, ePageAddTopLevel, "Folder 2");        // Page ID 2
    pageTree->NewItem(eNewFolder, ePageAddTopLevel, "Folder 3");        // Page ID 3

    // Simulate the user dragging page 2 to the first spot
    QTreeWidgetItem *pItem = pageTree->takeTopLevelItem(1);
    EXPECT_FALSE(NULL == pItem);

    pageTree->insertTopLevelItem(0, pItem);

    pageTree->UpdateParent(2, kInvalidPageId);

    QString     retrievedPageOrderString;

    EXPECT_TRUE(db->GetPageOrder(retrievedPageOrderString));

    EXPECT_TRUE(QStringsAreEqual(QString("2,1,3"), retrievedPageOrderString));
}

TEST_F(PageTreeTest, PageOrderSavedWhenFolderDeleted) {
    pageTree->NewItem(eNewFolder, ePageAddTopLevel, "Folder 1");        // Page ID 1
    pageTree->NewItem(eNewFolder, ePageAddTopLevel, "Folder 2");        // Page ID 2
    pageTree->NewItem(eNewFolder, ePageAddTopLevel, "Folder 3");        // Page ID 3

    pageTree->OnPageDeleted(2);

    QString     retrievedPageOrderString;

    EXPECT_TRUE(db->GetPageOrder(retrievedPageOrderString));

    EXPECT_TRUE(QStringsAreEqual(QString("1,3"), retrievedPageOrderString));
}

TEST_F(PageTreeTest, PageOrderStringDoesNotEndWithACommaWhenOnlyOnePagePresent) {
    pageTree->NewItem(eNewPage, ePageAddDefault, "Title 1");

    QString     pageTreeList;

    pageTreeList = pageTree->GetTreeIdList();
    EXPECT_FALSE(pageTreeList.endsWith(","));
}

TEST_F(PageTreeTest, PageOrderStringDoesNotEndWithACommaWhenMoreThanOnePagePresent) {
    pageTree->NewItem(eNewPage, ePageAddDefault, "Title 1");
    pageTree->NewItem(eNewPage, ePageAddDefault, "Title 2");

    QString     pageTreeList;

    pageTreeList = pageTree->GetTreeIdList();
    EXPECT_FALSE(pageTreeList.endsWith(","));
}

}   // namespace
