#include "user.h"
#include "ui_user.h"


namespace
{
     // Regular expressions for messages
    const QByteArray userInit = "INITUSER";
    const QByteArray msgToUser = "MSG";
    const QByteArray pingUser = "PING";
    const QByteArray separator = "#@#"; //???
}

User::User(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::User)
{
    timePing = new QTimer(this);
    timePing->setInterval(60000);
    initDialog = new QMessageBox(this);
    connectToServer();
    ui->setupUi(this);
}

User::~User()
{
    delete socket_Out;
    delete socket_In;

    delete ui;
}

bool User :: connectToServer()
{
    initDialog->setText("Подключение к чату");
    name_lineEdit = new QLineEdit(this);
    initDialog->layout()->addWidget(name_lineEdit);
    //name_lineEdit
    initDialog->setStandardButtons(QMessageBox::Close);
    QPushButton *connectButton = initDialog->addButton("Connect", QMessageBox::ActionRole);
    connectButton->setFocusPolicy(Qt::StrongFocus);
    initDialog->exec();

    if (initDialog->clickedButton() == connectButton)
    {
        socket_Out = new QUdpSocket(this);
        socket_Out->bind(QHostAddress::Any, Port);
        QByteArray datagram =  userInit + separator + QByteArray(Name.toUtf8()) + separator
             + QByteArray::number(Port);
        socket_Out->writeDatagram(datagram.data(), datagram.size(), AddressServer, PortServer);
        connected = false;
        return true;
    }
    else
        return false;
}

bool User:: parse_message(QByteArray message)
{
    if (message.startsWith(userInit))
    {
        connected = true;
        timePing->start();
        return true;
    }
    if (message.startsWith(pingUser))
    {
        timePing->stop();
        timePing->start();
        return true;
    }
    if (message.startsWith(msgToUser))
    {
        QByteArray buf;
        QString name;
        int index;
        if (message.count(separator) != 2)
        {
            __print << "Wrong message";
            return false;
        }
        message.remove(0, ::msgToUser.size() + ::separator.size());
        index = message.indexOf(::separator);

        qCopy(message.begin(), message.begin() + index, buf.begin());

        name = QString(buf.data());

        message.remove(0, index + ::separator.size());
        qCopy(message.begin(), message.end(), buf.begin());
        if (buf.data() == NULL)
        {
            __print << "Empty";
            return false;

        }
        return false;
        ///TO DO ВЫвести
    }
    return false;
}

void User :: disconnect()
{
    socket_Out->close();
    connected = false;
}

void  User :: ping_server()
{

}

/*void User :: send_Datagramm(QByteArray MSG)
{
   // socket_Out->writeDatagram(datagram.data(), datagram.size(), AddressServer, PortServer);
}*/

void  User :: receive_Datagramm()
{

}
