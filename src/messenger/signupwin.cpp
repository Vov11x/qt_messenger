#include "signupwin.h"
#include "ui_signupwin.h"
#include "encryptpassword.h"
#include "connecttodb.h"


SignUpWin::SignUpWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SignUpWin)
{
    ui->setupUi(this);
}


SignUpWin::~SignUpWin()
{
    delete ui;
}


void SignUpWin::on_btn_sign_up_clicked()
{
    QString user_name           = ui->entry_name          ->text();
    QString user_email          = ui->entry_email         ->text();
    QString user_pswrd          = ui->entry_pswrd         ->text();
    QString user_confirm_passwd = ui->entry_confirm_pasrwd->text();

    if (!user_name.isEmpty() && !user_email.isEmpty() && !user_pswrd.isEmpty() && !user_confirm_passwd.isEmpty()) {
        if (user_pswrd == user_confirm_passwd) {
            QSqlDatabase database = db::connect_to_database();
            if (database.open()) {
                QSqlQuery query;
                query.prepare("SELECT * \
                               FROM `messenger_users` \
                               WHERE `email` = :email;");
                query.bindValue(":email", user_email);
                query.exec();
                if (!query.next()) {
                    query.prepare("INSERT INTO `messenger_users` (`name`, `email`, `password`) \
                                   VALUES (:name, :email, :password);");
                    query.bindValue(":name",     user_name);
                    query.bindValue(":email",    user_email);
                    query.bindValue(":password", encrypted_password(user_pswrd));
                    query.exec();
                    QMessageBox::StandardButton reply = QMessageBox::information(this, "Success!",
                                              "Now, you have an account.\n Back to authorization?", QMessageBox::Yes);
                    if (reply == QMessageBox::Yes) {
                        this->close();
                    }
                } else {
                    QMessageBox::warning(this, "Warning!", "This e-mail has been already used.");
                }
                database.close();
            } else {
                QMessageBox::critical(this, "Error!", "Database is not accessible!");
            }
        } else {
            QMessageBox::warning(this, "Warning!", "Passwords don`t match.\nPlease, try again.");
        }
    } else {
        QMessageBox::warning(this, "Warning!", "Fields have to be not empty.");
    }
}
