/*
   Copyright (C) 2010-2012 Sebastian Trueg <trueg@kde.org>

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

#include "series.h"
#include "series_p.h"
#include "season.h"
#include "season_p.h"
#include "episode.h"
#include "episode_p.h"
#include "banner.h"
#include "banner_p.h"
#include "url.h"
#include <QtCore/QSharedData>
#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QDate>
#include <QtCore/QHash>

#include <QDebug>

namespace {
    QUrl buildBannerUrl(const QUrl& mirrorUrl, const QString& path) {
        Tvdb::Url url(mirrorUrl);
        url.addPath(QLatin1String("/banners"));
        url.addPath(path);
        return url;
    }

    Tvdb::Banner::BannerType convertBannerType( const QString& type ) {
        if(type == QLatin1String("fanart")) {
            return Tvdb::Banner::FanartBanner;
        }
        else if(type == QLatin1String("poster")) {
            return Tvdb::Banner::PosterBanner;
        }
        else if(type == QLatin1String("season")) {
            return Tvdb::Banner::SeasonBanner;
        }
        else {
            return Tvdb::Banner::GraphicalBanner;
        }
    }
}

void Tvdb::SeriesPrivate::addEpisode( int season, int seasonid, const Episode& episode )
{
    m_seasons[season].d->m_series.d = this;
    m_seasons[season].d->m_id = seasonid;
    m_seasons[season].d->m_seasonNumber = season;
    m_seasons[season].d->addEpisode( episode );
    m_seasons[season].d->m_episodes[episode.episodeNumber()].d->m_season = m_seasons[season];
    m_seasons[season].d->m_episodes[episode.episodeNumber()].d->m_series.d = this;
}


Tvdb::Series Tvdb::SeriesPrivate::parseSeries( const QUrl& mirrorUrl, QIODevice* s )
{
    Q_UNUSED(mirrorUrl);

    Series series;

    QXmlStreamReader xml( s );
    if ( xml.readNextStartElement() ) { // dive into the "Data" element
        while ( xml.readNextStartElement() ) {
            if ( xml.name() == QLatin1String( "Series" ) ) {
                // parse series details
                while ( xml.readNextStartElement() ) {
                    if ( xml.name() == QLatin1String( "id" ) ) {
                        series.d->m_id = xml.readElementText().toInt();
                    }
                    else if ( xml.name() == QLatin1String( "FirstAired" ) ) {
                        series.d->m_firstAired = QDate::fromString( xml.readElementText(), Qt::ISODate );
                    }
                    else if ( xml.name() == QLatin1String( "Actors" ) ) {
                        series.d->m_actors = xml.readElementText().split( '|', QString::SkipEmptyParts );
                    }
                    else if ( xml.name() == QLatin1String( "Genre" ) ) {
                        series.d->m_genres = xml.readElementText().split( '|', QString::SkipEmptyParts );
                    }
                    else if ( xml.name() == QLatin1String( "Overview" ) ) {
                        series.d->m_overview = xml.readElementText();
                    }
                    else if ( xml.name() == QLatin1String( "SeriesName" ) ) {
                        series.d->m_name = xml.readElementText();
                    }
                    else if ( xml.name() == QLatin1String( "IMDB_ID" ) ) {
                        series.d->m_imdbId = xml.readElementText();
                    }
                    else if( xml.name() == QLatin1String("Language") ) {
                        series.d->m_language = xml.readElementText();
                    }
                    else {
                        xml.skipCurrentElement();
                    }
                }
            }
            else if ( xml.name() == QLatin1String( "Episode" ) ) {
                Episode episode;
                int seasonNumber = 0;
                int seasonid = 0;
                while ( xml.readNextStartElement() ) {
                    if ( xml.name() == QLatin1String( "id" ) ) {
                        episode.d->m_id = xml.readElementText().toInt();
                    }
                    else if ( xml.name() == QLatin1String( "FirstAired" ) ) {
                        episode.d->m_firstAired = QDate::fromString( xml.readElementText(), Qt::ISODate );
                    }
                    else if ( xml.name() == QLatin1String( "IMDB_ID" ) ) {
                        episode.d->m_imdbId = xml.readElementText();
                    }
                    else if ( xml.name() == QLatin1String( "EpisodeName" ) ) {
                        episode.d->m_name = xml.readElementText();
                    }
                    else if ( xml.name() == QLatin1String( "EpisodeNumber" ) ) {
                        episode.d->m_episodeNumer = xml.readElementText().toInt();
                    }
                    else if ( xml.name() == QLatin1String( "Overview" ) ) {
                        episode.d->m_overview = xml.readElementText();
                    }
                    else if ( xml.name() == QLatin1String( "GuestStars" ) ) {
                        episode.d->m_guestStars = xml.readElementText().split( '|', QString::SkipEmptyParts );
                    }
                    else if ( xml.name() == QLatin1String( "Director" ) ) {
                        episode.d->m_director = xml.readElementText();
                    }
                    else if ( xml.name() == QLatin1String( "Writer" ) ) {
                        episode.d->m_writers = xml.readElementText().split( '|', QString::SkipEmptyParts );
                    }
                    else if ( xml.name() == QLatin1String( "SeasonNumber" ) ) {
                        seasonNumber = xml.readElementText().toInt();
                    }
                    else if ( xml.name() == QLatin1String( "seasonid" ) ) {
                        seasonid = xml.readElementText().toInt();
                    }
                    else {
                        xml.skipCurrentElement();
                    }
                }
                series.d->addEpisode( seasonNumber, seasonid, episode );
            }
            else {
                // skip the data we do not handle
                xml.skipCurrentElement();
            }
        }
    }

    return series;
}

bool Tvdb::SeriesPrivate::parseBanners(const QUrl &mirrorUrl, QIODevice *dev, Series& series)
{
    QXmlStreamReader xml( dev );
    if ( xml.readNextStartElement() ) { // dive into the "Banners" element
        while ( xml.readNextStartElement() ) {
            if ( xml.name() == QLatin1String( "Banner" ) ) {
                Banner banner;
                // parse series details
                while ( xml.readNextStartElement() ) {
                    if ( xml.name() == QLatin1String( "id" ) ) {
                        banner.d->m_id = xml.readElementText().toInt();
                    }
                    else if ( xml.name() == QLatin1String( "Rating" ) ) {
                        banner.d->m_rating = xml.readElementText().toDouble();
                    }
                    else if ( xml.name() == QLatin1String( "RatingCount" ) ) {
                        banner.d->m_ratingCount = xml.readElementText().toInt();
                    }
                    else if ( xml.name() == QLatin1String( "SeriesName" ) ) {
                        banner.d->m_containsSeriesName = xml.readElementText().toLower() == QLatin1String("true");
                    }
                    else if ( xml.name() == QLatin1String( "Season" ) ) {
                        const int season = xml.readElementText().toInt();
                        if(series.d->m_seasons.contains(season)) {
                            series[season].d->m_banners.append(banner);
                            banner.d->m_season = series[season];
                        }
                    }
                    else if ( xml.name() == QLatin1String( "BannerType" ) ) {
                        banner.d->m_type = convertBannerType(xml.readElementText().toLower());
                    }
                    else if( xml.name() == QLatin1String("BannerPath") ) {
                        banner.d->m_url = buildBannerUrl(mirrorUrl, xml.readElementText());
                    }
                    else if( xml.name() == QLatin1String("ThumbnailPath") ) {
                        banner.d->m_thumbUrl = buildBannerUrl(mirrorUrl, xml.readElementText());
                    }
                    else if( xml.name() == QLatin1String("VignettePath") ) {
                        banner.d->m_vignetteUrl = buildBannerUrl(mirrorUrl, xml.readElementText());
                    }
                    else if( xml.name() == QLatin1String("Language") ) {
                        banner.d->m_language = xml.readElementText();
                    }
                    else {
                        xml.skipCurrentElement();
                    }
                }
                banner.d->m_series = series;
                series.d->m_banners.append(banner);
            }
            else {
                // skip the data we do not handle
                xml.skipCurrentElement();
            }
        }
    }

    return true;
}


Tvdb::Series::Series()
    : d( new SeriesPrivate() )
{
}


Tvdb::Series::Series( int id, const QString& name, const QString& overview )
    : d( new SeriesPrivate() )
{
    d->m_id = id;
    d->m_name = name;
    d->m_overview = overview;
}


Tvdb::Series::Series( const Series& other )
    : d( other.d )
{
}


Tvdb::Series::~Series()
{
}


Tvdb::Series& Tvdb::Series::operator=( const Series& other )
{
    d = other.d;
    return *this;
}


bool Tvdb::Series::isValid() const
{
    return d->m_id > 0 && !d->m_name.isEmpty();
}


int Tvdb::Series::id() const
{
    return d->m_id;
}


QString Tvdb::Series::imdbId() const
{
    return d->m_imdbId;
}


QUrl Tvdb::Series::imdbUrl() const
{
    if ( d->m_imdbId.isEmpty() )
        return QUrl();
    else
        return QUrl( QLatin1String( "http://www.imdb.com/title/" ) + d->m_imdbId );
}


QString Tvdb::Series::name() const
{
    return d->m_name;
}


QString Tvdb::Series::overview() const
{
    return d->m_overview;
}


QDate Tvdb::Series::firstAired() const
{
    return d->m_firstAired;
}


QStringList Tvdb::Series::genres() const
{
    return d->m_genres;
}


QStringList Tvdb::Series::actors() const
{
    return d->m_actors;
}


int Tvdb::Series::numSeasons() const
{
    return d->m_seasons.count();
}

bool Tvdb::Series::hasSeason(int season) const
{
    return d->m_seasons.contains(season);
}


namespace {
    bool seasonNumberLessThan( const Tvdb::Season& e1, const Tvdb::Season& e2 ) {
        return e1.seasonNumber() < e2.seasonNumber();
    }
}

QList<Tvdb::Season> Tvdb::Series::seasons() const
{
    QList<Tvdb::Season> seasons = d->m_seasons.values();
    qSort( seasons.begin(), seasons.end(), seasonNumberLessThan );
    return seasons;
}


Tvdb::Season Tvdb::Series::operator[]( int i ) const
{
    return d->m_seasons[i];
}

QList<Tvdb::Banner> Tvdb::Series::banners() const
{
    return d->m_banners;
}

QList<QUrl> Tvdb::Series::bannerUrls() const
{
    QList<QUrl> l;
    foreach(const Banner& banner, d->m_banners) {
        if(banner.type() == Banner::GraphicalBanner) {
            l << banner.bannerUrl();
        }
    }

    return l;
}

QList<QUrl> Tvdb::Series::posterUrls() const
{
    QList<QUrl> l;
    foreach(const Banner& banner, d->m_banners) {
        if(banner.type() == Banner::PosterBanner) {
            l << banner.bannerUrl();
        }
    }

    return l;
}

QString Tvdb::Series::language() const
{
    return d->m_language;
}


QDebug& Tvdb::operator<<( QDebug& dbg, const Series& series )
{
    dbg << QString::fromLatin1( "%1 (%2)" ).arg( series.name() ).arg( series.firstAired().year() ) << endl
        << series.seasons() << endl
        << series.banners();
    return dbg;
}
