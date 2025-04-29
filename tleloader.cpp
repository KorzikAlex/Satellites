#include "tleloader.h"

TleLoader::TleLoader(QObject *parent = nullptr)
    : QObject(parent)
{}

void TleLoader::loadFromUrl(const QUrl &url)
{
    QNetworkRequest request(url);
    network_manager_.get(request);
    connect(&network_manager_, &QNetworkAccessManager::finished, this, &TleLoader::onNetworkReply);
}

void TleLoader::onNetworkReply(QNetworkReply *reply)
{
    if (reply->error()) {
        emit errorOccurred(reply->errorString());
        return;
    }

    const QString data = QString::fromUtf8(reply->readAll());
    emit dataLoaded(parseTle(data));
}

// Сериализация/десериализация
QDataStream &operator<<(QDataStream &out, const TleData &data)
{
    return out << data.name_ << data.line1_ << data.line2_;
}

QDataStream &operator>>(QDataStream &in, TleData &data)
{
    return in >> data.name_ >> data.line1_ >> data.line2_;
}
