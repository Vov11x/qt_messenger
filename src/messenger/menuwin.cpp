#include "menuwin.h"
#include "ui_menuwin.h"

MenuWin::MenuWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MenuWin)
{
    ui->setupUi(this);
}

MenuWin::~MenuWin()
{
    delete ui;
}
