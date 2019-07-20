#ifndef PluginInterface_h__
#define PluginInterface_h__

#include <QStringList>
#include <QString>
#include <QSettings>
#include "CAbstractPageInterface.h"
#include "PageData.h"

// TODO: After an SDK directory has been created, add an include
//		line for PageData.h.  Until then, the plugin will
//		need to include PageData.h before including this file.

/*
 * PluginInterface.h
 *
 * Interface for plug-ins.
 */

class CPluginInterface
{
public:
	virtual ~CPluginInterface()  {}

    /*
     * The Capabilities flags indicate which operations a plug-in supports.
     * Plug-ins may support more than one operation, though it is typical
     * for a plug-in to support only one operation.
     *
     * Operations:
     *   Import     - Import data from an external file
     *   Edit       - Provide editing or read-only display capability
     *                  for a particular data type
     */
    enum Capability {
        Import = 0x0,
        Edit = 0x1
    };

    Q_DECLARE_FLAGS(Capabilities, Capability)

    /**
     * Returns the capabilities supported by the plug-in.
     * @return Capabilities flag.
     */
    virtual CPluginInterface::Capabilities capabilities() const = 0;

    /**
     * Called by the application to start the process of importing data.
     * This includes asking the user for a file, and optionally allowing him to
     * select specific data elements from the file.
     * @param pParent
     * @param importFilePath
     * @return
     */
	virtual PAGE_LIST DoImport(QObject *pParent, QString& importFilePath) = 0;

    /**
     * For plug-ins that require a menu item, this call-back returns the menu item string.
     * This string should not include a trailing elipsis ("...").
     * Plug-ins that don't require a menu item should return QString().
     * @return
     */
	virtual QString MenuItemString() const = 0;				// Don't add "..."

    /**
     * For plug-ins that provide editing capability, this call-back returns a pointer
     * to the plug-in's page interface.  Plug-ins that don't support editing should
     * return NULL.
     *
     * @return
     */
    virtual CAbstractPageInterface *getPageInterface() const = 0;

    /**
     * For plug-ins that provide editing capability, this call-back returns the type of
     * page data that it edits.
     *
     * @return Page type for which the plug-in edits, or -1 if it does not provide editing capability.
     */
    virtual int getPageType() const = 0;

    /**
     * Provides an opportunity for plug-ins to initialize themselves.  This is a good place for plug-ins
     * to load data files, perform object instantiaion, or do any other sort of initialization that
     * needs to be done.
     *
     * @param parent    Pointer to widget that will be the parent of the plug-in's widget.
     * @param pluginServices Pointer to the application's plug-in services object.
     */
    virtual void initialize(QWidget *parent, AbstractPluginServices *pluginServices) = 0;

    /**
     * Called just before the app shuts down, and provides plug-ins a chance to save data files or
     * do any other finalization activities.
     */
    virtual void shutdown() = 0;

    /*
     * Settings save/load
     */

    /**
     * Loads settings values from a settings object which is read from a file by
     * the application.
     * @param settingsObj
     */
    virtual void loadSettings(QSettings& settingsObj) = 0;

    /**
     * Saves settings to a settings object, which is then saved to a disk file by
     * the application.
     * @param settingsObj
     */
    virtual void saveSettings(QSettings& settingsObj) = 0;
};

#define PluginInterface_iid		"com.notebook.NoteBook.PluginInterface/1.0"

Q_DECLARE_OPERATORS_FOR_FLAGS(CPluginInterface::Capabilities)
Q_DECLARE_INTERFACE(CPluginInterface, PluginInterface_iid)

#endif // PluginInterface_h__
