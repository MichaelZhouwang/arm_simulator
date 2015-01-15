# Simlateur ARM

## Organisation

Le groupe est composé de:
- Vizzini Jeremy
- Finiel Jeremie
- Souchet Julie
- Heckmann Victor

### Le travail en équipe

Dès le premier jour, nous avons partagé nos informations personnelles et nous avons pris la décision d'utiliser Git et Github pour partager notre code.

### Organisation des taches

Le travail a été globalement réparti par fichiers :
- arm_instruction.c : tous
- arm_data_processing.c : Julie 
- arm_branch_others.c : Jérémie
- arm_load_store.c : Jérémy
- tests : Victor

## Structure du code

### Organisation des differents fichiers

Les fichiers où nous

### Ecriture des tests

Les tests ne sont pas automatisés.

## Journal

|-----|-----------------------------------------------------------------------|
| J1  | Prise en main de Git                                                  |
|     | Prise en main de la structure fournie                                 |
|     | memory.c : implementation des accès mémoire                           |
|-----|-----------------------------------------------------------------------|
| J2  | Création de tests pour les load et store                              |
|     | arm_instruction.c     : Décodage en vérifiant les conditions          |
|     | arm_branch_others.c   : B, BL et MRS                                  |
|     | arm_data_processing.c : Début du décodage des valeurs immédiates      |
|-----|-----------------------------------------------------------------------|
| J3  | Suite et fin des tests pour les load et store                         |
|     | Nettoyage du git avec .gitignore                                      |
|     | arm_load_store.c      : Debut de l'implementation de LDR / STR        |
|     | arm_data_processing.c : Ajout du calcul de la 2nde opérande           |
|     | arm_branch_others     : Implémentation de MSR                         |
|-----|-----------------------------------------------------------------------|
| J4  | Création de tests pour les branchements                               |
|     | arm_load_store.c      : Implémentation de LDR/STR {B, T}              |
|     | arm_data_processing.c : ajout de la maj des flags                     |
|-----|-----------------------------------------------------------------------|
| J5  | Intégration et première compilation et essais                         |
|     | Correction des bugs à l'aide des tests précédemments écrits           |
|     | arm_load_store.c      : factorisation du code                         |
|-----|-----------------------------------------------------------------------|
| J6  | Ecriture de tests et correction des bugs                              |
|     | Ecriture d'un script pour faciliter l'exécution des tests             |
|     | Lecture de la documentation pour les exceptions                       |
|-----|-----------------------------------------------------------------------|
| J7  | Ecriture de tests et correction des bugs                              |
|     | Création de tests pour les exceptions                                 |
|     | arm_exception.c : Implémentation des exceptions                       |
|-----|-----------------------------------------------------------------------|
| J8  | Fin des tests de toutes les instructions et validation                |
|-----|-----------------------------------------------------------------------|
| J9  | Création de programme de test pour la soutenance                      |
|     | Ecriture du compte rendu final                                        |
|-----|-----------------------------------------------------------------------|
| J10 | Controle écrit et soutenance                                          |
|-----|-----------------------------------------------------------------------|

