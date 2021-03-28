#include "client.h"
#include "chatwin.h"


Client::Client(QObject *parent, unsigned user_id)
    : QObject(parent)
    , m_clientSocket(new QTcpSocket(this))
    , m_loggedIn(false)
    , m_user_id(user_id)
{
    connect(m_clientSocket, &QTcpSocket::connected,     this, &Client::connected);
    connect(m_clientSocket, &QTcpSocket::disconnected,  this, &Client::disconnected);
    connect(m_clientSocket, &QTcpSocket::readyRead,     this, &Client::onReadyRead);
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Client::error);
    connect(m_clientSocket, &QTcpSocket::disconnected,  this, [this]()->void{m_loggedIn = false;});
}


void Client::login(const QString &userName)
{
    if (m_clientSocket->state() == QAbstractSocket::ConnectedState) {
        QDataStream clientStream(m_clientSocket);
        QJsonObject msg;
        msg[QStringLiteral("type")] = QStringLiteral("login");
        msg[QStringLiteral("username")] = userName;
        clientStream << QJsonDocument(msg).toJson(QJsonDocument::Compact);
    }
}


void Client::sendMessage(const QString &text)
{
    if (text.isEmpty())
        return;
    QDataStream clientStream(m_clientSocket);
    QJsonObject message;
    QDateTime dt = QDateTime::currentDateTime();
    QString datetime = dt.toString(Qt::ISODate);
    datetime[10] = ' ';
    message[QStringLiteral("type")] = QStringLiteral("message");
    message[QStringLiteral("text")] = text;
    message[QStringLiteral("user-id")] = QString::number(m_user_id);
    message[QStringLiteral("datetime")] = datetime;
    clientStream << QJsonDocument(message).toJson();
}


void Client::disconnectFromHost()
{
    m_clientSocket->disconnectFromHost();
}


void Client::jsonReceived(const QJsonObject &docObj)
{
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    if (typeVal.isNull() || !typeVal.isString())
        return;
    if (!typeVal.toString().compare(QLatin1String("login"), Qt::CaseInsensitive)) {
        if (m_loggedIn)
            return;
        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
        if (resultVal.isNull() || !resultVal.isBool())
            return;
        const bool loginSuccess = resultVal.toBool();
        if (loginSuccess) {
            emit loggedIn();
            return;
        }
        const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
        emit loginError(reasonVal.toString());
    } else if (!typeVal.toString().compare(QLatin1String("message"), Qt::CaseInsensitive)) {
        const QJsonValue textVal = docObj.value(QLatin1String("text"));
        const QJsonValue senderVal = docObj.value(QLatin1String("sender"));
        if (textVal.isNull() || !textVal.isString())
            return;
        if (senderVal.isNull() || !senderVal.isString())
            return;
        emit messageReceived(senderVal.toString(), textVal.toString());
    } else if (!typeVal.toString().compare(QLatin1String("newuser"), Qt::CaseInsensitive)) {
        const QJsonValue usernameVal = docObj.value(QLatin1String("username"));
        if (usernameVal.isNull() || !usernameVal.isString())
            return;
        emit userJoined(usernameVal.toString());
    } else if (!typeVal.toString().compare(QLatin1String("userdisconnected"), Qt::CaseInsensitive)) {
        const QJsonValue usernameVal = docObj.value(QLatin1String("username"));
        if (usernameVal.isNull() || !usernameVal.isString())
            return;
        emit userLeft(usernameVal.toString());
    }
}


void Client::connectToServer(const QHostAddress &address, quint16 port)
{
    m_clientSocket->connectToHost(address, port);
}


void Client::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    for (;;) {
        socketStream.startTransaction();
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject())
                    jsonReceived(jsonDoc.object());
            }
        } else {
            break;
        }
    }
}
