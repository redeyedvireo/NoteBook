#include "ChoosePageToLinkDlg.h"
#include "PageData.h"
#include <QListWidget>

//--------------------------------------------------------------------------
ChoosePageToLinkDlg::ChoosePageToLinkDlg(AbstractPluginServices *pluginServices, QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    LoadPageTitles(pluginServices);

    // Set focus to the filter edit widget
    ui.filterEdit->setFocus();
}

//--------------------------------------------------------------------------
ChoosePageToLinkDlg::~ChoosePageToLinkDlg()
{

}

//--------------------------------------------------------------------------
void ChoosePageToLinkDlg::LoadPageTitles(AbstractPluginServices *pluginServices)
{
    PAGE_HASH		pageHash;

    if (pluginServices->GetPageList(pageHash))
    {
        QHashIterator<ENTITY_ID, SHARED_CPAGEDATA>	it(pageHash);

        while (it.hasNext())
        {
            it.next();

            SHARED_CPAGEDATA	pageData = it.value();

            QListWidgetItem     *newItem = new QListWidgetItem(pageData.data()->m_title, ui.listWidget);

            newItem->setData(Qt::UserRole, QVariant(pageData.data()->m_pageId));
        }

        ui.listWidget->sortItems();
    }
}

//--------------------------------------------------------------------------
ENTITY_ID ChoosePageToLinkDlg::getSelectedPage()
{
    QListWidgetItem     *item = ui.listWidget->currentItem();
    return (ENTITY_ID) (item != NULL) ? item->data(Qt::UserRole).toUInt() : kInvalidPageId;
}

//--------------------------------------------------------------------------
void ChoosePageToLinkDlg::on_clearFilterButton_clicked()
{
    ShowAllRows();
}

//--------------------------------------------------------------------------
void ChoosePageToLinkDlg::on_filterEdit_returnPressed()
{
    FilterContents();
}

//--------------------------------------------------------------------------
void ChoosePageToLinkDlg::on_filterEdit_textChanged(const QString &text)
{
    FilterContents();
}

//--------------------------------------------------------------------------
void ChoosePageToLinkDlg::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    accept();
}

//--------------------------------------------------------------------------
QString ChoosePageToLinkDlg::itemText(int row)
{
    QListWidgetItem     *item = ui.listWidget->item(row);
    return item->text();
}

//--------------------------------------------------------------------------
void ChoosePageToLinkDlg::setRowVisibility(int row, bool visible)
{
    QListWidgetItem     *item = ui.listWidget->item(row);
    item->setHidden(!visible);
}

//--------------------------------------------------------------------------
void ChoosePageToLinkDlg::FilterContents()
{
    QString     filterString = ui.filterEdit->text();

    if (!filterString.isEmpty())
    {
        int     numItems = ui.listWidget->count();

        for (int row = 0; row < numItems; row++)
        {
            setRowVisibility(row, itemText(row).contains(filterString, Qt::CaseInsensitive));
        }
    }
}

//--------------------------------------------------------------------------
void ChoosePageToLinkDlg::ShowAllRows()
{
    ui.filterEdit->setText(QString());

    int     numItems = ui.listWidget->count();

    for (int row = 0; row < numItems; row++)
    {
        setRowVisibility(row, true);
    }
}
