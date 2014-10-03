#include "filedownloader.h"

namespace Tvdb {


FileDownloader::FileDownloader(QUrl url, QObject *parent) :
    QObject(parent),_finished(false)
{
    connect(&m_WebCtrl, SIGNAL(finished(QNetworkReply*)),
            SLOT(fileDownloaded(QNetworkReply*)));

    QNetworkRequest request(url);
    m_WebCtrl.get(request);

    // TODO: add timeout ?
}

FileDownloader::~FileDownloader()
{

}

void FileDownloader::fileDownloaded(QNetworkReply* pReply)
{
    _finished = true;
    m_DownloadedData = pReply->readAll();
    pReply->deleteLater();
    emit downloaded(this);
}

QByteArray FileDownloader::data() const
{
    return m_DownloadedData;
}


bool FileDownloader::finished() const
{ return _finished; }


}
