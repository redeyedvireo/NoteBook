#ifndef CRICHTEXTEDITORSETTINGSWIDGET_H
#define CRICHTEXTEDITORSETTINGSWIDGET_H

#include "ui_RichTextEditorSettingsWidget.h"

#include <QWidget>

class CRichTextEditorSettingsWidget : public QWidget, public Ui_RichTextEditorSettingsWidget
{
    Q_OBJECT
public:
    explicit CRichTextEditorSettingsWidget(QWidget *parent = 0);

    void setDefaultFont(QFont& font);

    QFont getDefaultFont();

signals:

public slots:

private:
    Ui::RichTextEditorSettingsWidget        ui;

    void PopulatePointSizesCombo();
    int FindClosestFontSize(int fontSize);
};

#endif // CRICHTEXTEDITORSETTINGSWIDGET_H
