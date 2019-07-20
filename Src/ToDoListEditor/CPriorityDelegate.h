#ifndef CPRIORITYDELEGATE_H
#define CPRIORITYDELEGATE_H

#include <QObject>
#include <QWidget>
#include <QStyledItemDelegate>

class CPriorityDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    CPriorityDelegate(QObject *parent = 0);
    virtual QWidget *createEditor(QWidget *parent,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor,
                              QAbstractItemModel *model,
                              const QModelIndex &index) const;
};

#endif // CPRIORITYDELEGATE_H
