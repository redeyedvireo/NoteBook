#include "CToDoListEditorSettingsWidget.h"
#include "ui_CToDoListEditorSettingsWidget.h"

CToDoListEditorSettingsWidget::CToDoListEditorSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    ui.setupUi(this);
}

CToDoListEditorSettingsWidget::~CToDoListEditorSettingsWidget()
{
}

void CToDoListEditorSettingsWidget::setAutoSave(bool autoSave)
{
    ui.autoSaveCheckBox->setChecked(autoSave);
}

bool CToDoListEditorSettingsWidget::getAutoSave()
{
    return ui.autoSaveCheckBox->isChecked();
}
