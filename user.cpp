#include "user.h"
#include "ui_user.h"
namespace
{
     // Regular expressions for messages
    const QByteArray userInit = "INITUSER";
    const QByteArray msgToUser = "MSG";
    const QByteArray pingUser = "PING";
    const QByteArray separator = "#@#";
    const QByteArray errorMsg = "ERROR";
    const uint timeReg = 3000;
    const uint timePNG = 60000;
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
    this->setFixedSize(400, 520);
    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));
    Port = qrand()%9999 + 10000;
    timePing = new QTimer(this);
    timePing->setInterval(timePNG);

    timeRegistration = new QTimer(this);
    timeRegistration->setInterval(timeReg);


    inputDialog = new QDialog(this);
    inputDialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowTitleHint);

    socket_Out = new QUdpSocket(this);
    socket_In = new QUdpSocket(this);

    name_lineEdit = new QLineEdit(this);
    host1_lineEdit = new QLineEdit(this);
    host2_lineEdit = new QLineEdit(this);
    host3_lineEdit = new QLineEdit(this);
    host4_lineEdit = new QLineEdit(this);
    port_lineEdit = new QLineEdit(this);

    connectBut = new QPushButton("Connect",this);
    closeBut = new QPushButton("Close",this);

    dialogLabel = new QLabel(this);
    dialogLabel->setFixedHeight(420);

    Chat = new QLabel(this);
    Chat->setText("Dialog");
    Chat->setFont(QFont("Courier", 16,QFont::Bold));
    Chat->setAlignment(Qt::AlignHCenter);

    msgText = new QTextEdit("Write message...", this);
    msgText->setFixedHeight(60);

    sendButton = new QPushButton("Send", this);
    closeButton = new QPushButton("Close", this);
    closeButton->setFixedWidth(50);
    connect(closeButton, SIGNAL(clicked()),this, SLOT(close()));
    connect(sendButton, SIGNAL(clicked()), this, SLOT(send_Datagramm()));
    //connect(msgText, SIGNAL(cursorPositionChanged()), msgText, SLOT(clear()));

    QHBoxLayout *headLayout = new QHBoxLayout;
    headLayout->addWidget(Chat);
    headLayout->addWidget(closeButton);

     QHBoxLayout *sendLayout = new QHBoxLayout;
     sendLayout->addWidget(msgText);
     sendLayout->addWidget(sendButton);

     QVBoxLayout * mainLayout = new QVBoxLayout;
     mainLayout->addLayout(headLayout);
     mainLayout->addWidget(dialogLabel);
     mainLayout->addLayout(sendLayout);

     this->setLayout(mainLayout);






    ui->setupUi(this);
    this->setDisabled(true);

    initUser();
}

User::~User()
{
    __print;


    //удалить дlineEdit
    delete ui;
}


