#include "chatwin.h"
#include "ui_chatwin.h"
#include "client.h"
#include "connecttodb.h"
#include "settingswin.h"


ChatWin::ChatWin(QWidget *parent, unsigned user_id)
    : QMainWindow(parent)
    , ui(new Ui::ChatWin)
    , m_chatClient(new Client(this, user_id))
    , m_chatModel(new QStandardItemModel(this))
    , m_user_id(user_id)
{
    ui->setupUi(this);
    m_chatModel->insertColumn(0);
    ui->chatView->setModel(m_chatModel);

    this->setFixedSize(597, 563);

    QSqlDatabase database = db::connect_to_database();
    if (database.open()) {
        QSqlQuery query;
        query.prepare("SELECT messenger_users.name \
                       FROM messenger_users \
                       WHERE id = :id;");
        query.bindValue(":id", m_user_id);
        query.exec();
        query.next();
        QString userName = query.value(0).toString();
        database.close();
        this->setWindowTitle("Common chat - "+userName+"");
    } else {
        this->setWindowTitle("Chat");
    }

    attempt_connection();
    restore_session();

    ui->chatView->scrollToBottom();

    connect(m_chatClient,    &Client::connected,        this, &ChatWin::connected_to_server);
    connect(m_chatClient,    &Client::loggedIn,         this, &ChatWin::logged_in);
    connect(m_chatClient,    &Client::loginError,       this, &ChatWin::login_failed);
    connect(m_chatClient,    &Client::messageReceived,  this, &ChatWin::message_received);
    connect(m_chatClient,    &Client::disconnected,     this, &ChatWin::disconnected_from_server);
    connect(m_chatClient,    &Client::error,            this, &ChatWin::error);
    connect(m_chatClient,    &Client::userJoined,       this, &ChatWin::user_joined);
    connect(m_chatClient,    &Client::userLeft,         this, &ChatWin::user_left);
    connect(ui->sendButton,  &QPushButton::clicked,     this, &ChatWin::send_message);
    connect(ui->messageEdit, &QLineEdit::returnPressed, this, &ChatWin::send_message);
}


ChatWin::~ChatWin()
{
    delete ui;
}


void ChatWin::restore_session()
{
    QSqlDatabase database = db::connect_to_database();
    if (database.open()) {
        QSqlQuery query;
        query.exec("SELECT messenger_users.id, messenger_users.name, messenger_messages.message_content, messenger_messages.datetime \
                    FROM messenger_users \
                    INNER JOIN messenger_messages ON messenger_users.id = messenger_messages.user_id \
                    ORDER BY datetime ASC;");
        QString sender, text, datetime, datetime_formated;
        while (query.next()) {
            sender = query.value(1).toString();
            text = query.value(2).toString();
            datetime = query.value(3).toString();
            datetime_formated = datetime.mid(0, 10) + ' ' + datetime.mid(11, 8);

            if (QString::number(m_user_id) == query.value(0).toString()) {
                int newRow = m_chatModel->rowCount();
                m_chatModel->insertRow(newRow);
                m_chatModel->setData(m_chatModel->index(newRow, 0), '<'+datetime_formated+"> "+text);
                m_chatModel->setData(m_chatModel->index(newRow, 0), int(Qt::AlignRight | Qt::AlignVCenter), Qt::TextAlignmentRole);
            } else {
                message_received(sender, '<'+datetime_formated+"> " + text);
            }
        }

        query.prepare("SELECT messenger_metadata.client_theme \
                       FROM messenger_metadata \
                       WHERE messenger_metadata.user_id = :id");
        query.bindValue(":id", m_user_id);
        query.exec();
        QFile file;
        if (query.next()) {
            unsigned theme_number = query.value(0).toUInt();
            enum {STANDARD, LIGHT_SKY, NIGHT};
            switch (theme_number) {
            case STANDARD:
                file.setFileName(":/new/prefix/standard_theme.css");
                qDebug() << "th:" << STANDARD;
                break;
            case LIGHT_SKY:
                file.setFileName(":/new/prefix/light_sky_theme.css");
                qDebug() << "th:" << LIGHT_SKY;
                break;
            case NIGHT:
                file.setFileName(":/new/prefix/night_theme.css");
                qDebug() << "th:" << NIGHT;
                break;
            }

            if (file.open(QFile::ReadOnly)) {
                qApp->setStyleSheet(file.readAll());
            }
        } else {
            file.setFileName(":/new/prefix/standard_theme.css");
            if (file.open(QFile::ReadOnly)) {
                qApp->setStyleSheet(file.readAll());
            }
        }
    } else {
        QMessageBox::critical(this, "Error!", "Database is not accessible!");
    }

}

QString ChatWin::get_current_datetime()
{
    QDateTime dt = QDateTime::currentDateTime();
    QString datetime = dt.toString(Qt::ISODate);
    datetime[10] = ' ';

    return datetime;
}


void ChatWin::attempt_connection()
{
    const QString hostAddress = db::HOSTNAME;
    m_chatClient->connectToServer(QHostAddress(hostAddress), 3000);
}


void ChatWin::connected_to_server()
{
    QSqlDatabase database = db::connect_to_database();
    QString newUsername;
    if (database.open()) {
        QSqlQuery query;
        query.prepare("SELECT * FROM `messenger_users` \
                       WHERE `id` = :id;");
        query.bindValue(":id", m_user_id);
        query.exec();
        if (query.next()) {
            newUsername = query.value(1).toString();
        }
        database.close();
    }
    attempt_login(newUsername);
}


