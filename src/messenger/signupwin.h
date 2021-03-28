#ifndef SIGNUPWIN_H
#define SIGNUPWIN_H
#include <QMainWindow>
#include <authorizationwin.h>


namespace Ui {
class SignUpWin;
}


class SignUpWin : public QMainWindow
{
    Q_OBJECT
public:
    explicit SignUpWin(QWidget *parent = nullptr);
    ~SignUpWin();
private slots:
    void on_btn_sign_up_clicked();
private:
    Ui::SignUpWin *ui;
};

#endif // SIGNUPWIN_H
