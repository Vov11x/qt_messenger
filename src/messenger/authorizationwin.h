#ifndef AUTHORIZATIONWIN_H
#define AUTHORIZATIONWIN_H
#include <QMainWindow>
#include <QMessageBox>
#include <QtDebug>
#include <QString>


QT_BEGIN_NAMESPACE
namespace Ui {
    class AuthorizationWin;
}
QT_END_NAMESPACE


class AuthorizationWin : public QMainWindow
{
    Q_OBJECT
public:
    AuthorizationWin(QWidget *parent = nullptr);
    ~AuthorizationWin();

private slots:
    void on_btn_log_in_clicked();
    void on_btn_sign_in_clicked();
private:
    Ui::AuthorizationWin *ui;
};
#endif // AUTHORIZATIONWIN_H
