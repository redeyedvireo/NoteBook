#include "PageItemModel.h"
#include "TitleLabelWidget.h"
#include "AbstractPluginServices.h"
#include <QCompleter>
#include <QKeyEvent>
#include <QDebug>

#define kPageIcon   ":/NoteBook/Resources/Page.png"
#define kFolderIcon ":/NoteBook/Resources/Folder Closed.png"
#define kToDoIcon   ":/NoteBook/Resources/ToDoList.png"


CTitleLabelWidget::CTitleLabelWidget(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.favoritesLabel, SIGNAL(LabelClicked()), this, SIGNAL(SetPageAsFavorite()));
    connect(ui.favoritesLabel, SIGNAL(LabelRightClicked()), this, SIGNAL(SetPageAsNonFavorite()));
}

//--------------------------------------------------------------------------
CTitleLabelWidget::~CTitleLabelWidget()
{
    // TODO: Determine what things need to be deleted by this class, and which things
    //       Qt will be deleting.
}

//--------------------------------------------------------------------------
void CTitleLabelWidget::Initialize(AbstractPluginServices *pluginServices)
{
    this->pluginServices = pluginServices;

    // Set the stacked widget to show the label widget
    ui.stackedWidget->setCurrentIndex(kLabelPage);

    ui.favoritesLabel->LoadIconForIndex(0, ":/NoteBook/Resources/star-outline.png");
    ui.favoritesLabel->LoadIconForIndex(1, ":/NoteBook/Resources/star.png");

    ui.pageTitleCombo->installEventFilter(this);
    ui.pageTitleLabel->installEventFilter(this);

    // Set a completer on the edit box
    completer = new QCompleter(this);
    pageItemModel = new PageItemModel(completer);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setModel(pageItemModel);
    //completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    ui.pageTitleCombo->setCompleter(completer);

    connect(completer, SIGNAL(activated(QModelIndex&)), this, SLOT(onTitleCompletion(QModelIndex&)));
    connect(ui.pageTitleCombo, SIGNAL(activated(int)), this, SLOT(on_pageTitleCombo_activated(int)));
}

//--------------------------------------------------------------------------
void CTitleLabelWidget::SetFavoritesIcon(bool bCurrentPageIsFavorite)
{
    int		iconIndex = bCurrentPageIsFavorite ? 1 : 0;

    ui.favoritesLabel->SetIcon(iconIndex);
}

//--------------------------------------------------------------------------
void CTitleLabelWidget::SetPageTitleLabel(QString title)
{
    ui.pageTitleLabel->setText(title);
    ui.stackedWidget->setCurrentIndex(kLabelPage);
}

//--------------------------------------------------------------------------
ENTITY_ID CTitleLabelWidget::getPageId(int index)
{
    QVariant    data = ui.pageTitleCombo->itemData(index);

    return data.toUInt();
}

//--------------------------------------------------------------------------
void CTitleLabelWidget::clear()
{
    ui.pageTitleLabel->setText("");
    ui.stackedWidget->setCurrentIndex(kLabelPage);
}

//--------------------------------------------------------------------------
void CTitleLabelWidget::populatePageCombo()
{
    PAGE_HASH   pageHash;

    ui.pageTitleCombo->clear();

    QPixmap     pageIcon(kPageIcon);
    QPixmap     folderIcon(kFolderIcon);

    if (this->pluginServices->GetPageList(pageHash))
    {
        QHashIterator<ENTITY_ID, SHARED_CPAGEDATA>	it(pageHash);

        while (it.hasNext())
        {
            it.next();

            SHARED_CPAGEDATA	pageData = it.value();

            QIcon     itemIcon;

            switch (pageData->m_pageType)
            {
            case kPageTypeUserText:
                itemIcon = QIcon(kPageIcon);
                break;

            case kPageFolder:
                itemIcon = QIcon(kFolderIcon);
                break;

            case kPageTypeToDoList:
                itemIcon = QIcon(kToDoIcon);
                break;
            }

            ui.pageTitleCombo->addItem(itemIcon, pageData->m_title, pageData->m_pageId);
        }

        pageItemModel->populateModel(pageHash);
    }
}

//--------------------------------------------------------------------------
bool CTitleLabelWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui.pageTitleLabel)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            // The user clicked on the label.  Switch to the edit control.
            ui.stackedWidget->setCurrentIndex(kTitleEditPage);
            populatePageCombo();
            ui.pageTitleCombo->clearEditText();
            ui.pageTitleCombo->setFocus();
        }

        // Don't filter out the event
        return false;
    }
    else if (object == ui.pageTitleCombo)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent   *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Escape)
            {
                ui.stackedWidget->setCurrentIndex(kLabelPage);
            }
        }

        // Don't filter out the event
        return false;
    }
    else {
        return QWidget::eventFilter(object, event);
    }
}

//--------------------------------------------------------------------------
void CTitleLabelWidget::onTitleCompletion(QModelIndex &modelIndex)
{
    emitGotoPageSignal(pageItemModel->getPageId(modelIndex));
}

//--------------------------------------------------------------------------
void CTitleLabelWidget::on_pageTitleCombo_activated(int index)
{
    emitGotoPageSignal(getPageId(index));
}

//--------------------------------------------------------------------------
void CTitleLabelWidget::emitGotoPageSignal(ENTITY_ID pageId)
{
    if (pageId != kInvalidPageId)
    {
        emit GotoPage(pageId);
    }
    else
    {
        ui.stackedWidget->setCurrentIndex(kLabelPage);
    }
}
