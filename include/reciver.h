#pragma once
#include <QObject>
#include <QBuffer>
#include <cstddef>

class MainWindow;
class QTcpSocket;
class QTcpServer;

class Reciver : public QObject
{
    Q_OBJECT
public:
    Reciver(QObject *parent = nullptr);

    inline void setMainWindow(MainWindow *mainWindow){
        _mainWindow = mainWindow;
    }

public slots:
    void listen(int port);

private slots:
    void onNewConnection();
    
private:
    QTcpServer *_server;
    MainWindow *_mainWindow;
};

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QTcpSocket* socket,MainWindow* mainwindow,QObject *parent = nullptr);
    
private slots:
    void onReadyRead();

private:
    void processData();
    enum MessageType{
        RegisterName = 0x00,
        UpdateValue = 0x01,
        ConsoleMessage = 0x02,
        MessageBox = 0x03,
    };

    void processRegisterName(QDataStream& stream);
    void processUpdateValue(QDataStream& stream);
    void processConsoleMessage(QDataStream& stream);
    void processMessageBox(QDataStream& stream);

private:
    QTcpSocket *_socket;
    QByteArray _buffer;
    int _packageSize;
    MainWindow *_mainWindow;
};