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

#include "season.h"
#include "season_p.h"
#include "episode.h"
#include "banner.h"

#include <QtCore/QSharedData>
#include <QtCore/QList>
#include <QtCore/QDebug>


void Tvdb::SeasonPrivate::addEpisode( const Episode& episode )
{
    m_episodes[episode.episodeNumber()] = episode;
}


Tvdb::Season::Season()
    : d( new SeasonPrivate() )
{
}


Tvdb::Season::Season( const Season& other )
    : d( other.d )
{
}


Tvdb::Season::~Season()
{
}


Tvdb::Season& Tvdb::Season::operator=( const Season& other )
{
    d = other.d;
    return *this;
}

bool Tvdb::Season::isValid() const
{
    return d->m_series.isValid();
}


int Tvdb::Season::id() const
{
    return d->m_id;
}


Tvdb::Series Tvdb::Season::series() const
{
    return d->m_series;
}


int Tvdb::Season::seasonNumber() const
{
    return d->m_seasonNumber;
}


int Tvdb::Season::numEpisodes() const
{
    return d->m_episodes.count();
}

bool Tvdb::Season::hasEpisode(int episode) const
{
    return d->m_episodes.contains(episode);
}


namespace {
    bool episodeNumberLessThan( const Tvdb::Episode& e1, const Tvdb::Episode& e2 ) {
        return e1.episodeNumber() < e2.episodeNumber();
    }
}

QList<Tvdb::Episode> Tvdb::Season::episodes() const
{
    QList<Tvdb::Episode> episodes = d->m_episodes.values();
    qSort( episodes.begin(), episodes.end(), episodeNumberLessThan );
    return episodes;
}

QList<Tvdb::Banner> Tvdb::Season::banners() const
{
    return d->m_banners;
}


Tvdb::Episode Tvdb::Season::operator[]( int i ) const
{
    return d->m_episodes[i];
}


QDebug& Tvdb::operator<<( QDebug& dbg, const Season& season )
{
    dbg << QString::fromLatin1( "Season %1" ).arg( season.seasonNumber() ) << endl
        << season.episodes()
        << endl;
    return dbg;
}
