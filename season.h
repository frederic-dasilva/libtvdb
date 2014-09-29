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

#ifndef _TVDB_SEASON_H_
#define _TVDB_SEASON_H_

#include <QtCore/QList>
#include <QtCore/QSharedDataPointer>

#include "episode.h"
#include "tvdb_export.h"

class QDebug;

namespace Tvdb {

    class Banner;
    class SeasonPrivate;

    /**
     * \class Season season.h tvdb/season.h
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class TVDB_EXPORT Season
    {
    public:
        Season();
        Season( const Season& other );
        ~Season();

        Season& operator=( const Season& other );

        bool isValid() const;

        int id() const;

        Series series() const;
        int seasonNumber() const;

        int numEpisodes() const;

        bool hasEpisode(int episode) const;

        QList<Episode> episodes() const;

        QList<Banner> banners() const;

        /**
         * Get an episode by episode number. Be aware that
         * the index starts at 1.
         */
        Episode operator[]( int number ) const;

    private:
        QExplicitlySharedDataPointer<SeasonPrivate> d;

        friend class SeriesPrivate;
    };

    TVDB_EXPORT QDebug& operator<<( QDebug& dbg, const Season& season );
}

#endif
