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

#include "episode.h"
#include "episode_p.h"
#include "season.h"
#include "series.h"

#include <QtCore/QSharedData>
#include <QtCore/QUrl>
#include <QtCore/QString>
#include <QtCore/QDebug>


Tvdb::Episode::Episode()
    : d( new EpisodePrivate() )
{
}


Tvdb::Episode::Episode( const Episode& other )
    : d( other.d )
{
}


Tvdb::Episode::~Episode()
{
}


Tvdb::Episode& Tvdb::Episode::operator=( const Episode& other )
{
    d = other.d;
    return *this;
}

bool Tvdb::Episode::isValid() const
{
    return d->m_season.isValid() && d->m_series.isValid();
}


int Tvdb::Episode::id() const
{
    return d->m_id;
}


Tvdb::Series Tvdb::Episode::series() const
{
    return d->m_series;
}


Tvdb::Season Tvdb::Episode::season() const
{
    return d->m_season;
}


QString Tvdb::Episode::imdbId() const
{
    return d->m_imdbId;
}


QUrl Tvdb::Episode::imdbUrl() const
{
    if ( d->m_imdbId.isEmpty() )
        return QUrl();
    else
        return QUrl( QLatin1String( "http://www.imdb.com/title/" ) + d->m_imdbId );
}


QString Tvdb::Episode::name() const
{
    return d->m_name;
}


QString Tvdb::Episode::overview() const
{
    return d->m_overview;
}


QDate Tvdb::Episode::firstAired() const
{
    return d->m_firstAired;
}


int Tvdb::Episode::episodeNumber() const
{
    return d->m_episodeNumer;
}

QStringList Tvdb::Episode::guestStars() const
{
    return d->m_guestStars;
}

QStringList Tvdb::Episode::writers() const
{
    return d->m_writers;
}

QString Tvdb::Episode::director() const
{
    return d->m_director;
}


QDebug& Tvdb::operator<<( QDebug& dbg, const Episode& episode )
{
    QString s = QString::fromLatin1( "%1x%2 - %3" )
                .arg( QString::number( episode.season().seasonNumber() ).rightJustified( 2, '0' ),
                      QString::number( episode.episodeNumber() ).rightJustified( 2, '0' ),
                      episode.name() );
    if ( episode.firstAired().isValid() )
        s.append( QString::fromLatin1( " (%4)" )
                  .arg( QString::number( episode.firstAired().year() ) ) );
    dbg << s << endl;
    return dbg;
}
