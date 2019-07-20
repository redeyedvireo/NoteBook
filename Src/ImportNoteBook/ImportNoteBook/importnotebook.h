#ifndef IMPORTNOTEBOOK_H
#define IMPORTNOTEBOOK_H

#include <QDialog>
#include "ImportDatabase.h"

class ImportNoteBook : public QDialog
{
	Q_OBJECT

public:
	ImportNoteBook(QWidget *parent = 0);
	~ImportNoteBook();

	/*
	 *	Entry point for performing the import of an external NoteBook.
	 */
	PAGE_LIST Import(const QString& filePath);

private:
	CImportDatabase		*m_pDatabase;
};

#endif // IMPORTNOTEBOOK_H
