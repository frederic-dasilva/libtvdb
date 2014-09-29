/*
   Copyright (C) 2010 Sebastian Trueg <trueg@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) version 3, or any
   later version accepted by the membership of KDE e.V. (or its
   successor approved by the membership of KDE e.V.), which shall
   act as a proxy defined in Section 6 of version 3 of the license.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "client.h"
#include "series.h"
#include "series_p.h"

#include <KZip>
#include <KZipFileEntry>

#include <QtCore/QXmlStreamReader>
#include <QtCore/QQueue>
#include <QtCore/QBuffer>
#include <QtCore/QScopedPointer>

#include <QDebug>
#include <QTime>

#include "tvdbfiledownloader.h"

namespace {
// the API key registered for this lib which will be used as a default
const char* s_thetvdb_api_key = "4D724EF8C0D6070D";

// the cached valid languages to avoid another http request
const char* s_thetvdb_languages[] = {
    "da",
    "fi",
    "nl",
    "de",
    "it",
    "es",
    "fr",
    "pl",
    "hu",
    "el",
    "tr",
    "ru",
    "he",
    "ja",
    "pt",
    "zh",
    "cs",
    "sl",
    "hr",
    "ko",
    "en",
    "sv",
    "no",
    0
};

enum TheTvdbTypeFlag {
    TheTvdbTypeXmlFiles = 1,
    TheTvdbTypeBannerFiles = 2,
    TheTvdbTypeZipFiles = 4
};
Q_DECLARE_FLAGS( TheTvdbTypeFlags, TheTvdbTypeFlag )
Q_DECLARE_OPERATORS_FOR_FLAGS( TheTvdbTypeFlags )

struct TvdbRequest {
    TvdbRequest( int id )
        : seriesId( id ) {
    }

    TvdbRequest( const QString& name )
        : seriesId( 0 ),
          seriesName( name ) {
    }

    int seriesId;
    QString seriesName;
};

/**
     * Parses a list of series as returned by the GetSeries.php call from thetvdb.com
     *
     * \return A list of Series objects with valid ids, names, and overviews which can be
     * presented to the user for a further call to Tvdb::Client::getSeriesById().
     */
QList<Tvdb::Series> parseSeriesList( const QByteArray& data )
{
    QList<Tvdb::Series> series;
    QXmlStreamReader xml( data );
    if ( xml.readNextStartElement() ) {
        while ( xml.readNextStartElement() &&
                xml.name() == QLatin1String( "Series" ) ) {
            int id = 0;
            QString name,  overview;
            while ( xml.readNextStartElement() ) {
                if ( xml.name() == QLatin1String( "seriesid" ) ) {
                    id = xml.readElementText().toInt();
                }
                else if ( xml.name() == QLatin1String( "SeriesName" ) ) {
                    name = xml.readElementText();
                }
                else if ( xml.name() == QLatin1String( "Overview" ) ) {
                    overview = xml.readElementText();
                }
                else {
                    // skip over this tag
                    xml.skipCurrentElement();
                }
            }
            if ( id > 0 && !name.isEmpty() ) {
                qDebug() << "found series Item:" << id << name;
                series.append( Tvdb::Series( id, name, overview ) );
            }
            else {
                qDebug() << "invalid Item:" << id << name;
            }
        }
    }

    return series;
}

/**
     * Parse a list of mirrors from thetvdb.com.
     */
QList<FUrl> parseMirrorList( const QByteArray& data, TheTvdbTypeFlags flags )
{
    QList<FUrl> mirrors;
    QXmlStreamReader xml( data );
    if( xml.readNextStartElement() && xml.name() == QLatin1String( "Mirrors" ) ) {
        qDebug() << "Parsing Mirrors";
        // iterate all mirrors
        while ( xml.readNextStartElement() &&
                xml.name() == QLatin1String( "Mirror" ) ) {
            FUrl url;
            int typemask = 0;
            while ( xml.readNextStartElement() ) {
                if ( xml.name() == QLatin1String( "mirrorpath" ) ) {
                    url = xml.readElementText();
                }
                else if ( xml.name() == QLatin1String( "typemask" ) ) {
                    typemask = xml.readElementText().toInt();
                }
                else {
                    // skip over this tag
                    xml.skipCurrentElement();
                }
            }
            if ( typemask & flags ) {
                qDebug() << "found useful mirror:" << url;
                mirrors.append( url );
            }
            else {
                qDebug() << "unusable mirror:" << url << typemask;
            }
        }
    }
    return mirrors;
}
}


