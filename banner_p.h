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

#ifndef _TVDB_BANNER_P_H_
#define _TVDB_BANNER_P_H_

#include "series.h"
#include "season.h"
#include "banner.h"

#include <QtCore/QSharedData>
#include <QtCore/QUrl>


namespace Tvdb {
    class BannerPrivate : public QSharedData
    {
    public:
        BannerPrivate()
            : m_id(0),
              m_rating(0),
              m_ratingCount(0),
              m_containsSeriesName(false),
              m_type(Banner::PosterBanner) {
        }

        int m_id;
        QUrl m_url;
        QUrl m_thumbUrl;
        QUrl m_vignetteUrl;
        double m_rating;
        int m_ratingCount;
        bool m_containsSeriesName;
        Banner::BannerType m_type;
        Series m_series;
        Season m_season;
        QString m_language;
    };
}

#endif
