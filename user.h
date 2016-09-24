#ifndef USER_H
#define USER_H


#pragma once

#include <QDialog>
#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QTime>
#include <QTimer>

namespace Ui {
class User;
}

class User : public QDialog
{
    Q_OBJECT

public:
    explicit User(uint ID,
                  QHostAddress Adress,
                  uint Port,
                  QString name,
                  QObject *parent = 0);
    ~User();
    bool connect();
    void disconnect();
    void ping_server();
    void send_Datagramm();
    void receive_Datagramm();


private:
    uint ID;
    QHostAddress Address;
    QHostAddress AddressServer;
    uint Port;
    uint PortServer;
    QString Name;
    bool connected;
    QUdpSocket *socket_Out;
    QUdpSocket *socket_In;
    QByteArray MSG;
    Ui::User *ui;
};

#endif // USER_H
