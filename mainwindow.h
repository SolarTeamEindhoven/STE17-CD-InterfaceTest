#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_sendButton_clicked();
    void on_startButton_clicked();
    void handleNewConnection();
    void handleClientDisconnection();
    //void sendData();
    void readResponse();

private:
    Ui::MainWindow *ui;
    QTcpServer *tcpServer = nullptr;
    QTcpSocket *client = nullptr;
    bool isClientConnected;
    int32_t socSetpoint;
    //QTimer *sendTimer = nullptr;

#pragma pack(push, 1)
    struct vieData {
        bool connected;
        int32_t soc;
        bool local;
        int32_t power;
        int32_t setpoint;
    }vieNetworkData;
#pragma pack(pop)

#pragma pack(push, 1)
    struct cdData {
        bool active;
        int32_t setpoint;
    }cdHostData;
#pragma pack(pop)
};
#endif // MAINWINDOW_H
