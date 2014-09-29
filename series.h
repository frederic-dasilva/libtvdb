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

#ifndef _TVDB_SERIES_H_
#define _TVDB_SERIES_H_

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>

#include "tvdb_export.h"
#include "season.h"

class QUrl;
class QStringList;
class QIODevice;
class QDate;
class QDebug;

namespace Tvdb {

    class Episode;
    class SeriesPrivate;

    /**
     * \class Series series.h tvdb/series.h
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class TVDB_EXPORT Series
    {
    public:
        Series();
        Series( int id, const QString& name, const QString& overview = QString() );
        Series( const Series& other );
        ~Series();

        Series& operator=( const Series& other );

        bool isValid() const;

        int id() const;

        QString imdbId() const;
        QUrl imdbUrl() const;

        QString name() const;
        QString overview() const;

        QDate firstAired() const;

        QStringList genres() const;
        QStringList actors() const;

        int numSeasons() const;

        bool hasSeason(int season) const;

        QList<Season> seasons() const;

        /**
         * Get a Season by its number.
         * \param number The season number. 0 might be used to group
         * special episodes which do not fit into any regular season.
         */
        Season operator[]( int number ) const;

        /**
         * All banners including the ones for the seasons.
         */
        QList<Banner> banners() const;

        /**
         * A list of URLs to banner artwork.
         */
        QList<QUrl> bannerUrls() const;

        QList<QUrl> posterUrls() const;

        QString language() const;

    private:
        QExplicitlySharedDataPointer<SeriesPrivate> d;

        friend class SeriesPrivate;
        friend class Client;
    };

    TVDB_EXPORT QDebug& operator<<( QDebug& dbg, const Series& series );
}

#endif
