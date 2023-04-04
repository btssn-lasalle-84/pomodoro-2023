#ifndef POMODORO_H
#define POMODORO_H

/**
 * @file pomodoro.h
 *
 * @brief Déclaration de la classe Pomodoro
 * @author Pierre HIRSCH
 * @version 0.1
 */

#include <QtWidgets>

/**
 * @def RASPBERRY_PI
 * @brief Pour le mode plein écran sur la Raspberry Pi
 */
//#define RASPBERRY_PI

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
        Configuration,
        NbEcrans
    };

  public:
    Pomodoro(QWidget* parent = nullptr);
    ~Pomodoro();

  private:
    Ui::GUIPomodoro* ui;          //!< la fenêtre graphique associée à cette classe
    BaseDeDonnees*   bddPomodoro; //!< l'objet BaseDeDonnees

    void initialiserGui();
    void ouvrirBaseDeDonnees();
    void connecterSignauxSlots();

  public slots:
    void afficherEcran(Pomodoro::Ecran ecran);
    void afficherEcranAccueil();
    void afficherEcranConfiguration();
};

#endif // POMODORO_H
