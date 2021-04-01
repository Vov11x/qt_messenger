#ifndef SETTINGSWIN_H
#define SETTINGSWIN_H
#include <QWidget>
#include <QMessageBox>
#include <QtDebug>
#include <QFile>


namespace Ui {
    class SettingsWin;
}


class SettingsWin : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsWin(QWidget *parent = nullptr, unsigned user_id = 0);
    ~SettingsWin();
private slots:
    void on_btn_save_username_clicked();
    void on_btn_save_password_clicked();
    void on_btn_accept_theme_clicked();

    void on_btn_OK_clicked();

private:
    Ui::SettingsWin *ui;
    unsigned m_user_id;
};

#endif // SETTINGSWIN_H
