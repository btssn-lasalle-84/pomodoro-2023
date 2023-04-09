#ifndef BASEDEDONNEES_H
#define BASEDEDONNEES_H

/**
 * @file basededonnees.h
 * @brief Déclaration de la classe BaseDeDonnees
 * @author Thierry VAIRA
 * @version 0.1
 */

#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>

//#define DEBUG_BDD

/**
 * @def FICHIER_BDD
 * @brief Le fichier de base de données SQLite
 */
#define FICHIER_BDD "pomodoro.sqlite"

/**
 * @class BaseDeDonnees
 * @brief Déclaration de la classe permettant l'accès à la base de données SQLite
 * @see https://fr.wikipedia.org/wiki/Singleton_(patron_de_conception)
 * @author Thierry VAIRA
 * @version 0.1
 */
class BaseDeDonnees
{
  public:
    static BaseDeDonnees* getInstance();
    static void           detruireInstance();

    bool ouvrir(QString nomBase = FICHIER_BDD);
    bool estOuverte();

    /* uniquement pour les requêtes UPDATE, INSERT et DELETE */
    bool executer(QString requete);

    /* uniquement pour les requêtes SELECT */
    bool recuperer(QString requete, QString& donnees);          // 1 -> 1
    bool recuperer(QString requete, QStringList& donnees);      // 1 -> 1..*
    bool recuperer(QString requete, QVector<QString>& donnees); // 1..* -> 1
    bool recuperer(QString               requete,
                   QVector<QStringList>& donnees); // 1..* -> 1..*

  private:
    BaseDeDonnees();
    ~BaseDeDonnees();
    QSqlDatabase          db;      //!< pour la connexion à la base de données MySQL
    static BaseDeDonnees* bdd;     //!< pointeur sur l'instance unique
    static int            nbAcces; //!< compte le nombre d'accès à l'instance unique
};

#endif // BASEDEDONNEES_H
