#ifndef CHOOSEPAGETOLINKDLG_H
#define CHOOSEPAGETOLINKDLG_H

#include <QDialog>
#include "AbstractPluginServices.h"
#include "ui_ChoosePageToLinkDlg.h"

class ChoosePageToLinkDlg : public QDialog
{
    Q_OBJECT

public:
    ChoosePageToLinkDlg(AbstractPluginServices *pluginServices, QWidget *parent);
    ~ChoosePageToLinkDlg();

    /**
     * Returns the page ID of the selected page.
     * @return Page ID of the selected page.
     */
    ENTITY_ID getSelectedPage();

private slots:
    void on_clearFilterButton_clicked();
    void on_filterEdit_returnPressed();
    void on_filterEdit_textChanged(const QString & text);
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    void LoadPageTitles(AbstractPluginServices *pluginServices);
    QString itemText(int row);
    void setRowVisibility(int row, bool visible);
    void FilterContents();
    void ShowAllRows();

    Ui::ChoosePageToLinkDlg         ui;
};

#endif // CHOOSEPAGETOLINKDLG_H
