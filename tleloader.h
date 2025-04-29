#ifndef TLELOADER_H
#define TLELOADER_H

#include <QList>
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>

#include "tledata.h"

class TleLoader : public QObject
{
    Q_OBJECT
public:
    explicit TleLoader(QObject *parent = nullptr);
    void loadFromUrl(const QUrl &url);
    void loadFromFile(const QString &filename);
    void saveToFile(const QString &filename);

signals:
    void dataLoaded(const QList<QString> &data);
    void errorOccurred(const QString &error);

private slots:
    void onNetworkReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *network_manager_;
    QList<TleData> parseTle(const QString &data);
};

#endif // TLELOADER_H
