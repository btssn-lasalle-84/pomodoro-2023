#include "peripherique.h"

Peripherique::Peripherique(QString nom, QString adresseMAC, QObject* parent) :
    QObject(parent), nom(nom), adresseMAC(adresseMAC)
{
}

QString Peripherique::getNom() const
{
    return nom;
}

QString Peripherique::getAdresseMAC() const
{
    return adresseMAC;
}
