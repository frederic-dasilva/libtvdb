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

#include "tvdbclient.h"

#include "client.h"
#include "series.h"

#include <QtCore/QStringList>
#include <QtCore/QHash>
#include <QtCore/QDate>

#include <QtGui/QApplication>
#include <QtGui/QInputDialog>
#include <QtGui/QFontMetrics>
#include <QtGui/QMessageBox>
#include <QtGui/QTextBrowser>

#include <KApplication>
#include <KDebug>
#include <KDialog>
#include <KCmdLineArgs>
#include <KAboutData>

namespace {
    QString seriesToHtml( const Tvdb::Series& series ) {
        QString html;
        QTextStream s( &html );

        // series title
        s << QLatin1String( "<p><strong>" ) << series.name();
        if ( series.firstAired().isValid() )
            s << QString::fromLatin1( " (%1)" ).arg( series.firstAired().year() );
        s << QLatin1String( "</strong></p>" );

        // series overview
        s << QLatin1String( "<p><i>" ) + series.overview() + QLatin1String( "</i></p>" );

        // episodes
        Q_FOREACH( const Tvdb::Season& season, series.seasons() ) {
            s << QLatin1String( "<p>Season " ) << season.seasonNumber() << QLatin1String( "<br>" );
            Q_FOREACH( const Tvdb::Episode& episode, season.episodes() ) {
                s << QString::number( episode.episodeNumber() ).rightJustified( 2, '0' ) << QLatin1String( " - " ) << episode.name();
                if ( episode.firstAired().isValid() )
                    s << QLatin1String( " (" ) << episode.firstAired().toString() << ')';
                s << QLatin1String( "<br>" );
            }
        }

        return html;
    }
}

TvdbClient::TvdbClient( QObject* parent )
    : QObject( parent )
{
    m_client = new Tvdb::Client("fr", this );
    connect( m_client, SIGNAL( finished( Tvdb::Series ) ), SLOT( slotFinished( Tvdb::Series ) ) );
    connect( m_client, SIGNAL( multipleResultsFound( QList<Tvdb::Series> ) ), SLOT( slotMultipleResultsFound( QList<Tvdb::Series> ) ) );
}


TvdbClient::~TvdbClient()
{
}


void TvdbClient::lookup( const QString& s )
{
    m_client->getSeriesByName( s );
}


void TvdbClient::slotFinished( const Tvdb::Series& series )
{
    kDebug() << "Done:" << series;
    if ( series.isValid() ) {
        KDialog dlg;
        dlg.setButtons( KDialog::Ok );
        dlg.setCaption( QLatin1String( "Found Series" ) );
        QTextBrowser* browser = new QTextBrowser( &dlg );
        dlg.setMainWidget( browser );
        browser->setText( seriesToHtml( series ) );
        dlg.exec();
    }
    else {
        QMessageBox::information( 0, QLatin1String( "No Series Found" ), QLatin1String( "Sorry, nothing found. Either no series matched the query term of there was an error (for which there is no handling yet)" ) );
    }
    qApp->quit();
}


void TvdbClient::slotMultipleResultsFound( const QList<Tvdb::Series>& series )
{
    QStringList seriesTitles;
    Q_FOREACH( const Tvdb::Series& s, series ) {
        QString title = s.name();
        if ( s.firstAired().isValid() )
            title.append( QString::fromLatin1( " (%1)" ).arg( s.firstAired().year() ) );
        title.append( QLatin1String( " - " ) );
        title.append( qApp->fontMetrics().elidedText( s.overview(), Qt::ElideRight, 300 ) );
        seriesTitles.append( title );
    }

    int i = seriesTitles.indexOf( QInputDialog::getItem( 0,
                                                         QLatin1String( "Multiple Matches" ),
                                                         QLatin1String( "Multiple matching series found. Please select one" ),
                                                         seriesTitles,
                                                         0,
                                                         false ) );
    if ( i >= 0 ) {
        kDebug() << "Getting" << series[i];
        m_client->getSeriesById( series[i].id() );
    }
    else {
        qApp->quit();
    }
}


int main( int argc, char **argv )
{
    KAboutData aboutData( "tvdbclient",
                          0,
                          ki18n("Simple LibTvdb Client"),
                          "1.0",
                          ki18n("A Simple LibTvdb Client which allows to lookup TV series"),
                          KAboutData::License_GPL,
                          ki18n("(c) 2008-2011, Sebastian Trüg"),
                          KLocalizedString(),
                          "http://libtvdb.sourceforge.net" );
    aboutData.addAuthor(ki18n("Sebastian Trüg"), ki18n("Maintainer"), "trueg@kde.org");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KCmdLineOptions options;
    options.add("+[query]", ki18n("The query to run through libtvdb - typically the name of a show."));
    KCmdLineArgs::addCmdLineOptions( options );
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

    KApplication app;
    QApplication::setQuitOnLastWindowClosed( false );

    TvdbClient t;

    QString s;
    if ( args->count() > 0 )
        s = args->arg(0);
    else
        s = QInputDialog::getText( 0, QLatin1String( "Series Search" ), QLatin1String( "Please enter a search term for the series you are looking for:" ) );

    if ( !s.isEmpty() ) {
        t.lookup( s );
        return app.exec();
    }
}

#include "tvdbclient.moc"