void ChatWin::attempt_login(const QString &username)
{
    m_chatClient->login(username);
}


void ChatWin::logged_in()
{
    ui->sendButton->setEnabled(true);
    ui->messageEdit->setEnabled(true);
    ui->chatView->setEnabled(true);
    m_lastUserName.clear();
}


void ChatWin::login_failed(const QString &reason)
{
    QMessageBox::critical(this, "Error", reason);
    connected_to_server();
}


void ChatWin::message_received(const QString &sender, const QString &text)
{
    int newRow = m_chatModel->rowCount();
    if (m_lastUserName != sender) {
        m_lastUserName = sender;
        QFont boldFont;
        boldFont.setBold(true);
        m_chatModel->insertRows(newRow, 2);
        m_chatModel->setData(m_chatModel->index(newRow, 0), sender + QLatin1Char(':'));
        m_chatModel->setData(m_chatModel->index(newRow, 0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
        m_chatModel->setData(m_chatModel->index(newRow, 0), boldFont, Qt::FontRole);
        ++newRow;
    } else {
        m_chatModel->insertRow(newRow);
    }
    m_chatModel->setData(m_chatModel->index(newRow, 0), text);
    m_chatModel->setData(m_chatModel->index(newRow, 0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
    ui->chatView->scrollToBottom();
}


void ChatWin::send_message()
{
    m_chatClient->sendMessage(ui->messageEdit->text());
    const int newRow = m_chatModel->rowCount();
    m_chatModel->insertRow(newRow);
    m_chatModel->setData(m_chatModel->index(newRow, 0),  '<'+get_current_datetime()+"> " + ui->messageEdit->text());
    m_chatModel->setData(m_chatModel->index(newRow, 0), int(Qt::AlignRight | Qt::AlignVCenter), Qt::TextAlignmentRole);
    ui->messageEdit->clear();
    ui->chatView->scrollToBottom();
    m_lastUserName.clear();
}


void ChatWin::disconnected_from_server()
{
    QMessageBox::warning(this, "Disconnected", "The host terminated the connection");
    ui->sendButton->setEnabled(false);
    ui->messageEdit->setEnabled(false);
    ui->chatView->setEnabled(false);
    m_lastUserName.clear();
}


void ChatWin::user_joined(const QString &username)
{
    const int newRow = m_chatModel->rowCount();
    m_chatModel->insertRow(newRow);
    m_chatModel->setData(m_chatModel->index(newRow, 0), tr("%1 joined").arg(username));
    m_chatModel->setData(m_chatModel->index(newRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
    m_chatModel->setData(m_chatModel->index(newRow, 0), QBrush(Qt::white), Qt::ForegroundRole);
    ui->chatView->scrollToBottom();
    m_lastUserName.clear();
}


void ChatWin::user_left(const QString &username)
{
    const int newRow = m_chatModel->rowCount();
    m_chatModel->insertRow(newRow);
    m_chatModel->setData(m_chatModel->index(newRow, 0), tr("%1 left").arg(username));
    m_chatModel->setData(m_chatModel->index(newRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
    m_chatModel->setData(m_chatModel->index(newRow, 0), QBrush(Qt::white), Qt::ForegroundRole);
    ui->chatView->scrollToBottom();
    m_lastUserName.clear();
}


void ChatWin::error(QAbstractSocket::SocketError socket_error)
{
    switch (socket_error) {
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::ProxyConnectionClosedError:
        return;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::critical(this, "Error", tr("Host refused to connect"));
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        QMessageBox::critical(this, "Error", tr("Proxy refused to connect."));
        break;
    case QAbstractSocket::ProxyNotFoundError:
        QMessageBox::critical(this, "Error", tr("Could not find proxy server."));
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::critical(this, "Error", "Could not find server.");
        break;
    case QAbstractSocket::SocketAccessError:
        QMessageBox::critical(this, "Error", "You do not have permission to\nperform this operation.");
        break;
    case QAbstractSocket::SocketResourceError:
        QMessageBox::critical(this, "Error", "Too many connections open.");
        break;
    case QAbstractSocket::SocketTimeoutError:
        QMessageBox::critical(this, "Error", "Operation timeout.");
        return;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        QMessageBox::critical(this, "Error", "Proxy server timeout.");
        break;
    case QAbstractSocket::NetworkError:
        QMessageBox::critical(this, "Error", "Can't connect to the network.");
        break;
    case QAbstractSocket::UnknownSocketError:
        QMessageBox::critical(this, "Error", "An unknown error has occurred.");
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        QMessageBox::critical(this, "Error", "Operation not supported.");
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        QMessageBox::critical(this, "Error", "Your proxy server requires authentication.");
        break;
    case QAbstractSocket::ProxyProtocolError:
        QMessageBox::critical(this, "Error", "Proxy link failure.");
        break;
    case QAbstractSocket::TemporaryError:
    case QAbstractSocket::OperationError:
        QMessageBox::warning(this, "Error", "Operation failed, please try again.");
        return;
    default:
        Q_UNREACHABLE();
    }
    ui->sendButton->setEnabled(false);
    ui->messageEdit->setEnabled(false);
    ui->chatView->setEnabled(false);
    m_lastUserName.clear();
}

void ChatWin::on_btn_settings_clicked()
{
    SettingsWin *sw = new SettingsWin(nullptr, m_user_id);
    sw->show();
}

void ChatWin::on_btn_menu_clicked()
{

}
