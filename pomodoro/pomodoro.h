/**
 * @file pomodoro.h
 *
 * @brief Déclaration de la classe Pomodoro
 * @author Pierre HIRSCH
 * @version 0.2
 */
#ifndef POMODORO_H
#define POMODORO_H

#include <QtWidgets>

/**
 * @def RASPBERRY_PI
 * @brief Pour le mode plein écran sur la Raspberry Pi
 */
//#define RASPBERRY_PI

// Valeur par défaut (en minutes)
#define DUREE_POMODORO        20 // 01 à 59
#define DUREE_PAUSE_COURTE    5  // 01 à 59
#define DUREE_PAUSE_LONGUE    15 // 01 à 59
#define NB_CYCLES             1  // 01 à 10
#define MODE_PAUSE_AUTOMATIQE true
#define MODE_PAUSE_AUTOMATIQE true

#define TOUTES_LES_SECONDES 1000

/**
 * @def DEMO
 * @brief En mode démo, le temps est accéléré (minutes = secondes)
 */
#define DEMO

namespace Ui
{
class GUIPomodoro;
}

class BaseDeDonnees;

/**
 * @class Pomodoro
 * @brief Affiche les différents écrans de l'application Pomodoro
 */
class Pomodoro : public QWidget
{
    Q_OBJECT
    /**
     * @enum Ecran
     * @brief Les différents écrans de la GUI
     */
    enum Ecran
    {
        Accueil,
        Tache,
        Cycle,
        Session,
        NbEcrans,
    };
    /**
     * @enum Mode
     * @brief Les différents modes
     */
    enum Mode
    {
        Minuteur,
        Chronometre,
        NbModes,
    };

  public:
    Pomodoro(QWidget* parent = nullptr);
    ~Pomodoro();

  private:
    Ui::GUIPomodoro* ui;          //!< la fenêtre graphique associée à cette classe
    BaseDeDonnees*   bddPomodoro; //!< l'objet BaseDeDonnees
    int              tempsRestant;
    QTimer*          compteRebours;
    bool             pomodoro;
    bool             pause;

    void initialiserGui();
    void ouvrirBaseDeDonnees();
    void connecterSignauxSlots();

  public slots:
    void afficherEcran(Pomodoro::Ecran ecran);
    void afficherEcranAccueil();
    void afficherEcranTache();
    void afficherEcranCycle();
    void afficherEcranSession();
    void afficherTexteTache();
    void mettreAJourAffichageDuree();

  private slots:
    void demarrerPomodoro();
    void arreterPomodoro();
    void gererBoutonPomodoro();
};

#endif // POMODORO_H
