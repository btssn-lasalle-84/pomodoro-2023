#ifndef BLUETOOTHPOMODORO_H
#define BLUETOOTHPOMODORO_H

#include <QBluetoothLocalDevice>
#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QBluetoothSocket>
#include <QBluetoothDeviceInfo>
#include <QBluetoothServiceInfo>
#include <QBluetoothDeviceDiscoveryAgent>

#define SERVICE_UUID "{00001101-0000-1000-8000-00805F9B34FB}"

class BluetoothPomodoro : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString nom MEMBER nom NOTIFY detecte)
    Q_PROPERTY(QString adresse MEMBER adresse NOTIFY detecte)
    Q_PROPERTY(bool peripheriqueDetecte MEMBER peripheriqueDetecte NOTIFY detecte)
    Q_PROPERTY(bool etatRecherche MEMBER etatRecherche NOTIFY recherche)
    Q_PROPERTY(bool etatConnexion MEMBER etatConnexion NOTIFY connecte)
    Q_PROPERTY(bool connexionErreur MEMBER connexionErreur NOTIFY erreur)
    Q_PROPERTY(QVariant listePeripheriques READ getPeripheriques NOTIFY peripheriquesUpdated)

  public:
    BluetoothPomodoro();
    ~BluetoothPomodoro();

    Q_INVOKABLE void rechercher();
    Q_INVOKABLE void arreter();
    Q_INVOKABLE bool estValide();
    Q_INVOKABLE bool estConnecte();
    Q_INVOKABLE void connecterPeripherique(const QString& adresse);
    Q_INVOKABLE void deconnecterPeripherique();
    Q_INVOKABLE void envoyer(QString trame);

    QString  getNom();
    QString  getAdresse();
    QVariant getPeripheriques();

  protected slots:
    // recherche
    void ajouterPeripherique(const QBluetoothDeviceInfo&);
    void rechercheTerminee();
    void rechercheErreur(QBluetoothDeviceDiscoveryAgent::Error erreur);
    // socket
    void socketConnected();
    void socketDisconnected();
    void socketReadyRead();
    void socketError(QBluetoothSocket::SocketError erreur);

  private:
    QBluetoothLocalDevice peripheriqueLocal;
    QString               nom;
    QString               adresse;
    // QList<QBluetoothAddress> peripheriquesDistants;
    QList<QObject*>                 peripheriquesDistants;
    QBluetoothDeviceDiscoveryAgent* discoveryAgentDevice;
    QBluetoothSocket*               socket;
    bool                            etatConnexion;
    bool                            etatRecherche;
    bool                            connexionErreur;
    bool                            peripheriqueDetecte;

  signals:
    void connecte();
    void recherche();
    void erreur();
    void detecte();
    void peripheriquesUpdated();
    void trameRecue(QString trame);
};

#endif // BluetoothPomodoro_H
