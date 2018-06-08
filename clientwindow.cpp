#include "clientwindow.h"
#include "ui_clientwindow.h"

#include <QMessageBox>
#include <QtEndian>
ClientWindow::ClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientWindow),
    tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);
    connect(tcpSocket, &QIODevice::readyRead, this, &ClientWindow::readResponse);
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &ClientWindow::displayError);
    connect(tcpSocket, &QAbstractSocket::connected, this, &ClientWindow::socketConnected);
}

ClientWindow::~ClientWindow()
{
    tcpSocket->disconnect();
    tcpSocket->disconnectFromHost();
    tcpSocket->abort();
    tcpSocket->deleteLater();
    delete ui;
}

void ClientWindow::on_connectButton_clicked()
{
    ui->connectButton->setEnabled(false);
    QString port = ui->portEdit->text();
    bool ok;
    int portNr = port.toInt(&ok);

    //port should be int and between 0 and 65535
    if(!ok || !(0 < portNr && portNr < 65535)){
        ui->connectButton->setEnabled(true);
        ui->connectionLabel->setText("Disconnected");
        ui->connectionLabel->setStyleSheet("color: red");
        QMessageBox::information(this, tr("STE-CroonwolterDros Interface test"),
                                         tr("Port should be an integer between 1 and 65535"));

        return;
    }
    tcpSocket->connectToHost(ui->addressEdit->text(), portNr);

    ui->connectionLabel->setText("Connecting");
    ui->connectionLabel->setStyleSheet("color: orange");
}

void ClientWindow::readResponse(){
    if (tcpSocket->bytesAvailable() >= 14) {
        char networkBytes[14];
        tcpSocket->read(networkBytes, 14);

        bool coupled = networkBytes[0];
        int32_t soc = ((unsigned char)(networkBytes[1]) << 24) | ((unsigned char)(networkBytes[2]) << 16) | ((unsigned char)(networkBytes[3]) << 8) | ((unsigned char)(networkBytes[4]));
        bool localControl = networkBytes[5];
        int32_t power = ((unsigned char)(networkBytes[6]) << 24) | ((unsigned char)(networkBytes[7]) << 16) | ((unsigned char)(networkBytes[8]) << 8) | ((unsigned char)(networkBytes[9]));
        int32_t setpoint = ((unsigned char)(networkBytes[10]) << 24) | ((unsigned char)(networkBytes[11]) << 16) | ((unsigned char)(networkBytes[12]) << 8) | ((unsigned char)(networkBytes[13]));

        ui->coupledLabel->setText((coupled ? QString("true") : QString("false")));
        ui->socLabel->setText(QString().number(soc));
        ui->controlLabel->setText(localControl ? QString("true") : QString("false"));
        ui->powerLabel->setText(QString().number(power));
        ui->socSetpointLabel->setText(QString().number(setpoint));
    }
}

void ClientWindow::on_sendButton_clicked()
{
    bool controlActive = ui->controlActiveTrueButton->isChecked();
    bool conversionOk;
    int32_t socSetpoint = ui->socSetpointEdit->text().toInt(&conversionOk);
    if (!conversionOk || !(0 <= socSetpoint && socSetpoint <= 100)) {
        QMessageBox::information(this, tr("STE-CroonwolterDros Interface test"),
                                         tr("SoC setpoint should be an integer between 0 and 100"));

        return;
    }

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << controlActive;
    stream << socSetpoint;

    tcpSocket->write(data);
}

/**
 * @brief MainWindow::displayError
 *
 * This slot is called when TCPSocket throws an error.
 * Whe should stop the connection and display an error to the user. If an error
 * occured while setting up the connection to the host reenable the connectButton.
 */
void ClientWindow::displayError(QAbstractSocket::SocketError socketError) {
    qDebug() << "error occured";
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        ui->connectionLabel->setText("Host closed connection");
        ui->connectionLabel->setStyleSheet("color: red");
        break;
    case QAbstractSocket::HostNotFoundError:
        ui->connectionLabel->setText("Host not found");
        ui->connectionLabel->setStyleSheet("color: red");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        ui->connectionLabel->setText("Connection refused");
        ui->connectionLabel->setStyleSheet("color: red");
        break;
    default:
        QMessageBox::information(this, tr("Delta SM15K Client"),
                                         tr("The following error occurred: %1.")
                                         .arg(tcpSocket->errorString()));
    }
    tcpSocket->abort();
    ui->connectButton->setEnabled(true);
}

/**
 * @brief MainWindow::socketConnected
 *
 * This slot gets called when the TCP socket emits a connected signal. the connect button
 * is enabled again and a green connected message gets placed on the screen.
 * Start asking the delta for measurements every 500 msec.
 */
void ClientWindow::socketConnected(){
    ui->connectionLabel->setText("Connected");
    ui->connectionLabel->setStyleSheet("color: green");
}
