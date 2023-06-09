#ifndef PERIPHERIQUE_H
#define PERIPHERIQUE_H

#include <QObject>

class Peripherique : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString nom READ getNom NOTIFY peripheriqueChanged)
    Q_PROPERTY(QString adresseMAC READ getAdresseMAC NOTIFY peripheriqueChanged)

  public:
    explicit Peripherique(QString nom, QString adresseMAC, QObject* parent = nullptr);
    QString getNom() const;
    QString getAdresseMAC() const;

  private:
    QString nom;
    QString adresseMAC;

  signals:
    void peripheriqueChanged();

  public slots:
};

#endif // Peripherique_H
