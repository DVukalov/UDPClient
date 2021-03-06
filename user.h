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

    void disconnect();
    bool parse_message(QByteArray message);

    void send_Datagramm();
    void receive_Datagramm();
    //void errorRegistration();
    void initUser();
    //void print_message();


private:
    //QHostAddress Address;
    QHostAddress AddressServer;
    uint Port;
    uint PortServer;
    QString Name;
    bool connected;
    QUdpSocket *socket_Out;
    QUdpSocket *socket_In;
    QString MSG;
    QTimer *timePing;
    QTimer * timeRegistration;
    QPushButton * connectBut;
    QPushButton * closeBut;
    QDialog * inputDialog;

    //QMessageBox *initDialog;
    QLabel *dialogLabel;
    QTextEdit *msgText;
    QPushButton *sendButton;
    QPushButton *closeButton;
    QLabel *Chat;

    QLineEdit * name_lineEdit ;
    QLineEdit *host1_lineEdit ;
    QLineEdit * host2_lineEdit ;
    QLineEdit *host3_lineEdit ;
    QLineEdit * host4_lineEdit ;
    QLineEdit *port_lineEdit ;

    Ui::User *ui;
};

#endif // USER_H
