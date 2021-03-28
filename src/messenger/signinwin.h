#ifndef SIGNINWIN_H
#define SIGNINWIN_H

#include <QMainWindow>

namespace Ui {
class SignInWin;
}

class SignInWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit SignInWin(QWidget *parent = nullptr);
    ~SignInWin();

private:
    Ui::SignInWin *ui;
};

#endif // SIGNINWIN_H
