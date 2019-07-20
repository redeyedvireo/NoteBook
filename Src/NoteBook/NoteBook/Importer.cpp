#include "stdafx.h"
#include "ExistingDbEntryDialog.h"
#include "Database.h"
#include "PageCache.h"
#include "Importer.h"
#include <QProgressDialog>

//--------------------------------------------------------------------------
CImporter::CImporter(CDatabase *db, QWidget* parent)
    : m_db(db), m_pParent(parent)
{
}

//--------------------------------------------------------------------------
CImporter::~CImporter(void)
{
}

//--------------------------------------------------------------------------
void CImporter::DoImport(PAGE_LIST& logEntryList)
{
	// Determine if an entry already exists for this date
	QDate			entryDate;
	QDateTime		lastModifiedDate;
	int				numModifications;
	int				numEntriesProcessed = 0;
	bool			bHasPermanentDecision = false;
	QString			progressDialogTitle;
	DecisionType	permanentDecision;

	progressDialogTitle = QString("Importing Page %1 of %2").arg(0).arg(logEntryList.size());

	QProgressDialog		progressDlg(progressDialogTitle, "Cancel", 0, logEntryList.size(), m_pParent);
	progressDlg.setWindowTitle("NoteBook - Import Progress");

	// Add the items in a transaction
    m_db->BeginTransaction();

	foreach(CPageData entry, logEntryList)
	{
		bool bSuccessfullyAdded = false;
		bool bExists = CPageCache::Instance()->Contains(entry.m_title);

		if (bExists)
		{
			// A log entry already exists.
			DecisionType	userDecision = kSkipEntry;

			if (bHasPermanentDecision)
			{
				// The user has already selected a permanent decision.  Do this.
				userDecision = permanentDecision;
			}
			else
			{
				// Ask user what to do.
				DuplicateEntryQuery(entry.m_title, userDecision, bHasPermanentDecision);

				if (bHasPermanentDecision)
				{
					permanentDecision = userDecision;
				}
			}

			switch (userDecision)
			{
			case kAbort:
				// The user has chosen to abort

				// TODO: Undo any entries that were changed up to this point.
				// This will probably involve creating a new
				// temporary table that contains the original contents of any
				// entries that were modified, and then restoring the entries
				// to the contents contained in the temporary table.  The table
				// should also contain a list of any entries that were added as
				// new entries, and then those new entries would be deleted.

				// End transaction
                m_db->EndTransaction();
				return;

			case kAppendEntry:
				{
					int		pageId;

					pageId = CPageCache::Instance()->PageId(entry.m_title);

					entry.m_pageId = pageId;
                    m_db->AppendPage(pageId, entry);

					emit IMPORTER_PageUpdated(entry);	// Note: entry doesn't contain the entire page!
					bSuccessfullyAdded = true;
				}
				break;

			case kOverwriteEntry:
				{
					int		pageId;

					pageId = CPageCache::Instance()->PageId(entry.m_title);

					entry.m_pageId = pageId;
                    m_db->UpdatePage(entry);

					emit IMPORTER_PageUpdated(entry);
					bSuccessfullyAdded = true;
				}
				break;

			case kSkipEntry:
				bSuccessfullyAdded = false;
				break;
			}
		}
		else
		{
            int		pageId = m_db->AddNewPage(entry);

			if (pageId != kInvalidPageId)
			{
				CPageCache::Instance()->AddPage(pageId, entry.m_title);

				entry.m_pageId = pageId;
				emit IMPORTER_PageAdded(entry);

				bSuccessfullyAdded = true;
			}
			else
			{
				// This should never happen.
				// TODO: Maybe should log this instead of asserting.
				Q_ASSERT(false);
				bSuccessfullyAdded = false;
			}
		}

		// Update progress bar and label text
		qApp->processEvents();		// Prevent GUI freeze

		if (progressDlg.wasCanceled())
		{
			// TODO: Undo any entries that were changed up to this point.
			// This will probably involve creating a new
			// temporary table that contains the original contents of any
			// entries that were modified, and then restoring the entries
			// to the contents contained in the temporary table.  The table
			// should also contain a list of any entries that were added as
			// new entries, and then those new entries would be deleted.
			break;
		}

		// Update progress bar
		numEntriesProcessed++;
		progressDlg.setValue(numEntriesProcessed);
		progressDlg.setLabelText(QString("Importing entry %1 of %2").arg(numEntriesProcessed).arg(logEntryList.size()));
	}

	// End transaction
    m_db->EndTransaction();
}

//--------------------------------------------------------------------------
void CImporter::DuplicateEntryQuery(const QString& pageTitle, DecisionType& outUserDecision, bool& outDoAlways)
{
	// An entry already exists for this date.  Ask user what to do.
	CExistingDbEntryDialog	dlg(pageTitle, m_pParent);

	dlg.exec();

	if (dlg.result() == QDialog::Rejected)
	{
		// The user has chosen to abort

		// TODO: Undo any entries that were changed up to this point.
		// This will probably involve creating a new
		// temporary table that contains the original contents of any
		// entries that were modified, and then restoring the entries
		// to the contents contained in the temporary table.  The table
		// should also contain a list of any entries that were added as
		// new entries, and then those new entries would be deleted.

		outUserDecision = kAbort;
	}
	else
	{
		outUserDecision = dlg.ChosenDecision();
		outDoAlways = dlg.DoThisAlways();
	}
}
