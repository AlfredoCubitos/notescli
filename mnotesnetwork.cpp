#include "mnotesnetwork.h"


#define PATH QString("/index.php/apps/notes/api/v0.2/notes")

MNotesNetwork::MNotesNetwork()
{
    MNotesConnect();

}

void MNotesNetwork::initConnect(const QHash<QString,QString> params)
{

    account = params;
  //  qDebug() << params;
    MNotesRequestAuth();

}

void MNotesNetwork::MNotesConnect()
{
    manager = new QNetworkAccessManager(this);
    request.setSslConfiguration( QSslConfiguration::defaultConfiguration());
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void MNotesNetwork::MNotesRequestAuth()
{
    QByteArray auth;
   // qDebug() << "Auth: " << account["user"];
    auth.append(account["user"]);
    auth.append(":");
    auth.append(account["password"]);
    request.setRawHeader("User-Agent", "MNotes");
    request.setRawHeader("Authorization",  "Basic " + auth.toBase64());
}

void MNotesNetwork::getMNotesJson(const QByteArray data)
{
    //   qDebug() << data;
    //  qDebug() << "Req:" << request.rawHeader("Authorization");
    request.setRawHeader("Content-type", "application/json");
    manager->post(request,data);

}

void MNotesNetwork::getMnotes(const QString param)
{
    QString uri = account["url"];
   // qDebug() << "URI: " << uri;
    uri.append(PATH);
    if (!param.isEmpty())
    {
        uri.append("/");
        uri.append(param);
    }
   qDebug() << "URI: " << uri;
    request.setUrl(QUrl(uri));
    m_getRequest();
}

void MNotesNetwork::delMnote(const QString url)
{
    QString uri = account["url"];
    uri.append(url);
    request.setUrl(QUrl(uri));
    m_deleteRequest();
}

void MNotesNetwork::updateMnote(const QString url, const QByteArray data)
{
    QString uri = account["url"];
    uri.append(url);
    request.setUrl(QUrl(uri));
    request.setRawHeader("Content-Type","application/json");
    m_putRequest(data);
}

void MNotesNetwork::newMnote( const QByteArray data)
{
    QString uri = account["url"];
    uri.append(PATH);
    request.setUrl(QUrl(uri));
    request.setRawHeader("Content-Type","application/json");
    m_postRequest(data);
}

void MNotesNetwork::clearNetwork()
{
    manager->clearAccessCache();
    manager->clearConnectionCache();
}

void MNotesNetwork::m_getRequest()
{
     manager->get(request);
}

void MNotesNetwork::m_postRequest(const QByteArray data)
{
    manager->post(request,data);
}
void MNotesNetwork::m_putRequest(const QByteArray data)
{
    manager->put(request,data);
}
void MNotesNetwork::m_deleteRequest()
{
    manager->deleteResource(request);
}

/***
 * ### SLOTS ###
 * **/
void MNotesNetwork::replyFinished(QNetworkReply *reply)
    {
        if(reply->error())
            {
                qDebug() << "ERROR!";
                qDebug() << reply->errorString();
                qDebug() << reply->rawHeaderList();
                return;
            }
     //   QString ctype = reply->rawHeader("Content-Type");
        QByteArray data = reply->readAll();

      // result = QString::fromUtf8( data);
      // qDebug() << "Data: " << QString::fromUtf8( data);

       emit resultAvailable(data);

     //  qDebug() <<  QString::fr( data);
        reply->deleteLater();
    }
