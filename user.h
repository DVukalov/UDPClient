#ifndef USER_H
#define USER_H


#pragma once

#include <QDialog>
#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QTime>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QMessageBox>
#include <QLayout>
namespace Ui {
class User;
}

class User : public QDialog
{
    Q_OBJECT

public:
    explicit User(QDialog *parent = 0);
    ~User();
private slots:
    bool connectToServer();
    void disconnect();
    bool parse_message(QByteArray message);
    void ping_server();
    //void send_Datagramm();
    void receive_Datagramm();


private:
    QHostAddress Address;
    QHostAddress AddressServer;
    uint Port;
    uint PortServer;
    QString Name;
    bool connected;
    QUdpSocket *socket_Out;
    QUdpSocket *socket_In;
    QByteArray MSG;
    QTimer *timePing;

    QMessageBox *initDialog;
    QLabel *dialog;
    QTextEdit *msg;
    QPushButton *send;
    QPushButton *close;

    QLineEdit *name_lineEdit;
    QLineEdit *adr_lineEdit;
    QLineEdit *port_lineEdit;
    Ui::User *ui;
};

#endif // USER_H
