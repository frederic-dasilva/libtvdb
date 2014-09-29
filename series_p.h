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

#ifndef _TVDB_SERIES_P_H_
#define _TVDB_SERIES_P_H_

#include <QtCore/QSharedData>
#include <QtCore/QStringList>
#include <QtCore/QDate>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QUrl>

#include "season.h"
#include "banner.h"

namespace Tvdb {

    class Episode;

    class SeriesPrivate : public QSharedData
    {
    public:
        SeriesPrivate()
            : m_id( 0 ) {
        }

        int m_id;
        QString m_name;
        QString m_overview;
        QDate m_firstAired;
        QString m_imdbId;
        QStringList m_genres;
        QStringList m_actors;
        QList<Banner> m_banners;
        QHash<int, Season> m_seasons;
        QString m_language;

        /**
         * Adds an episode to the Series, properly creating
         * Seasons if necessary.
         */
        void addEpisode( int season, int seasonid, const Episode& episode );

        static Series parseSeries( const QUrl& mirrorUrl, QIODevice* dev );
        bool parseBanners( const QUrl& mirrorUrl, QIODevice* dev, Series &series );
    };
}

#endif
