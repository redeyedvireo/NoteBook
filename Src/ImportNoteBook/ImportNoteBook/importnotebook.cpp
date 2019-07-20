#include "importnotebook.h"
#include "PreviewImportDialog.h"
#include <QFileInfo>


//--------------------------------------------------------------------------
ImportNoteBook::ImportNoteBook(QWidget *parent)
	: QDialog(parent)
{
	m_pDatabase = NULL;
}

//--------------------------------------------------------------------------
ImportNoteBook::~ImportNoteBook()
{

}

//--------------------------------------------------------------------------
PAGE_LIST ImportNoteBook::Import( const QString& filePath )
{
	PAGE_LIST	pageList;

	// Get file from edit control, and load it
	QFileInfo	fileInfo(filePath);

	if (fileInfo.exists())
	{
		// Open the NoteBook
		m_pDatabase = new (std::nothrow) CImportDatabase(this);

		if (m_pDatabase != NULL)
		{
			if (m_pDatabase->OpenDatabase(filePath))
			{
				PAGE_HASH	pageHash;

				if (m_pDatabase->GetPageList(pageHash))
				{
					// Populate dialog
					if (pageHash.size() > 0)
					{
						// Show preview dialog and add to database if user is OK with it.
						CPreviewImportDialog	previewDialog(this, pageHash, m_pDatabase);
						previewDialog.exec();

						if (previewDialog.result() == QDialog::Accepted)
						{
							pageList = previewDialog.GetPages();
						}
					}
				}
			}
		}
	}

	m_pDatabase->CloseDatabase();
	return pageList;
}
