#ifndef MNOTESNETWORK_H
#define MNOTESNETWORK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>



class MNotesNetwork : public QObject
{
    Q_OBJECT

public:
    explicit MNotesNetwork();


   void getMNotesJson(const QByteArray data);
   void getMnotes(const QString param="");
   void delMnote(const QString url);
   void updateMnote(const QString url, const QByteArray data);
   void newMnote(const QByteArray data);
   void initConnect(const QHash<QString, QString> params);
   void clearNetwork();

 //  QString result;
signals:
     void resultAvailable(const QByteArray &result);

private:
     void MNotesConnect();
     void MNotesRequestAuth();
     void m_getRequest();
     void m_postRequest(const QByteArray data);
     void m_putRequest(const QByteArray data);
     void m_deleteRequest();

     QHash<QString,QString> account;
     QNetworkAccessManager *manager;
     QNetworkRequest request;
     QByteArray m_reqData;
     QString method;

signals:



public slots:
    void replyFinished(QNetworkReply *reply);

};

#endif // MNOTESNETWORK_H
