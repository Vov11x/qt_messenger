#ifndef MENUWIN_H
#define MENUWIN_H

#include <QWidget>

namespace Ui {
class MenuWin;
}

class MenuWin : public QWidget
{
    Q_OBJECT

public:
    explicit MenuWin(QWidget *parent = nullptr);
    ~MenuWin();

private:
    Ui::MenuWin *ui;
};

#endif // MENUWIN_H
