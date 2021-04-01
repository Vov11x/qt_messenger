#include "authorizationwin.h"
#include "chatwin.h"
#include "signupwin.h"
#include "ui_authorizationwin.h"
#include "encryptpassword.h"
#include "connecttodb.h"


AuthorizationWin::AuthorizationWin(QWidget *parent): QMainWindow(parent), ui(new Ui::AuthorizationWin)
{
    ui->setupUi(this);
    this->setWindowTitle("Authorization");
    this->setFixedSize(597, 563);
}


AuthorizationWin::~AuthorizationWin()
{
    delete ui;
}


void AuthorizationWin::on_btn_log_in_clicked()
{
    QString user_email = ui->entry_email->text();
    QString user_password = ui->entry_password->text();
    unsigned user_id;

    if (!user_email.isEmpty() && !user_password.isEmpty()) {
        QSqlDatabase database = db::connect_to_database();

        if (database.open()) {
            QSqlQuery query;
            query.prepare("SELECT * FROM `messenger_users` \
                           WHERE `email` = :email \
                           AND `password` = :password;");
            query.bindValue(":email", user_email);
            query.bindValue(":password", encrypted_password(user_password));
            query.exec();
            if (query.next()) {
                user_id = query.value(0).toUInt();

                ChatWin *chatwin = new ChatWin(nullptr, user_id);
                chatwin->show();

                this->close();
            } else {
                QMessageBox::warning(this, "Warning!", "Wrong login or password.");
            }
            database.close();
        } else {
            QMessageBox::critical(this, "Error!", "Database is not accessible!");
        }
    } else {
        QMessageBox::warning(this, "Warning!", "Fields have to be not empty.");
    }
}


void AuthorizationWin::on_btn_sign_in_clicked()
{
    SignUpWin *signupwin = new SignUpWin;
    signupwin->show();

}
