#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QtEndian>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    tcpServer(new QTcpServer(this)),
    isClientConnected(false),
    socSetpoint(100)
    //sendTimer(new QTimer(this))
{
    ui->setupUi(this);
    //connect(sendTimer, &QTimer::timeout, this, &MainWindow::sendData);
}

MainWindow::~MainWindow() {
    tcpServer->close();
    tcpServer->deleteLater();
    delete ui;
}

/**
 * @brief MainWindow::on_sendButton_clicked
 *
 * Send the data from the form (representing the car) to croonwolter & dros.
 */
void MainWindow::on_sendButton_clicked()
{
    if (!isClientConnected){
        QMessageBox::information(this, tr("STE-CroonwolterDros Interface test"),
                                         tr("Client is not connected."));
        return;
    }
    bool conversionOk;

    int32_t soc = ui->carSoCEdit->text().toInt(&conversionOk);
    if(!conversionOk || !(0 <= soc && soc <= 100)) {
        QMessageBox::information(this, tr("STE-CroonwolterDros Interface test"),
                                         tr("State of Charge should be an integer between 0 and 100."));
        return;

    }

    int32_t power = ui->actualPowerEdit->text().toInt(&conversionOk);
    if(!conversionOk || !(-15000 <= power && power <= 15000)) {
        QMessageBox::information(this, tr("STE-CroonwolterDros Interface test"),
                                         tr("Actual power should be an integer between -15000 and 15000."));
        return;
    }

    bool connected = ui->coupledTrueRadioButton->isChecked();
    bool local = ui->localControlTrueButton->isChecked();

    vieData myData;
    myData.connected = connected;
    myData.local = local;
    myData.power = qToBigEndian(power);
    myData.setpoint = qToBigEndian(socSetpoint);
    myData.soc = qToBigEndian(soc);

    client->write(reinterpret_cast<char*>(&myData), sizeof(vieNetworkData));
    client->flush();
}

/**
 * @brief MainWindow::on_startButton_clicked
 *
 * The user clicked the start server button, handle this click by starting a server.
 * A TCP server can only be created on valid ports (1 - 65535), on succesfully creating
 * a server display this to the user on the UI.
 */
void MainWindow::on_startButton_clicked() {
    ui->startButton->setEnabled(false);
    QString port = ui->portEdit->text();
    bool ok;
    int portNr = port.toInt(&ok);

    //port should be int and between 0 and 65535
    if(!ok || !(0 < portNr && portNr < 65535)){
        ui->startButton->setEnabled(true);
        ui->connectedLabel->setText("Disconnected");
        ui->connectedLabel->setStyleSheet("color: red");
        QMessageBox::information(this, tr("STE-CroonwolterDros Interface test"),
                                         tr("Port should be an integer between 1 and 65535"));

        return;
    }
    if (!tcpServer->listen(QHostAddress::Any, portNr)){
        ui->startButton->setEnabled(true);
        ui->connectedLabel->setText("Disconnected");
        ui->connectedLabel->setStyleSheet("color: red");
        QMessageBox::information(this, tr("STE-CroonwolterDros Interface test"),
                                         tr("Could not start the server: %1.").arg(tcpServer->errorString()));
        return;
    }

    ui->connectedLabel->setText("Waiting for connection");
    ui->connectedLabel->setStyleSheet("color: orange");
    connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::handleNewConnection);
}

/**
 * @brief MainWindow::handleNewConnection
 * A new client is trying to connect to the server, we only allow one active connection
 * for simplicity reasons. Hence drop the connection if there is already a client connected
 * else accept the socket by connecting the signals with the slots of this application.
 * And showing that a client was connected on the UI.
 */
void MainWindow::handleNewConnection(){
    if (isClientConnected){
        // We only accept one client in this setting, so disconnect the new client.
        QTcpSocket *newClient = tcpServer->nextPendingConnection();
        connect(newClient, &QAbstractSocket::disconnected, newClient, &QObject::deleteLater);
        newClient->disconnectFromHost();
        QMessageBox::information(this, tr("STE-CroonwolterDros Interface test"),
                                         tr("A new client connected, while there was already a client connected. The new client has been disconnected."));

    } else {
        isClientConnected = true;
        client = tcpServer->nextPendingConnection();
        connect(client, &QAbstractSocket::disconnected, this, &MainWindow::handleClientDisconnection);
        connect(client, &QAbstractSocket::disconnected, this, &QObject::deleteLater);
        connect(client, &QIODevice::readyRead, this, &MainWindow::readResponse);
        ui->connectedLabel->setText("Client connected");
        ui->connectedLabel->setStyleSheet("color: green");
        //sendTimer->start(500);
    }
}

/**
 * @brief MainWindow::handleClientDisconnection
 *
 * Handle the disconnection of the client by showing on the UI that the server
 * is waiting for a connection.
 */
void MainWindow::handleClientDisconnection() {
    ui->connectedLabel->setText("Waiting for connection");
    ui->connectedLabel->setStyleSheet("color: orange");
    //sendTimer->stop();
    isClientConnected = false;
}

void MainWindow::readResponse() {
    if (client->bytesAvailable() >= 5) {
        char networkBytes[5];
        client->read(networkBytes, 5);
        cdData hostData;
        hostData.active = networkBytes[0];
        hostData.setpoint = (networkBytes[4] << 24) | (networkBytes[3] << 16) | (networkBytes[2] << 8) | (networkBytes[1]);
        socSetpoint = hostData.setpoint;

        ui->currentSetpointLabel->setText(QString(socSetpoint));
        ui->receiveControlLabel->setText(QString(hostData.active));
        ui->receiveSetpointLabel->setText(QString(socSetpoint));
    }
}
/*
void MainWindow::sendData() {
    if (isClientConnected) {

    } else {
        qDebug() << "Warining: Timer fired after client was disconnected";
        return;
    }
}
*/


