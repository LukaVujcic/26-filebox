#ifndef SERVERFILESBROWSER_H
#define SERVERFILESBROWSER_H
#include <QString>
#include <zipper/unzipper.h>
#include <QDirIterator>
class ServerFilesBrowser
{
private:
    QString m_zipPath;
public:
    ServerFilesBrowser(const QString& zipPath);
    QString getFolderPath() const;
};

#endif // SERVERFILESBROWSER_H
