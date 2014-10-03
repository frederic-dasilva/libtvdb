
#include "url.h"


namespace Tvdb {

    Url::Url()
        :QUrl()
    { }

    Url::Url(const QString &url)
        :QUrl(url)
    { }

    Url::Url(const QUrl &other)
        :QUrl(other)
    { }

    void Url::addPath(const QString &txt)
    {
        // lol that works
        setUrl(toString() + "/" + txt);
    }

}
