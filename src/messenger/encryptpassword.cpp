#include "encryptpassword.h"


QByteArray encrypted_password(QString &password) {
    QByteArray b_a;
    b_a.append(password);

    return b_a.toBase64();
}
