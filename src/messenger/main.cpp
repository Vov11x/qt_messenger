#include <QApplication>
#include "authorizationwin.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/new/prefix/logo.png"));
    AuthorizationWin w;
    w.show();

    return a.exec();
}
