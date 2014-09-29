/*
   This file is part of the Nepomuk KDE project.
   Copyright (C) 2012 Sebastian Trueg <trueg@kde.org>
   
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

#include "banner.h"
#include "banner_p.h"
#include "season.h"
#include "series.h"

#include <QDebug>

Tvdb::Banner::Banner()
    : d( new BannerPrivate() )
{
}

Tvdb::Banner::Banner(const Banner &other)
    : d( other.d )
{
}

Tvdb::Banner::~Banner()
{
}

Tvdb::Banner &Tvdb::Banner::operator =(const Tvdb::Banner &other)
{
    d = other.d;
    return *this;
}

int Tvdb::Banner::id() const
{
    return d->m_id;
}

Tvdb::Series Tvdb::Banner::series() const
{
    return d->m_series;
}

Tvdb::Season Tvdb::Banner::season() const
{
    return d->m_season;
}

QUrl Tvdb::Banner::bannerUrl() const
{
    return d->m_url;
}

QUrl Tvdb::Banner::thumbnailUrl() const
{
    return d->m_thumbUrl;
}

QUrl Tvdb::Banner::vignetteUrl() const
{
    return d->m_vignetteUrl;
}

double Tvdb::Banner::rating() const
{
    return d->m_rating;
}

int Tvdb::Banner::ratingCount() const
{
    return d->m_ratingCount;
}

bool Tvdb::Banner::containsSeriesName() const
{
    return d->m_containsSeriesName;
}

Tvdb::Banner::BannerType Tvdb::Banner::type() const
{
    return d->m_type;
}

QString Tvdb::Banner::language() const
{
    return d->m_language;
}

QDebug& Tvdb::operator<<( QDebug& dbg, const Banner& banner )
{
    QString s;
    switch(banner.type()) {
    case Banner::PosterBanner:
        s += QLatin1String("Poster");
        break;
    case Banner::FanartBanner:
        s += QLatin1String("Fanart");
        break;
    case Banner::SeasonBanner:
        s += QLatin1String("Season");
        break;
    default:
        s += QLatin1String("Graphical");
        break;
    }
    s += QLatin1String(" banner: ");
    s += banner.bannerUrl().toString();

    return dbg << s;
}
