#ifndef TITLELABELWIDGET_H
#define TITLELABELWIDGET_H

#include <QWidget>
#include "PageData.h"
#include "ui_TitleLabelWidget.h"


class QCompleter;
class PageItemModel;
class AbstractPluginServices;

class CTitleLabelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CTitleLabelWidget(QWidget *parent = 0);
    ~CTitleLabelWidget();

    void Initialize(AbstractPluginServices *pluginServices);

    enum {
            kLabelPage = 0,
            kTitleEditPage = 1
    };

    void SetFavoritesIcon(bool bCurrentPageIsFavorite);

    void SetPageTitleLabel(QString title);

    QString GetPageTitleLabel()     { return ui.pageTitleLabel->text(); }

    void clear();

    virtual bool eventFilter(QObject *object, QEvent *event);

public slots:
    void onTitleCompletion(QModelIndex &modelIndex);
    void on_pageTitleCombo_activated(int index);

signals:
    void SetPageAsFavorite();
    void SetPageAsNonFavorite();
    void GotoPage(ENTITY_ID pageId);

private:
    ENTITY_ID getPageId(int index);
    void populatePageCombo();
    void emitGotoPageSignal(ENTITY_ID pageId);

    Ui::CTitleLabelWidget        ui;

    AbstractPluginServices      *pluginServices;
    PageItemModel               *pageItemModel;
    QCompleter                  *completer;
};

#endif // TITLELABELWIDGET_H
