

#ifndef _TVDB_URL_H_
#define _TVDB_URL_H_

#include <QUrl>

namespace Tvdb {

    class Url : public QUrl
    {
    public:
        Url();
        Url(const QString &url);
        Url(const QUrl &other);

        void addPath(const QString &txt);
    };
}

#endif
