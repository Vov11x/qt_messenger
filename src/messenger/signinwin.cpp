#include "signinwin.h"
#include "ui_signinwin.h"

SignInWin::SignInWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SignInWin)
{
    ui->setupUi(this);
}

SignInWin::~SignInWin()
{
    delete ui;
}
