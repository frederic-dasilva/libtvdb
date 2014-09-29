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

#ifndef TVDB_BANNER_H
#define TVDB_BANNER_H

#include <QtCore/QSharedDataPointer>

#include "tvdb_export.h"

class QUrl;

namespace Tvdb {

    class Series;
    class Season;
    class BannerPrivate;

    class TVDB_EXPORT Banner
    {
    public:
        Banner();
        Banner( const Banner& other );
        ~Banner();

        Banner& operator=( const Banner& other );

        enum BannerType {
            PosterBanner,
            GraphicalBanner,
            FanartBanner,
            SeasonBanner
        };

        int id() const;

        Series series() const;
        Season season() const;
        QUrl bannerUrl() const;
        QUrl thumbnailUrl() const;
        QUrl vignetteUrl() const;
        double rating() const;
        int ratingCount() const;
        bool containsSeriesName() const;
        BannerType type() const;
        QString language() const;

    private:
        QExplicitlySharedDataPointer<BannerPrivate> d;

        friend class SeriesPrivate;
    };

    TVDB_EXPORT QDebug& operator<<( QDebug& dbg, const Banner& banner );
}

#endif
