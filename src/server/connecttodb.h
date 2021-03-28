#ifndef CONNECTTODB_H
#define CONNECTTODB_H
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>


namespace db {
    const QString DB_DRIVER = "QMYSQL";
    const QString HOSTNAME  = "127.0.0.1";
    const QString USERNAME  = "root";
    const QString PASSWORD  = "root";
    const QString DB_NAME   = "messenger";

    QSqlDatabase connect_to_database();
}


QSqlDatabase db::connect_to_database();


#endif // CONNECTTODB_H
