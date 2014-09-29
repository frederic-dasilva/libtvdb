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

#ifndef _TVDB_CLIENT_H_
#define _TVDB_CLIENT_H_

#include <QtCore/QObject>
#include <QtCore/QList>

#include "tvdb_export.h"
#include "series.h"

#include <QUrl>

class TVDBFileDownloader;

class FUrl : public QUrl
{
public:
    FUrl()
        :QUrl()
    {

    }

    FUrl(const QString &url)
        :QUrl(url)
    {

    }

    FUrl(const QUrl &other)
        :QUrl(other)
    {

    }

    void addPath(const QString &txt)
    {
        // lol that works
        setUrl(toString() + "/" + txt);
    }
};

namespace Tvdb {
    /**
     * \class Client client.h tvdb/client.h
     *
     * \brief An asyncronous client to the thetvdb.com API.
     *
     * Using the client is very easy: Create a new instance,
     * connect to the finished() and the multipleResultsFound()
     * signals and then call getSeriesByName() or getSeriesById().
     * The result will be an instance of Series which contains
     * all the information fetched from the tvdb web service.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class TVDB_EXPORT Client : public QObject
    {
        Q_OBJECT

    public:
        /**
         * Create a new client.
         */
        Client( QObject* parent = 0 );

        /**
         * Destructor.
         */
        ~Client();

        enum TvdbFlag {
            NoFlags = 0,
            /// always fetch the full details of all series, even if there are multiple matches
            FetchFullDetails = 1
        };
        Q_DECLARE_FLAGS(TvdbFlags, TvdbFlag)

    public Q_SLOTS:
        /**
         * Set a custom thetvdb API key.
         */
        void setApiKey( const QString& key );

        /**
         * Set flags to be used in the client.
         */
        void setFlags(TvdbFlags flags);

        /**
         * Search a series by name. This will result in either
         * a finished() signal or a multipleResultsFound() signal
         * to be emitted.
         *
         * \param name The name of the series or part of it.
         */
        void getSeriesByName( const QString& name );

        /**
         * Get a series by id. This will result in the finished()
         * signal being emitted.
         */
        void getSeriesById( int id );

    Q_SIGNALS:
        /**
         * Emitted once a series has been found. In case of an error
         * an empty series is returned.
         */
        void finished( const Tvdb::Series& series );

        /**
         * Emitted if multiple matches have been found in a getSeriesByName call.
         * \param series A list of matching series without and season or episode information.
         * Use Tvdb::Series::id() in getSeriesById() to get the final Series data. This can be
         * done automatically by specifying the FetchFullDetails flag.
         */
        void multipleResultsFound( const QList<Tvdb::Series>& series );

    private:
        class Private;
        Private* const d;

        Q_PRIVATE_SLOT( d, void getMirrorListResult(TVDBFileDownloader*))
        Q_PRIVATE_SLOT( d, void getSeriesByIdResult(TVDBFileDownloader*))
        Q_PRIVATE_SLOT( d, void getSeriesByNameResult(TVDBFileDownloader*))
    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Tvdb::Client::TvdbFlags)

#endif
