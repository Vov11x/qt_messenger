#ifndef CHATWIN_H
#define CHATWIN_H
#include <QMainWindow>
#include <QAbstractSocket>
#include <QStandardItemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QHostAddress>
#include <QStringRef>
#include <QFile>



namespace Ui {
    class ChatWin;
}


class Client;
class QStandardItemModel;


class ChatWin : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatWin)
private:
    Ui::ChatWin        *ui;
    Client             *m_chatClient;
    QStandardItemModel *m_chatModel;
    QString             m_lastUserName;
    unsigned            m_user_id;
    void restore_session();
    QString get_current_datetime();
public:
    explicit ChatWin(QWidget *parent = nullptr, unsigned user_id = 0);
    ~ChatWin();
private slots:
    void attempt_connection();
    void connected_to_server();
    void attempt_login(const QString &username);
    void logged_in();
    void login_failed(const QString &reason);
    void message_received(const QString &sender, const QString &text);
    void send_message();
    void disconnected_from_server();
    void user_joined(const QString &username);
    void user_left(const QString &username);
    void error(QAbstractSocket::SocketError socket_error);
    void on_btn_settings_clicked();
    void on_btn_menu_clicked();
};

#endif // CHATWIN_H
