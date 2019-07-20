# NoteBook
An application for keeping notes.

This application is built using Qt.  The Qt SDK must be installed before attempting to build this application.

The Qt SDK can be obtained from https://www.qt.io/download .

When building, the compiler used must be the same as was used to build Qt.  This will vary, depending on the version
of the Qt libraries used.  Qt is typically built with both Visual Studio and Mingw.

Qt Creator was used in building the application, and the .pro files are included in the repository.  At one time,
Visual Studio was used to build the app, and the .sln files are still present in the repo, though they haven't been
used -- or maintained -- in quite a while.

Another dependency needed is OpenSSL, which can be obtained from https://www.openssl.org .
