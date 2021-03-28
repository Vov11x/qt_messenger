#include <connecttodb.h>


QSqlDatabase db::connect_to_database()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(db::DB_DRIVER);
    db.setHostName(db::HOSTNAME);
    db.setUserName(db::USERNAME);
    db.setPassword(db::PASSWORD);
    db.setDatabaseName(db::DB_NAME);

    return db;
}
