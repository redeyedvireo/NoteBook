#include "CustomWebPage.h"


//--------------------------------------------------------------------------
CustomWebPage::CustomWebPage(QObject *parent)
{
}

//--------------------------------------------------------------------------
bool CustomWebPage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    if (type == QWebEnginePage::NavigationTypeLinkClicked)
    {
        emit linkClicked(url);
        return false;
    }

    return true;
}
