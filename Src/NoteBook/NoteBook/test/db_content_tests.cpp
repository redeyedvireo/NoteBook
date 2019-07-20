#include <QObject>
#include <QFile>
#include "../Database.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;

#define kTestDatabase   "testdb.db"
#define kDbPassword     "dbpassword"


namespace {
// Fixture for testing CDatabase content functions

class DbContentTest : public ::testing::Test {
public:
    virtual void SetUp() {
        pObject = new QObject();
        db = new CDatabase(pObject);
        db->OpenDatabase(kTestDatabase);
    }

    virtual void TearDown() {
        db->CloseDatabase();
        delete db;
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

    /**
     * Creates a new blank page.
     * @return Returns the page ID, or kInvalidPageId (0) if an error occurred.
     */
    int createNewBlankPage() {
        CPageData   pageData;

        pageData.m_pageType = kPageTypeUserText;
        pageData.m_parentId = kInvalidPageId;
        pageData.m_createdDateTime = QDateTime::currentDateTime();
        pageData.m_modifiedDateTime = pageData.m_createdDateTime;
        pageData.m_title = "Title";

        return db->AddNewBlankPage(pageData);
    }

    CDatabase       *db;
    QObject         *pObject;
};

TEST_F(DbContentTest, DatabaseOpenAfterCallingOpenDatabase) {
    EXPECT_TRUE(db->IsDatabaseOpen());
}

TEST_F(DbContentTest, CanSetPageOrder) {
    QString     pageOrderStr("1,2,3,4,8,7,6,5");

    EXPECT_TRUE(db->SetPageOrder(pageOrderStr));

    QString     retrievedPageOrderString;

    EXPECT_TRUE(db->GetPageOrder(retrievedPageOrderString));

    EXPECT_TRUE(pageOrderStr == retrievedPageOrderString);
}

}   // namespace
