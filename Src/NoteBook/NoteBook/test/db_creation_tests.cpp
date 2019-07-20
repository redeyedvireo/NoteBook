#include <QObject>
#include "../Database.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;

namespace {
// Fixture for testing CDatabase
class DbCreationTest : public ::testing::Test {

public:
    virtual void SetUp() {
        pObject = new QObject();
        db = new CDatabase(pObject);
    }

    virtual void TearDown() {
        delete db;
        delete pObject;
    }

    CDatabase       *db;
    QObject         *pObject;
};

TEST_F(DbCreationTest, NoDatabaseOpenAtCreation) {
    EXPECT_FALSE(db->IsDatabaseOpen());
}

}   // namespace

