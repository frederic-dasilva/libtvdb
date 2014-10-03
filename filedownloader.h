#ifndef _TVDB_FILEDOWNLOADER_H_
#define _TVDB_FILEDOWNLOADER_H_

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>


namespace Tvdb {

    class FileDownloader;

    class FileDownloader : public QObject
    {
        Q_OBJECT
    public:
        explicit FileDownloader(QUrl imageUrl, QObject *parent = 0);

        virtual ~FileDownloader();

        QByteArray data() const;

        bool finished() const;
    signals:
        void downloaded(Tvdb::FileDownloader*);

    private slots:
        void fileDownloaded(QNetworkReply* pReply);

    private:
        QNetworkAccessManager m_WebCtrl;
        QByteArray m_DownloadedData;
        bool _finished;
    };
}

#endif // FILEDOWNLOADER_H

