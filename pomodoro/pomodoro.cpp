/**
 * @file pomodoro.cpp
 *
 * @brief Définition de la classe Pomodoro
 * @author Pierre HIRSCH
 * @version 0.1
 */

#include "pomodoro.h"
#include "basededonnees.h"
#include "ui_pomodoro.h"

/**
 * @brief Constructeur de la classe Pomodoro
 *
 * @fn Pomodoro::Pomodoro
 * @param parent nullptr pour définir la fenêtre principale de l'application
 */
Pomodoro::Pomodoro(QWidget* parent) : QWidget(parent), ui(new Ui::GUIPomodoro)
{
    qDebug() << Q_FUNC_INFO;

    ouvrirBaseDeDonnees();
    initialiserGui();
    connecterSignauxSlots();
}

/**
 * @brief Destructeur de la classe Pomodoro
 *
 * @fn Pomodoro::~Pomodoro
 * @details Libère les ressources de l'application
 */
Pomodoro::~Pomodoro()
{
    delete ui;
    BaseDeDonnees::detruireInstance();
    qDebug() << Q_FUNC_INFO;
}

/**
 * @fn Pomodoro::afficherEcran(Pomodoro::Ecran ecran)
 * @brief Selectionne et affiche l'écran
 */
void Pomodoro::afficherEcran(Pomodoro::Ecran ecran)
{
    ui->ecrans->setCurrentIndex(ecran);
}

/**
 * @fn Pomodoro::afficherEcranAccueil()
 * @brief Affiche l'écran d'accueil
 */
void Pomodoro::afficherEcranAccueil()
{
    ui->labelTitre->setText("Pomodoro");
    afficherEcran(Pomodoro::Ecran::Accueil);
}

/**
 * @fn Pomodoro::afficherEcranTache()
 * @brief Affiche l'écran d'une tâche
 */
void Pomodoro::afficherEcranTache()
{
    ui->labelTitre->setText("Édition d'une tâche");
    afficherEcran(Pomodoro::Ecran::Tache);
}

void Pomodoro::initialiserGui()
{
    ui->setupUi(this);
#ifdef RASPBERRY_PI
    showFullScreen();
#else
    setFixedSize(qApp->desktop()->availableGeometry(this).width(),
                 qApp->desktop()->availableGeometry(this).height());
    // showMaximized();
#endif
    afficherEcranAccueil();
}

/**
 * @fn Pomodoro::ouvrirBaseDeDonnees()
 * @brief Instancie l'objet et l'accès à base de données
 */
void Pomodoro::ouvrirBaseDeDonnees()
{
    bddPomodoro = BaseDeDonnees::getInstance();
    bddPomodoro->ouvrir();
    qDebug() << Q_FUNC_INFO << bddPomodoro->estOuverte();
}

/**
 * @fn Pomodoro::connecterSignauxSlots()
 * @brief Connecte les signaux aux slots pour l'application
 */
void Pomodoro::connecterSignauxSlots()
{
    connect(ui->boutonEditionTache, SIGNAL(clicked(bool)), this, SLOT(afficherEcranTache()));
    // Pour les tests
    connect(ui->boutonValidationEditionTache,
            SIGNAL(clicked(bool)),
            this,
            SLOT(afficherEcranAccueil()));
    connect(ui->boutonAnnulationEditionTache,
            SIGNAL(clicked(bool)),
            this,
            SLOT(afficherEcranAccueil()));
}
