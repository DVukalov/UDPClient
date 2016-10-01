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
    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));
    Port = qrand()%9999 + 10000;

    socket_Out = new QUdpSocket(this);
    socket_In = new QUdpSocket(this);

    timeRegistration = new QTimer(this);
    timeRegistration->setInterval(3000);

    connect(timeRegistration, SIGNAL(timeout()), SLOT(errorRegistration()));

    timePing = new QTimer(this);
    timePing->setInterval(60000);

    initDialog = new QMessageBox(this);
    connectToServer();

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
    //setWindowTitle("Dialog");
    ui->setupUi(this);
}

User::~User()
{
    delete socket_Out;
    delete socket_In;
    delete initDialog;
    delete dialogLabel;
    delete msgText;
    delete sendButton;
    delete closeButton;
    delete ui;
}

bool User :: connectToServer()
{
    initDialog->setText("Подключение к чату");

    QLineEdit *name_lineEdit = new QLineEdit(this);
    QLineEdit *host1_lineEdit = new QLineEdit(this);
    QLineEdit *host2_lineEdit = new QLineEdit(this);
    QLineEdit *host3_lineEdit = new QLineEdit(this);
    QLineEdit *host4_lineEdit = new QLineEdit(this);
    QLineEdit *port_lineEdit = new QLineEdit(this);

    initDialog->layout()->addWidget(name_lineEdit);
    initDialog->layout()->addWidget(host1_lineEdit);
    initDialog->layout()->addWidget(host2_lineEdit);
    initDialog->layout()->addWidget(host3_lineEdit);
    initDialog->layout()->addWidget(host4_lineEdit);
    initDialog->layout()->addWidget(port_lineEdit);




    initDialog->setStandardButtons(QMessageBox::Close);
    QPushButton *connectButton = initDialog->addButton("Connect", QMessageBox::ActionRole);
    connectButton->setFocusPolicy(Qt::StrongFocus);
    initDialog->exec();


    if (initDialog->clickedButton() == connectButton)
    {
        Name = name_lineEdit->text();
        PortServer = port_lineEdit->text().toUInt();

        if (host1_lineEdit->text().toUInt() <= 255 &&
                host2_lineEdit->text().toUInt() <= 255 &&
                host3_lineEdit->text().toUInt() <=255 &&
                host4_lineEdit->text().toUInt() <=255 )
        {
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
            delete port_lineEdit ;
            delete connectButton;
            return false;
        }
        QByteArray datagram =  userInit + separator + QByteArray(Name.toUtf8()) + separator
             + QByteArray::number(Port);
        socket_Out->writeDatagram(datagram.data(), datagram.size(), AddressServer, PortServer);
        socket_Out->bind(QHostAddress::Any, Port);
        connected = false;
        timeRegistration->start();

        delete name_lineEdit;
        delete host1_lineEdit;
        delete host2_lineEdit;
        delete host3_lineEdit;
        delete host4_lineEdit;
        delete port_lineEdit ;
        delete connectButton;
        return true;
    }
    else
    {
        delete name_lineEdit;
        delete host1_lineEdit;
        delete host2_lineEdit;
        delete host3_lineEdit;
        delete host4_lineEdit;
        delete port_lineEdit;
        delete connectButton;
        return false;
    }
}

bool User:: parse_message(QByteArray message)
{
    if (message.startsWith(userInit))
    {
        connected = true;
        timePing->start();
//        ui->setupUi(this);
        this->show();
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
        socket_Out->close();
        QByteArray buf;
        int index;
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
            connectToServer();
        }
        if (buf.toInt() == eFault)
        {
            QMessageBox *msgBox = new QMessageBox(this);
            msgBox->setStandardButtons(QMessageBox::Close);
            msgBox->setText("Errors Server");
            msgBox->exec();
            delete msgBox;
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
    return false;
}

void User :: disconnect()
{
    socket_Out->close();
    socket_In->close();
    connected = false;
}

void  User :: ping_server()
{

}

void User :: send_Datagramm()
{
   if (msgText->toPlainText() != separator)
   {
        QByteArray datagram = msgToUser + separator + QByteArray(Name.toUtf8()) + separator
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
    QByteArray datagram;
    socket_In->readDatagram(datagram.data(), datagram.size());
    dialogLabel->setText(datagram.trimmed());

}
void User :: errorRegistration()
{
    QMessageBox *msgBox = new QMessageBox(this);

    msgBox->setText("TimeOUT");
    QPushButton *okButton = msgBox->addButton("OK", QMessageBox::ActionRole);
    okButton->setFocusPolicy(Qt::StrongFocus);
    msgBox->exec();
    if (msgBox->clickedButton() == okButton)
    {
        connectToServer();
    }
    delete msgBox;


}
