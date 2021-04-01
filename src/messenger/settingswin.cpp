#include "settingswin.h"
#include "ui_settingswin.h"
#include "connecttodb.h"
#include "encryptpassword.h"


SettingsWin::SettingsWin(QWidget *parent, unsigned user_id) :
    QWidget(parent),
    ui(new Ui::SettingsWin),
    m_user_id(user_id)
{
    ui->setupUi(this);
    this->setWindowTitle("Settings");
    this->setFixedSize(303, 291);
}


SettingsWin::~SettingsWin()
{
    delete ui;
}


void SettingsWin::on_btn_save_username_clicked()
{
    QString new_username = ui->ent_new_username->text();
    QSqlDatabase database = db::connect_to_database();
    const int MIN_USERNAME_SIZE = 3;
    if (new_username.length() > MIN_USERNAME_SIZE) {
        if (database.open()) {
            QSqlQuery query;
            query.prepare("UPDATE messenger_users \
                           SET name = :name \
                           WHERE id = :id;");
            query.bindValue(":name", new_username);
            query.bindValue(":id",   m_user_id);
            query.exec();
            database.close();
            QMessageBox::information(this, "Success!", "Your nickname was changed.");
        } else {
            QMessageBox::critical(this, "Error!", "Database is not accessible!");
        }
    } else {
        QMessageBox::warning(this, "Error!", "Too short username!\n(At least 4 characters)");
    }
}


void SettingsWin::on_btn_save_password_clicked()
{
    QString new_password = ui->ent_new_password->text();
    const int MIN_PASSWORD_SIZE = 3;
    if (new_password.length() > MIN_PASSWORD_SIZE) {
        QSqlDatabase database = db::connect_to_database();
        if (database.open()) {
            QSqlQuery query;
            query.prepare("UPDATE messenger_users \
                           SET password = :password \
                           WHERE id = :id;");
            query.bindValue(":password", encrypted_password(new_password));
            query.bindValue(":id",   m_user_id);
            query.exec();
            database.close();
            QMessageBox::information(this, "Success!", "Your password was changed.");
        } else {
            QMessageBox::critical(this, "Error!", "Database is not accessible!");
        }
    } else {
        QMessageBox::warning(this, "Error!", "Too short password!\n(At least 4 characters)");
    }
}

void SettingsWin::on_btn_accept_theme_clicked()
{
    QFile file;
    unsigned theme_number;
    enum {STANDARD, LIGHT_SKY, NIGHT};
    if (ui->box_themes->currentText() == "Standard") {
        file.setFileName(":/new/prefix/standard_theme.css");
        theme_number = STANDARD;
    } else if (ui->box_themes->currentText() == "Light Sky") {
        file.setFileName(":/new/prefix/light_sky_theme.css");
        theme_number = LIGHT_SKY;
    } else if (ui->box_themes->currentText() == "Night") {
        file.setFileName(":/new/prefix/night_theme.css");
        theme_number = NIGHT;
    }

    if (file.open(QFile::ReadOnly)) {
        qApp->setStyleSheet(file.readAll());
    }

    QSqlDatabase database = db::connect_to_database();
    if (database.open()) {
        QSqlQuery query;
        query.prepare("SELECT * FROM `messenger_metadata` WHERE user_id = :user_id");
        query.bindValue(":user_id", m_user_id);
        query.exec();
        if (query.next()) {
            query.prepare("UPDATE messenger_metadata \
                           SET client_theme = :client_theme \
                           WHERE user_id = :user_id;");
            query.bindValue(":client_theme", theme_number);
            query.bindValue(":user_id", m_user_id);
            query.exec();
            qDebug() << "!!updated!!";
        } else {
            query.prepare("INSERT INTO `messenger_metadata` (user_id, client_theme) \
                           VALUES (:user_id, :client_theme)");
            query.bindValue(":user_id",      m_user_id);
            query.bindValue(":client_theme", theme_number);
            query.exec();
            qDebug() << "!!written!!";
        }
        database.close();
    } else {
        QMessageBox::critical(this, "Error!", "Database is not accessible!");
    }
}

void SettingsWin::on_btn_OK_clicked()
{
    this->close();
}
