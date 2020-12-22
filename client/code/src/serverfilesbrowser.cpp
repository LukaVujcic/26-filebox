#include "serverfilesbrowser.h"
ServerFilesBrowser::ServerFilesBrowser(const QString& zipPath)
{
    m_zipPath=QString(zipPath);

}

QString ServerFilesBrowser::getFolderPath(const QString& unzipPath) const
{
    zipper::Unzipper unzipper(m_zipPath.toStdString());
    unzipper.extract(unzipPath.toStdString());
    QDirIterator it(unzipPath, QDir::AllDirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    QString path;
    if (it.hasNext())
    {
       path=it.next();
    }
    else{
        path="";
    }
    QFile file(m_zipPath);
   if (file.exists())
   {
      file.remove();
   }

    return path;
}

