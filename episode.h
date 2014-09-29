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

#ifndef _TVDB_EPISODE_H_
#define _TVDB_EPISODE_H_

#include <QtCore/QSharedDataPointer>

#include "tvdb_export.h"

class QUrl;
class QString;
class QStringList;
class QDate;
class QDebug;

namespace Tvdb {
    class Series;
    class Season;
    class EpisodePrivate;

    /**
     * \class Episode episode.h tvdb/episode.h
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class TVDB_EXPORT Episode
    {
    public:
        Episode();
        Episode( const Episode& other );
        ~Episode();

        Episode& operator=( const Episode& other );

        bool isValid() const;

        int id() const;

        Series series() const;
        Season season() const;

        QString imdbId() const;
        QUrl imdbUrl() const;

        QString name() const;
        QString overview() const;

        QDate firstAired() const;

        int episodeNumber() const;

        QStringList guestStars() const;

        QStringList writers() const;
        QString director() const;

    private:
        QExplicitlySharedDataPointer<EpisodePrivate> d;

        friend class SeriesPrivate;
    };

    TVDB_EXPORT QDebug& operator<<( QDebug& dbg, const Episode& episode );
}

#endif
