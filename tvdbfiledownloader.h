#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class TVDBFileDownloader;

class TVDBFileDownloader : public QObject
{
    Q_OBJECT
public:
    explicit TVDBFileDownloader(QUrl imageUrl, QObject *parent = 0);

    virtual ~TVDBFileDownloader();

    QByteArray data() const;

    bool finished() const;
signals:
    void downloaded(TVDBFileDownloader*);

private slots:
    void fileDownloaded(QNetworkReply* pReply);

private:
    QNetworkAccessManager m_WebCtrl;
    QByteArray m_DownloadedData;
    bool _finished;
};

#endif // FILEDOWNLOADER_H