class Tvdb::Client::Private
{
public:
    Private(QString language);

    QString _language;
    QString m_apiKey;

    Tvdb::Client::TvdbFlags m_flags;

    // while there can only be one request at the moment,
    // using a queue is still more convinient than checking
    // if a single instance is valid or not.
    QQueue<TvdbRequest> m_requests;

    // when fetching multiple series by id in FetchAllDetails mode
    // we cache them here until we can emit the result
    QList<Series> m_multiResultCache;

    FUrl m_usedMirrorUrl;

    QString apiKey() const {
        if ( !m_apiKey.isEmpty() )
            return m_apiKey;
        else
            return QLatin1String( s_thetvdb_api_key );
    }

    FUrl createMirrorUrl();

    void updateMirrors();
    void handleNextRequest();
    void handleRequest( const TvdbRequest& req );

    void fetchDetails(const QList<Series>& series);

    void getMirrorListResult(TVDBFileDownloader*);
    void getSeriesByNameResult(TVDBFileDownloader*);
    void getSeriesByIdResult(TVDBFileDownloader*);
    Client* q;

private:
    QList<FUrl> m_mirrors;
};


FUrl Tvdb::Client::Private::createMirrorUrl()
{
    FUrl url;
    if ( m_mirrors.isEmpty() ) {
        // fallback
        url.setUrl(QLatin1String( "http://www.thetvdb.com" ));
    }
    else {
        // choose one at random as recommended in the thetvdb api docs
        qsrand(QTime::currentTime().second());
        url = m_mirrors[int( double(qrand())/double(RAND_MAX) * double(m_mirrors.count()-1) )];
    }
    return url;
}


void Tvdb::Client::Private::updateMirrors()
{
    qDebug() << "updateMirrors";
    FUrl url = createMirrorUrl();
    url.addPath( QLatin1String( "/api/" ) );
    url.addPath( apiKey() );
    url.addPath( QLatin1String( "mirrors.xml" ) );
    qDebug() << url;

    TVDBFileDownloader *file = new TVDBFileDownloader(url);
    connect(file,SIGNAL(downloaded(TVDBFileDownloader*)),q,SLOT(getMirrorListResult(TVDBFileDownloader*)));
}


void Tvdb::Client::Private::handleNextRequest()
{
    // trueg: mirror fetching has been disabled since there are no mirrors as of August 2010.
    //        thus, we avoid one http request.
    if ( 0 && m_mirrors.isEmpty() ) {
        updateMirrors();
    }
    else {
        handleRequest( m_requests.head() );
    }
}


void Tvdb::Client::Private::handleRequest( const TvdbRequest& req )
{
    const QString lang = _language.toLatin1(); // FIXME: do something with the language

    m_usedMirrorUrl = createMirrorUrl();
    FUrl url(m_usedMirrorUrl);
    url.addPath( QLatin1String( "/api/" ) );

    if ( req.seriesId > 0 ) {
        url.addPath( apiKey() );
        url.addPath( QString::fromLatin1( "/series/%1/all/%2.zip" ).arg( req.seriesId ).arg( lang ) );
    }
    else {
        url.addPath( QLatin1String( "GetSeries.php" ) );
        url.addQueryItem( QLatin1String( "seriesname" ), req.seriesName );
        url.addQueryItem( QLatin1String( "language" ), lang );
    }

    qDebug() << url;


    TVDBFileDownloader *file = new TVDBFileDownloader(url);
    if ( req.seriesId > 0 ) {
        connect(file,SIGNAL(downloaded(TVDBFileDownloader*)),q,SLOT(getSeriesByIdResult(TVDBFileDownloader*)));
    }
    else {
        connect(file,SIGNAL(downloaded(TVDBFileDownloader*)),q,SLOT(getSeriesByNameResult(TVDBFileDownloader*)));
    }

}

void Tvdb::Client::Private::fetchDetails(const QList<Series> &series)
{
    foreach(const Series& s, series) {
        m_requests << TvdbRequest(s.id());
    }

    handleNextRequest();
}


