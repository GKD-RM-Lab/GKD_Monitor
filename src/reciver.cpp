
#include <QtCore/qobject.h>
#include <QtCore/qstringview.h>
#include <QtCore/qvariant.h>
#include <QtWidgets/qmessagebox.h>
#include <cstdint>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <iostream>

#include "log_box.h"
#include "reciver.h"
#include "mainwindow.h"
#include "valve_manager.h"
Reciver::Reciver(QObject *parent)
    : QObject(parent)
{
    _server = new QTcpServer(this);

    connect(
        _server,
        &QTcpServer::newConnection,
        this,
        &Reciver::onNewConnection
    );


};

void Reciver::listen(int port){
    _server->listen(QHostAddress::Any,port);
}

void Reciver::onNewConnection(){
    while(_server->hasPendingConnections()){
        QTcpSocket *socket = _server->nextPendingConnection();
        new Connection(socket,_mainWindow,this);
        QString info = tr("New connection from %1:%2").arg(socket->peerAddress().toString(),QString::number(socket->peerPort()));
        QMessageBox::information(nullptr,tr("Information"),info);
    }
}

Connection::Connection(QTcpSocket* socket,MainWindow *mainWindow,QObject *parent)
    : QObject(parent)
    , _socket(socket)
    , _mainWindow(mainWindow)
{
    connect(
        _socket,
        &QTcpSocket::readyRead,
        this,
        &Connection::onReadyRead
    );

    connect(
        _socket,
        &QTcpSocket::disconnected,
        this,
        &Connection::deleteLater
    );
}

void Connection::onReadyRead(){
    _buffer += _socket->readAll();

    if(_buffer.size() < 2)
        return;

    
    _packageSize = (_buffer[1] << 8) + _buffer[0];

    if(_buffer.size() >= _packageSize){
        processData();
        _packageSize = 0;
    }
}

void Connection::processData(){ 
    QByteArray data = _buffer.mid(0,_packageSize);
    _buffer.remove(0, _packageSize);

    QDataStream stream{data};
    stream.setByteOrder(QDataStream::LittleEndian);

    stream.skipRawData(2); // 数据包长度

    quint8 type;
    stream >> type;

    switch(type){
        case MessageType::RegisterName:
            processRegisterName(stream);
            break;
        case MessageType::UpdateValue:
            processUpdateValue(stream);
            break;
        case MessageType::ConsoleMessage:
            processConsoleMessage(stream);
            break;
        case MessageType::MessageBox:
            processMessageBox(stream);
            break;
    }
}

void Connection::processRegisterName(QDataStream& stream){
    uint8_t nameLength;
    uint32_t id;
    char* rawName;

    stream >> id;
    stream >> nameLength;
    rawName = new char[nameLength];
    stream.readRawData(rawName, nameLength);

    std::string name{rawName,nameLength};

    std::cout << "RegisterName: " << id << " " << name << std::endl;
    valueManager.registerName(name,id);
}
void Connection::processUpdateValue(QDataStream& stream){
    ValueType value;
    uint32_t id;

    stream >> id;
    stream >> value;

    if(!valueManager.name(id).starts_with("rc"))
        std::cout << "UpdateValue: " << valueManager.name(id) << " " << value << std::endl;

    valueManager.updateValue(id,value);
}

void Connection::processConsoleMessage(QDataStream& stream){
    uint16_t messageSize;
    char* message;

    stream >> messageSize;
    message = new char[messageSize];
    stream.readRawData(message, messageSize);

    _mainWindow->logBox()->addText(QString::fromStdString({message,messageSize}));
    delete[] message;
}
void Connection::processMessageBox(QDataStream& stream){
    uint16_t messageSize;
    char* message;

    stream >> messageSize;
    message = new char[messageSize];
    stream.readRawData(message, messageSize);

    QMessageBox::information(nullptr,tr("Message"),QString::fromStdString({message,messageSize}));
    delete[] message;
}