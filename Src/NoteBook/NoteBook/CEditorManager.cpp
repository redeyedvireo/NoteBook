#include "CEditorManager.h"
#include "CAbstractPageInterface.h"
#include <QStackedWidget>


//--------------------------------------------------------------------------
CEditorManager::CEditorManager(QStackedWidget *pStackedWidget)
    : m_pStackedWidget(pStackedWidget)
{
}

//--------------------------------------------------------------------------
void CEditorManager::addEditor(CAbstractPageInterface *pPageInterface, int pageType)
{
    int index = m_pStackedWidget->addWidget(pPageInterface->editorWidget());
    m_editorWidgetMap.insert(pageType, index);

    m_pageInterfaceMap.insert(pageType, pPageInterface);
}

//--------------------------------------------------------------------------
void CEditorManager::activateEditor(int pageType)
{
    int stackedWidgetIndex = getStackedWidgetIndexForPageType(pageType);

    if (stackedWidgetIndex != -1)
    {
        m_pStackedWidget->setCurrentIndex(stackedWidgetIndex);
    }
}

//--------------------------------------------------------------------------
int CEditorManager::getActiveEditorPageType()
{
    int index = m_pStackedWidget->currentIndex();
    return m_editorWidgetMap.key(index);
}

//--------------------------------------------------------------------------
CAbstractPageInterface *CEditorManager::getActiveEditor()
{
    return dynamic_cast<CAbstractPageInterface*>(m_pStackedWidget->currentWidget());
}

//--------------------------------------------------------------------------
CAbstractPageInterface *CEditorManager::getEditorForPageType(int pageType)
{
    if (m_pageInterfaceMap.contains(pageType))
    {
        return m_pageInterfaceMap.value(pageType);
    }
    else
    {
        return NULL;
    }
}

//--------------------------------------------------------------------------
QWidget *CEditorManager::getEditorWidgetForPageType(int pageType)
{
    int stackedWidgetIndex = getStackedWidgetIndexForPageType(pageType);

    if (stackedWidgetIndex != -1)
    {
        return m_pStackedWidget->widget(stackedWidgetIndex);
    }
    else
    {
        return NULL;
    }
}

//--------------------------------------------------------------------------
QList<CAbstractPageInterface *> CEditorManager::getEditors()
{
    return m_pageInterfaceMap.values();
}

//--------------------------------------------------------------------------
int CEditorManager::getStackedWidgetIndexForPageType(int pageType)
{
    if (m_editorWidgetMap.contains(pageType))
    {
        return m_editorWidgetMap.value(pageType);
    }
    else
    {
        return -1;
    }
}
