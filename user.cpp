#include "user.h"
#include "ui_user.h"

User::User(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::User)
{
    ui->setupUi(this);
}

User::~User()
{
    delete ui;
}

bool User :: connect()
{
    socket_Out = new QUdpSocket(this);
    socket_Out->bind(QHostAddress::Any, Port);
    QByteArray datagram = "?" + QByteArray::number(ID) + "?" + QByteArray::number(Address) +
            "?" + QByteArray.number(Port) + "?" + QByteArray.number(Name);
    socket_Out->writeDatagram(datagram.data(), datagram.size(), AddressServer, PortServer);
    connected = true;

}

void User :: disconnect()
{
    socket_Out->close();
    connected = false;
}

void  User :: ping_server()
{

}

void User :: send_Datagramm(QByteArray MSG)
{
    socket_Out->writeDatagram(datagram.data(), datagram.size(), AddressServer, PortServer);
}

void  User :: receive_Datagramm()
{

}
