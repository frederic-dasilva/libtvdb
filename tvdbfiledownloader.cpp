#include "tvdbfiledownloader.h"

TVDBFileDownloader::TVDBFileDownloader(QUrl imageUrl, QObject *parent) :
    QObject(parent),_finished(false)
{
    connect(&m_WebCtrl, SIGNAL(finished(QNetworkReply*)),
            SLOT(fileDownloaded(QNetworkReply*)));

    QNetworkRequest request(imageUrl);
    m_WebCtrl.get(request);

    // TODO: add timeout
}

TVDBFileDownloader::~TVDBFileDownloader()
{

}

void TVDBFileDownloader::fileDownloaded(QNetworkReply* pReply)
{
    _finished = true;
    m_DownloadedData = pReply->readAll();
    pReply->deleteLater();
    emit downloaded(this);
}

QByteArray TVDBFileDownloader::data() const
{
    return m_DownloadedData;
}


bool TVDBFileDownloader::finished() const
{ return _finished; }
