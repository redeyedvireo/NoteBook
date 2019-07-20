#ifndef CTEXTIMAGE_H
#define CTEXTIMAGE_H

#include <QString>
#include <QTextDocument>
#include <QTextImageFormat>

class AbstractPluginServices;

class CTextImage
{

    // Constructors are private.  Factory methods must be used to create new objects.
private:
    CTextImage();

public:
    ~CTextImage();

    /**
     * @brief generateRandomImageName
     * @return Returns a random image name
     *
     * Generates a random string to be used as the image name, which is used to identify
     * the image in the database.
     */
    static QString generateRandomImageName();

    /**
     * @brief insertImageIntoDocument
     * @param pDocument
     * @param imageFilePath
     *
     * Inserts the given image into the document, at the current cursor position.
     */
    static void insertImageIntoDocument(AbstractPluginServices *pluginServices, QTextDocument* pDocument, QTextCursor cursor, QString imageFilePath);
};

#endif // CTEXTIMAGE_H
