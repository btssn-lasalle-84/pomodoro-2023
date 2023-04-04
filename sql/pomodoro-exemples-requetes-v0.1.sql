--- Exemples de requêtes SQL

-- Lise des modes

SELECT * FROM Mode;

|idMode|libelle|
|---|---|
|1|Minuteur|
|2|Chronomètre|

-- Liste des tâches

SELECT * FROM Tache;

--- Nombre de tâches

SELECT COUNT(*) AS NbTaches FROM Tache;

-- Liste des pomodoros

SELECT Tache.titre,Pomodoro.position,Etat.etat,Modele.nom,Modele.dureePomodoro,Modele.pauseCourte,Modele.pauseLongue,Modele.nbPomodori FROM Pomodoro
INNER JOIN Etat ON Etat.idEtat=Pomodoro.idEtat
INNER JOIN Tache ON Tache.idTache=Pomodoro.idTache
INNER JOIN Modele ON Modele.idModele=Pomodoro.idModele;

|titre                 |position|etat   |nom      |dureePomodoro|pauseCourte|pauseLongue|nbPomodori|
|----------------------|--------|-------|---------|-------------|-----------|-----------|----------|
|Maquette IHM          |0       |démarré|Classique|1500         |300        |1200       |4         |
|Classes du domaine    |1       |prêt   |Classique|1500         |300        |1200       |4         |
|Implémenter squelettes|2       |prêt   |Classique|1500         |300        |1200       |4         |
