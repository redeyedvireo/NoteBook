#ifndef CUSTOMWEBPAGE_H
#define CUSTOMWEBPAGE_H

#include <QObject>
#include <QWebEnginePage>

class CustomWebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    CustomWebPage(QObject *parent = 0);

signals:
    void linkClicked(const QUrl& link);

protected:
    /*
     * When QWebEngine is used, this is the only way to intercept link clicks.
     */
    virtual bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame);
};

#endif // CUSTOMWEBPAGE_H
