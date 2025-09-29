
#include <QtCore/qobject.h>
#include <QtCore/qstringview.h>
#include <QtCore/qvariant.h>
#include <QtNetwork/qhostaddress.h>
#include <QtNetwork/qnetworkdatagram.h>
#include <QtNetwork/qudpsocket.h>
#include <QtWidgets/qmessagebox.h>
#include <cstdint>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <iostream>
#include <QUdpSocket>
#include <QNetworkDatagram>

#include "log_box.h"
#include "reciver.h"
#include "mainwindow.h"
#include "valve_manager.h"
Reciver::Reciver(QObject *parent)
    : QObject(parent)
{
    _server = new QUdpSocket(this);

    connect(
        _server,
        &QUdpSocket::readyRead,
        this,
        &Reciver::onReadyRead
    );


};

void Reciver::listen(int port){
    if(!_server->bind(QHostAddress::AnyIPv4,port)){
        QMessageBox::critical(nullptr,tr("Error"),tr("Can't bind to port %1").arg(port));
    }
}

void Reciver::onReadyRead(){
    while(_server->hasPendingDatagrams()){
        QNetworkDatagram datagram = _server->receiveDatagram();
        processData(datagram.data());
    }
}

void Reciver::processData(const QByteArray& data){ 

    QDataStream stream{data};
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

void Reciver::processRegisterName(QDataStream& stream){
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
void Reciver::processUpdateValue(QDataStream& stream){
    ValueType value;
    uint32_t id;

    stream >> id;
    stream >> value;

    // if(!valueManager.name(id).starts_with("rc"))
    //     std::cout << "UpdateValue: " << valueManager.name(id) << " " << value << std::endl;

    valueManager.updateValue(id,value);
}

void Reciver::processConsoleMessage(QDataStream& stream){
    uint16_t messageSize;
    char* message;

    stream >> messageSize;
    message = new char[messageSize];
    stream.readRawData(message, messageSize);

    _mainWindow->logBox()->addText(QString::fromStdString({message,messageSize}));
    delete[] message;
}
void Reciver::processMessageBox(QDataStream& stream){
    uint16_t messageSize;
    char* message;

    stream >> messageSize;
    message = new char[messageSize];
    stream.readRawData(message, messageSize);

    QMessageBox::information(nullptr,tr("Message"),QString::fromStdString({message,messageSize}));
    delete[] message;
}