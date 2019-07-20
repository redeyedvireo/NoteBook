#ifndef CTODOLISTEDITORSETTINGSWIDGET_H
#define CTODOLISTEDITORSETTINGSWIDGET_H

#include "ui_CToDoListEditorSettingsWidget.h"
#include <QWidget>

class CToDoListEditorSettingsWidget : public QWidget, public Ui_CToDoListEditorSettingsWidget
{
    Q_OBJECT

public:
    explicit CToDoListEditorSettingsWidget(QWidget *parent = 0);
    ~CToDoListEditorSettingsWidget();

    void setAutoSave(bool autoSave);
    bool getAutoSave();

private:
    Ui::CToDoListEditorSettingsWidget   ui;
};

#endif // CTODOLISTEDITORSETTINGSWIDGET_H
