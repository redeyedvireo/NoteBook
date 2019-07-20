#include <QUuid>
#include <QUrl>
#include <QTextCursor>
#include <QPixmap>
#include "CTextImage.h"
#include "AbstractPluginServices.h"


// Number of letters to use in a random image name.
const int kNumLettersInImageName = 10;


//--------------------------------------------------------------------------
CTextImage::CTextImage()
{
}

//--------------------------------------------------------------------------
CTextImage::~CTextImage()
{

}

//--------------------------------------------------------------------------
QString CTextImage::generateRandomImageName()
{
    QUuid   uuid = QUuid::createUuid();
    return uuid.toString();
}

//--------------------------------------------------------------------------
void CTextImage::insertImageIntoDocument(AbstractPluginServices *pluginServices, QTextDocument *pDocument, QTextCursor cursor, QString imageFilePath)
{
    QPixmap         pixmap(imageFilePath);
    QString         randomImageName;

    randomImageName = generateRandomImageName();

    pDocument->addResource(QTextDocument::ImageResource, QUrl(randomImageName), QVariant(pixmap));

    if (pluginServices->AddImageToCurrentPage(randomImageName, pixmap))
    {
        QTextImageFormat    imageFormat;
        imageFormat.setName(randomImageName);
        cursor.insertImage(imageFormat);
    }
}