void User::initUser()
{
    __print;

    inputDialog = new QDialog(this);
    inputDialog->setFixedSize(275,150);
    inputDialog->setWindowFlags(inputDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    inputDialog->setWindowFlags(inputDialog->windowFlags() & ~Qt::WindowCloseButtonHint);

    QLabel *nameLabel = new QLabel("Name",this);
    nameLabel->setFont(QFont("Courier", 12, QFont::Bold));

    QLabel *point1Label = new QLabel(".",this);
    point1Label->setFont(QFont("Courier", 11, QFont::Bold));

    QLabel *point2Label = new QLabel(".",this);
    point2Label->setFont(QFont("Courier", 11, QFont::Bold));

    QLabel *point3Label = new QLabel(".",this);
    point3Label->setFont(QFont("Courier", 11, QFont::Bold));

    QLabel *portLabel = new QLabel("Port",this);
    portLabel->setFont(QFont("Courier", 12, QFont::Bold));

    QLabel *helloLabel = new QLabel("Hello",this);
    helloLabel->setFont(QFont("Courier", 16,QFont::Bold));
    helloLabel->setAlignment(Qt::AlignHCenter);

    QLabel *hostLabel = new QLabel("Host",this);
    hostLabel->setFont(QFont("Courier", 12, QFont::Bold));

    QHBoxLayout *helloLayout = new QHBoxLayout;
    helloLayout->addWidget(helloLabel);

    QVBoxLayout *labelLayout = new QVBoxLayout;
    labelLayout->addWidget(hostLabel);
    labelLayout->addWidget(portLabel);
    labelLayout->addWidget(nameLabel);

    QHBoxLayout *hostLayout = new QHBoxLayout;
    hostLayout->addWidget(host1_lineEdit);
    hostLayout->addWidget(point1Label);
    hostLayout->addWidget(host2_lineEdit);
    hostLayout->addWidget(point2Label);
    hostLayout->addWidget(host3_lineEdit);
    hostLayout->addWidget(point3Label);
    hostLayout->addWidget(host4_lineEdit);

    QVBoxLayout *lineLayout = new QVBoxLayout;
    lineLayout->addLayout(hostLayout);
    lineLayout->addWidget(port_lineEdit);
    lineLayout->addWidget(name_lineEdit);


    QHBoxLayout *faceLayout = new QHBoxLayout;
    faceLayout->addLayout(labelLayout);
    faceLayout->addLayout(lineLayout);

    connectBut->setFocusPolicy(Qt::StrongFocus);


    QHBoxLayout *butLayout = new QHBoxLayout;
    butLayout->addWidget(connectBut);
    butLayout->addWidget(closeBut);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(helloLayout);
    mainLayout->addLayout(faceLayout);
    mainLayout->addLayout(butLayout);

    inputDialog->setLayout(mainLayout);

    connect(timeRegistration, &QTimer::timeout, this, [this] ()
    {
        __print;
        QMessageBox *msgBox = new QMessageBox;

        timeRegistration->stop();
        socket_In->close();

        msgBox->setText("Connection timeout!");
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->exec();

        connectBut->setEnabled(true);
        connectBut->setFocusPolicy(Qt::StrongFocus);
        delete msgBox;


    });

    connect(closeBut, &QPushButton::clicked, this, [this]()
    {
        this->inputDialog->close();

    });

    connect(connectBut, &QPushButton::clicked, this, [this]()
    {
        connectBut->setDisabled(true);
        __print;
        bool ok1, ok2, ok3, ok4, ok5, OK = true;
        host1_lineEdit->text().toUInt(&ok1);
        host2_lineEdit->text().toUInt(&ok2);
        host3_lineEdit->text().toUInt(&ok3);
        host4_lineEdit->text().toUInt(&ok4);
        port_lineEdit->text().toUInt(&ok5);
        if (ok1 && ok2 && ok3 && ok4)
        {
            __print<<"NORM host";
            AddressServer = QHostAddress(host1_lineEdit->text() + "." +
                                         host2_lineEdit->text() + "." +
                                         host3_lineEdit->text() + "." +
                                         host4_lineEdit->text());
        }
        else
        {
            OK = false;
            QMessageBox *erHost = new QMessageBox;
            erHost->setText("Wrong Host!");
            erHost->setIcon(QMessageBox::Warning);
            erHost->exec();
            delete erHost;
        }
        if (ok5)
        {
            __print << "Norm Port";
            PortServer = port_lineEdit->text().toUInt();
        }
        else
        {
            OK = false;
            QMessageBox *erHost = new QMessageBox;
            erHost->setText("Wrong Port!");
            erHost->setIcon(QMessageBox::Warning);
            erHost->exec();
            delete erHost;
        }
        if (!name_lineEdit->text().isEmpty() &&
                !name_lineEdit->text().contains(::separator))
        {
            Name = name_lineEdit->text();
        }
        else if (name_lineEdit->text().contains(::separator))
        {
            OK = false;
            QMessageBox *erHost = new QMessageBox;
            erHost->setText(tr("Name contains illegual symbol %1!")
                            .arg(separator.data()));
            erHost->setIcon(QMessageBox::Warning);
            erHost->exec();

        }
        else
        {
            OK = false;
            QMessageBox *erHost = new QMessageBox;
            erHost->setText("Empty name!");
            erHost->setIcon(QMessageBox::Warning);
            erHost->exec();
            delete erHost;
        }
        if (OK)
        {
            QByteArray datagram =  ::userInit + separator
                    + QByteArray(Name.toUtf8()) + separator
                    + QByteArray::number(Port);
            socket_In->bind(Port, QUdpSocket::ShareAddress);
            connect(socket_In, SIGNAL(readyRead()),
                    this, SLOT(receive_Datagramm()));
            socket_Out->writeDatagram(datagram.data(), datagram.size(),
                                      AddressServer, PortServer);
            connected = false;
            timeRegistration->start();
        }
        else
        {
            connectBut->setEnabled(true);
            connectBut->setFocusPolicy(Qt::StrongFocus);

        }
    });
    inputDialog->exec();

    delete inputDialog;
    inputDialog = NULL;
}


bool User :: parse_message(QByteArray message)
{
__print << message;
    if (message.startsWith(userInit))
    {
        __print;
        connected = true;
        timePing->start();
        timeRegistration->stop();
        inputDialog->close();
        this->setEnabled(true);
        this->show();
        __print<<this->isActiveWindow()<<this->isEnabled();

        return true;
    }
    if (message.startsWith(pingUser))
    {
        __print;
        timePing->stop();
        timePing->start();
        return true;
    }
    if(message.startsWith(errorMsg))
    {
        __print;
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
            msgBox->setText("Duplicate name!");
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->exec();
            delete msgBox;
        }
        if (buf.toInt() == eFault)
        {
            QMessageBox *msgBox = new QMessageBox(this);
            msgBox->setStandardButtons(QMessageBox::Close);
            msgBox->setText("Server Fatal!");
            msgBox->setIcon(QMessageBox::Critical);
            msgBox->exec();
            inputDialog->show();
            this->setDisabled(true);
            inputDialog->show();
            inputDialog->setEnabled(true);
            delete msgBox;

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
        MSG = QString(buf.data());
        dialogLabel->setText(dialogLabel->text() + "\n\n"
                             + name + " "
                             + QByteArray::number(
                                 QTime::currentTime().hour()) + ":"
                             + QByteArray::number(
                                 QTime::currentTime().minute()) + ":"
                             + QByteArray::number(
                                 QTime::currentTime().second()) + "\n"
                             + MSG);
        return true;
    }
    return false;
}

void User::receive_Datagramm()
{
    while (socket_In->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(socket_In->pendingDatagramSize());
        socket_In->readDatagram(datagram.data(), datagram.size());
        __print << parse_message(datagram);
    }
}

void User :: disconnect()
{
    __print;
    socket_In->close();
    connected = false;
}

void User :: send_Datagramm()
{
    __print;
   if (!msgText->toPlainText().contains(::separator))
   {
        QByteArray datagram = msgToUser + separator
                + QByteArray(Name.toUtf8()) + separator
                + QByteArray(msgText->toPlainText().toUtf8());

        socket_Out->writeDatagram(datagram.data(), datagram.size(), AddressServer, PortServer);
        msgText->clear();
        msgText->setText("Write message...");
        return;
   }
   else
   {
       QMessageBox *separatorMSG = new QMessageBox(this);
       separatorMSG->setText(tr("Name contains illegual symbol %1!")
                             .arg(separator.data()));
       separatorMSG->setIcon(QMessageBox::Warning);
       separatorMSG->exec();
       delete separatorMSG;
   }
}










