#include "stdafx.h"
#include "notebook.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    CDatabase   *db = new CDatabase(0);
    CNoteBook   *w = new CNoteBook(db, 0, 0);

    w->initialize();
    w->show();

    int returnCode = a.exec();

    delete w;

    // Note: it is not necessary to delete the database, since the
    // CNotebook object reparents the database to itself.  Thus, when
    // the CNotebook object is deleted, the database will be deleted
    // along with it.

    return returnCode;
}
