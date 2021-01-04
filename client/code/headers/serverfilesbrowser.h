#ifndef SERVERFILESBROWSER_H
#define SERVERFILESBROWSER_H
#include <zipper/unzipper.h>
#include <QDirIterator>
#include <QString>
class ServerFilesBrowser
{
     private:
      QString m_zipPath;

     public:
      ServerFilesBrowser(const QString& zipPath);
      QString getFolderPath(const QString& unzipPath) const;
};

#endif  // SERVERFILESBROWSER_H
