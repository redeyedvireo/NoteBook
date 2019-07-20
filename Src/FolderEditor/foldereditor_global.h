#ifndef FOLDEREDITOR_GLOBAL_H
#define FOLDEREDITOR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FOLDEREDITOR_LIBRARY)
#  define FOLDEREDITORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define FOLDEREDITORSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // FOLDEREDITOR_GLOBAL_H