void Tvdb::Client::Private::getMirrorListResult( TVDBFileDownloader *downloader )
{
    if (downloader == NULL)
        return;

    if (downloader->finished() ) {
        m_mirrors.clear();

        // parse mirrors
        m_mirrors = parseMirrorList(downloader->data(), TheTvdbTypeZipFiles);
    }

    downloader->deleteLater();
    handleRequest( m_requests.head() );
}

void Tvdb::Client::Private::getSeriesByIdResult(TVDBFileDownloader *downloader)
{
    if (downloader == NULL)
        return;

    TvdbRequest req = m_requests.dequeue();
    if (downloader->finished()){
        const QString lang = _language.toLatin1(); // FIXME: do something with the language

        QByteArray data = downloader->data();
        QBuffer buffer( &data );
        KZip zip( &buffer );
        if ( zip.open( QIODevice::ReadOnly ) ) {
            if ( const KZipFileEntry* entry = dynamic_cast<const KZipFileEntry*>( zip.directory()->entry( lang + QLatin1String( ".xml" ) ) ) ) {
                QScopedPointer<QIODevice> dev( entry->createDevice() );
                Series s = SeriesPrivate::parseSeries( m_usedMirrorUrl, dev.data() );
                if ( const KZipFileEntry* entry = dynamic_cast<const KZipFileEntry*>( zip.directory()->entry( QLatin1String( "banners.xml" ) ) ) ) {
                    QScopedPointer<QIODevice> bannersDev( entry->createDevice() );
                    s.d->parseBanners( m_usedMirrorUrl, bannersDev.data(), s );
                }
                m_multiResultCache << s;
                if(m_requests.isEmpty()) {
                    if(m_multiResultCache.count() == 1)
                        emit q->finished( s );
                    else
                        emit q->multipleResultsFound(m_multiResultCache);
                }
                else {
                    handleNextRequest();
                }
            }
            else {
                qDebug() << "Failed to access file in zip archive.";
                // TODO: report the error somehow
                emit q->finished( Series() );
            }
        }
        else {
            qDebug() << "Failed to open zip archive";
            // TODO: report the error somehow
            emit q->finished( Series() );
        }
    }     else {
        qDebug() << "Download failed";
        // TODO: report the error somehow
        emit q->finished( Series() );
    }
    // not using parent so we need to delete it
    downloader->deleteLater();

}

void Tvdb::Client::Private::getSeriesByNameResult(TVDBFileDownloader *downloader)
{
    if (downloader == NULL)
        return;

    TvdbRequest req = m_requests.dequeue();
    if (downloader->finished()) {
        QList<Series> series = parseSeriesList( downloader->data() );
        if( series.isEmpty() ) {
            qDebug() << "No results found";
            emit q->finished( Series() );
        }
        else if ( series.count() == 1 ) {
            qDebug() << "series.count()";
            q->getSeriesById( series.first().id() );
        }
        else {
            if( m_flags & Tvdb::Client::FetchFullDetails ) {
                qDebug() << "FetchFullDetails";
                fetchDetails(series);
            }
            else {
                qDebug() << "Multiple Reulsts found";
                emit q->multipleResultsFound( series );
            }
        }
    }
    else {
        qDebug() << "Download failed";
        // TODO: report the error somehow
        emit q->finished( Series() );
    }
    // not using parent so we need to delete it
    downloader->deleteLater();
}


Tvdb::Client::Private::Private(QString language)
    :_language("en")
{
    if (!_language.isEmpty())
        _language = language;
}

Tvdb::Client::Client(QString language, QObject* parent )
    : QObject( parent ),
      d( new Private(language) )
{
    d->q = this;
}


Tvdb::Client::~Client()
{
    delete d;
}


void Tvdb::Client::setApiKey( const QString& key )
{
    d->m_apiKey = key;
}


void Tvdb::Client::getSeriesByName( const QString& name )
{
    d->m_requests.enqueue( TvdbRequest( name ) );
    d->m_multiResultCache.clear();
    d->handleNextRequest();
}


void Tvdb::Client::getSeriesById( int id )
{
    d->m_requests.enqueue( TvdbRequest( id ) );
    d->handleNextRequest();
}

void Tvdb::Client::setFlags(TvdbFlags flags)
{
    d->m_flags = flags;
}

#include "client.moc"
