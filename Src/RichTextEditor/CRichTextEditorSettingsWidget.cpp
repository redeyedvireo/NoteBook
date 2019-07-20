#include "CRichTextEditorSettingsWidget.h"

CRichTextEditorSettingsWidget::CRichTextEditorSettingsWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    PopulatePointSizesCombo();
}

//--------------------------------------------------------------------------
void CRichTextEditorSettingsWidget::setDefaultFont(QFont &font)
{
    ui.fontCombo->setCurrentFont(font);

    // Find closest point size for font size combo
    int closestPointSize = FindClosestFontSize(font.pointSize());

    int index = ui.fontSizeCombo->findText(QString::number(closestPointSize));

    ui.fontSizeCombo->setCurrentIndex(index != -1 ? index : 0);
}

//--------------------------------------------------------------------------
QFont CRichTextEditorSettingsWidget::getDefaultFont()
{
    return ui.fontCombo->currentFont();
}

//--------------------------------------------------------------------------
void CRichTextEditorSettingsWidget::PopulatePointSizesCombo()
{
    QFontDatabase	fontDatabase;
    QString			curFontFamily = ui.fontCombo->currentText();

    ui.fontSizeCombo->clear();
    QList<int>	fontSizeList = fontDatabase.pointSizes(curFontFamily);

    foreach (int curFontSize, fontSizeList)
    {
        QString		fontSizeStr = QString("%1").arg(curFontSize);
        ui.fontSizeCombo->addItem(fontSizeStr);
    }
}

//--------------------------------------------------------------------------
int CRichTextEditorSettingsWidget::FindClosestFontSize(int fontSize)
{
    int	curFontSize;
    int  index = ui.fontSizeCombo->findText(QString("%1").arg(fontSize));

    if (index != -1)
    {
        // Found the given font size in the combo box, so just return it
        return fontSize;
    }

    // The given font size was not found, so search for the first size that is
    // larger than the given font size
    for (int i = 0; i < ui.fontCombo->count(); i++)
    {
        bool	bOk;

        QString  fontSizeStr = ui.fontSizeCombo->itemText(i);
        if (fontSizeStr.isEmpty())
            continue;

        curFontSize = fontSizeStr.toInt(&bOk, 10);

        Q_ASSERT(bOk);
        if (bOk)
        {
            if (curFontSize >= fontSize)
            {
                return curFontSize;
            }
        }
    }

    // fontSize is larger than any font in the combo box.  In this
    // case, return the largest font in the combo box.
    return curFontSize;
}
