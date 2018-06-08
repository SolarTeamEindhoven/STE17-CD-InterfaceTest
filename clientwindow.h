#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

namespace Ui {
class ClientWindow;
}

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientWindow(QWidget *parent = 0);
    ~ClientWindow();

private slots:
    void on_connectButton_clicked();
    void readResponse();
    void on_sendButton_clicked();
    void displayError(QAbstractSocket::SocketError socketError);
    void socketConnected();

private:
    Ui::ClientWindow *ui;
    QTcpSocket* tcpSocket = nullptr;

};

#endif // CLIENTWINDOW_H
