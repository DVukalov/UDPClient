#include "user.h"
#include "ui_user.h"


namespace
{
     // Regular expressions for messages
    const QByteArray userInit = "INITUSER";
    const QByteArray msgToUser = "MSG";
    const QByteArray pingUser = "PING";
    const QByteArray separator = "#@#"; //???
    const QByteArray errorMsg = "ERROR";
    enum Errors
    {
        eName = 1,
        eFault = 2
    };
}



User::User(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::User)
{
    __print;
    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));
    Port = qrand()%9999 + 10000;

    socket_Out = new QUdpSocket(this);
    socket_In = new QUdpSocket(this);

    timeRegistration = new QTimer(this);
    timeRegistration->setInterval(10000);

    connect(timeRegistration, SIGNAL(timeout()), SLOT(errorRegistration()));

    timePing = new QTimer(this);
    timePing->setInterval(60000);

    inputDialog = new QDialog(this);

    //initDialog = new QMessageBox(this);
    if(!connectToServer())
    {
        // todo: обработать плохой коннект
    }

    dialogLabel = new QLabel(this);
    msgText= new QTextEdit(this);


    // buttons
    sendButton = new QPushButton(this);
    closeButton = new QPushButton(this);
    sendButton->setToolTip("Send message");
    closeButton->setToolTip("Close chat");
    sendButton->setText("Send Message");
    closeButton->setText("Close chat");

    connect(sendButton, SIGNAL(clicked()), this, SLOT(send_Datagramm()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    //sendButton->setToolTipDuration(800);

    // label
    dialogLabel->setText("Sas");

    //testEdit
    msgText = new QTextEdit(this);
    msgText->setText("Pisos");

    QVBoxLayout *sendLayout = new QVBoxLayout;
    sendLayout->addWidget(sendButton);

    QVBoxLayout *closeLayout = new QVBoxLayout;
    closeLayout->addWidget( closeButton);

    QVBoxLayout *dialogLayout = new QVBoxLayout;
    dialogLayout->addWidget(dialogLabel);

    QVBoxLayout *msgLayout = new QVBoxLayout;
    msgLayout->addWidget(msgText);


    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(sendLayout);
    mainLayout->addLayout(closeLayout);
    mainLayout->addLayout(dialogLayout);
    mainLayout->addLayout(msgLayout);

    this->setLayout(mainLayout);
    setWindowTitle("Client");
    ui->setupUi(this);
    this->setDisabled(true);
}

User::~User()
{
    __print;
    delete socket_Out;
    delete socket_In;
    //delete initDialog;
    delete dialogLabel;
    delete msgText;
    delete sendButton;
    delete closeButton;
    delete ui;
}

bool User :: connectToServer()
{
    __print;
    //initDialog->setText("Подключение к чату");
    inputDialog->setWindowTitle("Registration");

    QLineEdit *name_lineEdit = new QLineEdit(this);
    QLineEdit *host1_lineEdit = new QLineEdit(this);
    QLineEdit *host2_lineEdit = new QLineEdit(this);
    QLineEdit *host3_lineEdit = new QLineEdit(this);
    QLineEdit *host4_lineEdit = new QLineEdit(this);
    QLineEdit *port_lineEdit = new QLineEdit(this);

    QLabel *nameLabel = new QLabel(this);
    QLabel *hostLabel = new QLabel(this);
    QLabel *portLabel = new QLabel(this);
    QLabel *point1Label = new QLabel(this);
    QLabel *point2Label = new QLabel(this);
    QLabel *point3Label = new QLabel(this);
    QLabel *helloLabel = new QLabel(this);

    nameLabel->setText("Name");
    hostLabel->setText("Address");
    portLabel->setText("Port");
    point1Label->setText(".");
    point2Label->setText(".");
    point3Label->setText(".");
    helloLabel->setText("Hello");

    QHBoxLayout *helloLayout = new QHBoxLayout(this);
    helloLayout->addWidget(helloLabel);

    QHBoxLayout *hostLayout = new QHBoxLayout(this);
    hostLayout->addWidget(hostLabel);
    hostLayout->addWidget(host1_lineEdit);
    hostLayout->addWidget(point1Label);
    hostLayout->addWidget(host2_lineEdit);
    hostLayout->addWidget(point2Label);
    hostLayout->addWidget(host3_lineEdit);
    hostLayout->addWidget(point3Label);
    hostLayout->addWidget(host4_lineEdit);

    QHBoxLayout *portLayout = new QHBoxLayout(this);
    portLayout->addWidget(portLabel);
    portLayout->addWidget(port_lineEdit);

    QHBoxLayout *nameLayout = new QHBoxLayout(this);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(name_lineEdit);

    QPushButton *connectBut = new QPushButton;
    connectBut->setText("Connect");
    connectBut->setFocusPolicy(Qt::StrongFocus);

    QPushButton *closeBut = new QPushButton;
    closeBut->setText("Close");
    connect(closeBut, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *butLayout = new QHBoxLayout;
    butLayout->addWidget(connectBut);
    butLayout->addWidget(closeBut);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(helloLayout);
    mainLayout->addLayout(hostLayout);
    mainLayout->addLayout(portLayout);
    mainLayout->addLayout(nameLayout);
    mainLayout->addLayout(butLayout);

    inputDialog->setLayout(mainLayout);
    inputDialog->exec();
    /*initDialog->layout()->addWidget(name_lineEdit);
    initDialog->layout()->addWidget(host1_lineEdit);
    initDialog->layout()->addWidget(host2_lineEdit);
    initDialog->layout()->addWidget(host3_lineEdit);
    initDialog->layout()->addWidget(host4_lineEdit);
    initDialog->layout()->addWidget(port_lineEdit);


    initDialog->setStandardButtons(QMessageBox::Close);
    QPushButton *connectButton =
            initDialog->addButton("Connect", QMessageBox::ActionRole);
    connectButton->setFocusPolicy(Qt::StrongFocus);
    initDialog->exec();*/



    while(connectBut->isDown() == false)
    {
        __print << "sas";
    };
        inputDialog->close();
        if (!(host1_lineEdit->text().isEmpty() ||
              host2_lineEdit->text().isEmpty() ||
              host3_lineEdit->text().isEmpty() ||
              host4_lineEdit->text().isEmpty() ||
              name_lineEdit->text().isEmpty() ||
              port_lineEdit->text().isEmpty()) &&
                host1_lineEdit->text().toUInt() <= 255 &&
                host2_lineEdit->text().toUInt() <= 255 &&
                host3_lineEdit->text().toUInt() <= 255 &&
                host4_lineEdit->text().toUInt() <= 255 )
        {
            Name = name_lineEdit->text();
            PortServer = port_lineEdit->text().toUInt();
            AddressServer = QHostAddress(host1_lineEdit->text() + "." +
                                         host2_lineEdit->text() + "." +
                                         host3_lineEdit->text() + "." +
                                         host4_lineEdit->text());
        }
        else
        {
            delete name_lineEdit;
            delete host1_lineEdit;
            delete host2_lineEdit;
            delete host3_lineEdit;
            delete host4_lineEdit;
            delete port_lineEdit;
            delete nameLabel;
            delete hostLabel;
            delete portLabel;
            delete point1Label;
            delete point2Label;
            delete point3Label;
            delete helloLabel;
            delete helloLayout;
            delete hostLayout;
            delete portLayout ;
            delete nameLayout;
            delete connectBut;
            delete closeBut;
            delete butLayout;
            delete mainLayout;
            connectToServer();
            return false;
        }
        QByteArray datagram =  userInit + separator
                + QByteArray(Name.toUtf8()) + separator
                + QByteArray::number(Port);
        socket_Out->writeDatagram(datagram.data(), datagram.size(),
                                  AddressServer, PortServer);
        socket_In->bind(Port, QUdpSocket::ShareAddress);
        connect(socket_In, SIGNAL(readyRead()),
                this, SLOT(receive_Datagramm()));
        connected = false;

        timeRegistration->start();

        delete name_lineEdit;
        delete host1_lineEdit;
        delete host2_lineEdit;
        delete host3_lineEdit;
        delete host4_lineEdit;
        delete port_lineEdit;
        delete nameLabel;
        delete hostLabel;
        delete portLabel;
        delete point1Label;
        delete point2Label;
        delete point3Label;
        delete helloLabel;
        delete helloLayout;
        delete hostLayout;
        delete portLayout ;
        delete nameLayout;
        delete connectBut;
        delete closeBut;
        delete butLayout;
        delete mainLayout;
        return true;
   /* }
    else
    {
        delete name_lineEdit;
        delete host1_lineEdit;
        delete host2_lineEdit;
        delete host3_lineEdit;
        delete host4_lineEdit;
        delete port_lineEdit;
        delete nameLabel;
        delete hostLabel;
        delete portLabel;
        delete point1Label;
        delete point2Label;
        delete point3Label;
        delete helloLabel;
        delete helloLayout;
        delete hostLayout;
        delete portLayout ;
        delete nameLayout;
        delete connectBut;
        delete closeBut;
        delete butLayout;
        delete mainLayout;
        return false;
    }*/
}



bool User:: parse_message(QByteArray message)
{
    /*__print << message;
    if (message.startsWith(userInit))
    {
        connected = true;
        timePing->start();
        timeRegistration->stop();
        this->setEnabled(true);
        return true;
    }
    if (message.startsWith(pingUser))
    {
        timePing->stop();
        timePing->start();
        return true;
    }
    if(message.startsWith(errorMsg))
    {
        socket_In->close();
        QByteArray buf;
        int index;

        timeRegistration->stop();

        message.remove(0, ::errorMsg.size() + ::separator.size());
        index = message.indexOf(::separator);

        qCopy(message.begin(), message.begin() + index, buf.begin());
        if (buf.toInt() == eName)
        {
            QMessageBox *msgBox = new QMessageBox(this);
            msgBox->setStandardButtons(QMessageBox::Close);
            msgBox->setText("Errors Name");
            msgBox->exec();
            delete msgBox;

            delete initDialog;
            initDialog = new QMessageBox(this);

            connectToServer();
        }
        if (buf.toInt() == eFault)
        {
            QMessageBox *msgBox = new QMessageBox(this);
            msgBox->setStandardButtons(QMessageBox::Close);
            msgBox->setText("Errors Server");
            msgBox->exec();
            delete msgBox;

            delete initDialog;
            initDialog = new QMessageBox(this);

            connectToServer();
        }
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
    return false;*/
}

void User :: disconnect()
{
    __print;
    socket_Out->close();
    socket_In->close();
    connected = false;
}

void  User :: ping_server()
{    
    __print;
}

void User :: send_Datagramm()
{
    __print;
   if (msgText->toPlainText() != separator)
   {
        QByteArray datagram = msgToUser + separator
                + QByteArray(Name.toUtf8()) + separator
                + QByteArray(msgText->toPlainText().toUtf8());

        socket_Out->writeDatagram(datagram.data(), datagram.size(), AddressServer, PortServer);
        msgText->clear();
   }
   else
       msgText->setText("LOL");
       //QMessageBox *separatorMSG = new QMessageBox(this);
}

void  User :: receive_Datagramm()
{
    __print;
    QByteArray datagram;
    datagram.resize(socket_In->pendingDatagramSize());
    socket_In->readDatagram(datagram.data(), datagram.size());
    dialogLabel->setText(QString(datagram.data()));
    parse_message(datagram);

}

void User :: errorRegistration()
{
    __print;
    QMessageBox *msgBox = new QMessageBox(this);

    timeRegistration->stop();
    socket_In->close();

    msgBox->setText("TimeOUT");
    QPushButton *okButton = msgBox->addButton("OK", QMessageBox::ActionRole);
    okButton->setFocusPolicy(Qt::StrongFocus);
    msgBox->exec();

    if (msgBox->clickedButton() == okButton)
    {
        //delete initDialog;
      //  initDialog = new QMessageBox(this);
        connectToServer();
    }
    __print<<"HARD PISOS!"<<timeRegistration->isActive();
    delete msgBox;
}
