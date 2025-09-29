#pragma once
#include <QObject>
#include <QByteArray>

class MainWindow;
class QUdpSocket;

class Reciver : public QObject
{
    Q_OBJECT
public:
    Reciver(QObject *parent = nullptr);

    inline void setMainWindow(MainWindow *mainWindow){
        _mainWindow = mainWindow;
    }

private:

    enum MessageType{
        RegisterName = 0x00,
        UpdateValue = 0x01,
        ConsoleMessage = 0x02,
        MessageBox = 0x03,
    };

public slots:
    void listen(int port);

private slots:
    void onReadyRead();
    void processData(const QByteArray& data);

    void processRegisterName(QDataStream& stream);
    void processUpdateValue(QDataStream& stream);
    void processConsoleMessage(QDataStream& stream);
    void processMessageBox(QDataStream& stream);
    
private:
    QUdpSocket *_server;
    MainWindow *_mainWindow;
};