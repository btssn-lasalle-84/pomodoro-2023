#include <QApplication>
#include "pomodoro.h"

/**
 * @file main.cpp
 * @brief Programme principal
 * @details Crée et affiche la fenêtre principale de l'application Pomodoro
 * @author Pierre HIRSCH
 * @version 1.0
 *
 * @param argc
 * @param argv[]
 * @return int
 */
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    Pomodoro     pomodoro;

    pomodoro.show();

    return a.exec();
}
