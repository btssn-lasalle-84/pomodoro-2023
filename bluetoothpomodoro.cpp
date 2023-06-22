#include "bluetoothpomodoro.h"
#include "peripherique.h"
#include <unistd.h>
#include <QBluetoothDeviceInfo>
#include <QDebug>
#include <QtEndian>

BluetoothPomodoro::BluetoothPomodoro() :
    nom(""), adresse(""), discoveryAgentDevice(NULL), socket(NULL), etatConnexion(false),
    etatRecherche(false), connexionErreur(false), peripheriqueDetecte(false)
{
    qDebug() << Q_FUNC_INFO;
    if(!peripheriqueLocal.isValid())
    {
        qCritical("Bluetooth désactivé !");
        return;
    }

    peripheriqueLocal.powerOn();
    nom     = peripheriqueLocal.name();
    adresse = peripheriqueLocal.address().toString();
    peripheriqueLocal.setHostMode(QBluetoothLocalDevice::HostDiscoverable);
    discoveryAgentDevice = new QBluetoothDeviceDiscoveryAgent(this);
    // Slot pour la recherche de périphériques Bluetooth
    connect(discoveryAgentDevice,
            SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this,
            SLOT(ajouterPeripherique(QBluetoothDeviceInfo)));
    connect(discoveryAgentDevice, SIGNAL(finished()), this, SLOT(rechercheTerminee()));
    connect(discoveryAgentDevice,
            SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
            this,
            SLOT(rechercheErreur(QBluetoothDeviceDiscoveryAgent::Error)));

    // qDebug() << peripheriqueLocal.connectedDevices();
}

BluetoothPomodoro::~BluetoothPomodoro()
{
    deconnecterPeripherique();
    qDeleteAll(peripheriquesDistants);
    qDebug() << Q_FUNC_INFO;
}

void BluetoothPomodoro::rechercher()
{
    qDebug() << Q_FUNC_INFO;
    qDeleteAll(peripheriquesDistants);
    peripheriquesDistants.clear();
    peripheriqueDetecte = false;
    emit detecte();
    emit peripheriquesUpdated();

    if(discoveryAgentDevice != NULL)
    {
        discoveryAgentDevice->start();
        if(discoveryAgentDevice->isActive())
        {
            etatRecherche = true;
            emit recherche();
        }
    }
}

void BluetoothPomodoro::arreter()
{
    qDebug() << Q_FUNC_INFO;
    if(!etatRecherche)
        return;

    if(discoveryAgentDevice != NULL && discoveryAgentDevice->isActive())
        discoveryAgentDevice->stop();
}

bool BluetoothPomodoro::estValide()
{
    return peripheriqueLocal.isValid();
}

bool BluetoothPomodoro::estConnecte()
{
    if(!socket)
        return false;
    return socket->isOpen();
}

QString BluetoothPomodoro::getNom()
{
    return nom;
}

QString BluetoothPomodoro::getAdresse()
{
    return adresse;
}

QVariant BluetoothPomodoro::getPeripheriques()
{
    return QVariant::fromValue(peripheriquesDistants);
}

void BluetoothPomodoro::ajouterPeripherique(const QBluetoothDeviceInfo& info)
{
    qDebug() << Q_FUNC_INFO << info.name() << info.address().toString();
    Peripherique* p = new Peripherique(info.name(), info.address().toString(), this);
    peripheriquesDistants.append(p);
    peripheriqueDetecte = true;
    emit detecte();
}

void BluetoothPomodoro::rechercheTerminee()
{
    etatRecherche = false;
    emit recherche();
    emit detecte();
    emit peripheriquesUpdated();
    if(peripheriquesDistants.isEmpty())
        qDebug() << Q_FUNC_INFO << "recherche terminee : aucun peripherique bluetooth trouve !";
    else
        qDebug() << Q_FUNC_INFO << "recherche terminee";
}

void BluetoothPomodoro::rechercheErreur(QBluetoothDeviceDiscoveryAgent::Error erreur)
{
    qDebug() << Q_FUNC_INFO << erreur;
}

void BluetoothPomodoro::connecterPeripherique(const QString& adresse)
{
    qDebug() << Q_FUNC_INFO;
    if(!socket)
    {
        socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
        connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
        connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
        connect(socket,
                SIGNAL(error(QBluetoothSocket::SocketError)),
                this,
                SLOT(socketError(QBluetoothSocket::SocketError)));
    }
    else if(socket->isOpen())
    {
        socket->close();
    }
    QBluetoothUuid uuid = QBluetoothUuid(QBluetoothUuid::SerialPort);
    socket->connectToService(QBluetoothAddress(adresse), uuid);
    socket->open(QIODevice::ReadWrite);
}

void BluetoothPomodoro::deconnecterPeripherique()
{
    qDebug() << Q_FUNC_INFO;
    if(!socket)
        return;
    if(socket->isOpen())
        socket->close();
}

void BluetoothPomodoro::socketConnected()
{
    qDebug() << Q_FUNC_INFO;
    etatConnexion   = true;
    connexionErreur = false;
    emit connecte();
    emit erreur();
    qDebug() << peripheriqueLocal.connectedDevices();
}

void BluetoothPomodoro::socketDisconnected()
{
    qDebug() << Q_FUNC_INFO;
    etatConnexion   = false;
    connexionErreur = false;
    emit connecte();
    emit erreur();
}

void BluetoothPomodoro::socketReadyRead()
{
    qDebug() << Q_FUNC_INFO;
    QByteArray donnees;

    while(socket->bytesAvailable())
    {
        donnees += socket->readAll();
        usleep(150000); // cf. timeout
    }
    emit trameRecue(donnees);
}

void BluetoothPomodoro::socketError(QBluetoothSocket::SocketError error)
{
    qDebug() << Q_FUNC_INFO << error;
    etatConnexion   = false;
    connexionErreur = true;
    emit connecte();
    emit erreur();
}

void BluetoothPomodoro::envoyer(QString trame)
{
    qDebug() << Q_FUNC_INFO;
    if(!socket || !socket->isOpen())
    {
        return;
    }
    trame += "\r";
    socket->write(trame.toLatin1());
}
