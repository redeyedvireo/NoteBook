#include "CPriorityDelegate.h"

#include <QComboBox>
#include <QDebug>

CPriorityDelegate::CPriorityDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{

}

//--------------------------------------------------------------------------
QWidget *CPriorityDelegate::createEditor(QWidget *parent,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    QComboBox   *pComboBox = new QComboBox(parent);

    // Add priority levels to combo box
    for (int i = 1; i <= 10; i++)
    {
        // Note: the data field of the combo box stores the priority value.
        pComboBox->addItem(QString::number(i), QVariant(i));
    }

    return pComboBox;
}

//--------------------------------------------------------------------------
void CPriorityDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox   *pComboBox = dynamic_cast<QComboBox*>(editor);

    if (pComboBox)
    {
        // Get priority value from the index.
        // Then, use QComboBox::find to find the combo box item that corresponds to
        // the priority, then set that item as the current one.
        int priority = index.model()->data(index).toInt();
        int index = pComboBox->findData(QVariant(priority));
        pComboBox->setCurrentIndex(index);
    }
    else
    {
        qWarning() << "CPriorityDelegate::setEditorData: editor was not a combo box.";
    }
}

//--------------------------------------------------------------------------
void CPriorityDelegate::setModelData(QWidget *editor,
                                     QAbstractItemModel *model,
                                     const QModelIndex &index) const
{
    QComboBox   *pComboBox = dynamic_cast<QComboBox*>(editor);

    if (pComboBox)
    {
        int curComboIndex = pComboBox->currentIndex();

        QVariant comboData = pComboBox->itemData(curComboIndex);

        int priorityValue = comboData.toInt();

        model->setData(index, priorityValue, Qt::EditRole);   // Stores in Qt::EditRole, by default
    }
    else
    {
        qWarning() << "CPriorityDelegate::setEditorData: editor was not a combo box.";
    }
}
