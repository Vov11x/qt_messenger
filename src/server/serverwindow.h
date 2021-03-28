#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H
#include <QWidget>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QMessageBox>


QT_BEGIN_NAMESPACE
namespace Ui { class ServerWindow; }
QT_END_NAMESPACE


class ChatServer;

class ServerWindow : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ServerWindow)
public:
    explicit ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow();
private:
    Ui::ServerWindow *ui;
    ChatServer       *m_chatServer;
private slots:
    void toggleStartServer();
    void logMessage(const QString &msg);
};


#endif // SERVERWINDOW_H
