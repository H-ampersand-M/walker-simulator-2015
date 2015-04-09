% Walker Simulator 2015
% Maxime SCHMITT; Harenome RAZANAJATO

Utilisation
===========

```bash
$ walker-simulator-2015 -p
```

Voir le `README.pdf` pour plus d'informations sur la compilation et
l'utilisation du programme.

Détails d'implémentation
========================

Notre programme utilise deux processus :

1. L'interface : Un processus dédié aux entrées/sorties
2. Le gestionnaire : Un processus qui se charge des voies.

Entrées/sorties
---------------

L'entrée sortie utilise `readline(3)` pour l'entrée et reçoit ses informations à
partir d'une mémoire partagée. L'affichage est mis à jour lorsque le processus
reçoit un signal.

Gestion des voies
-----------------

Le processus chargé des voies se décompose en trois étapes :

1. Mise à jour de la voie et notification du processus entrée/sortie.
2. Phase d'activation de la voie.
3. Phase de sécurité.

### Mise à jour de la voie et notification du processus entrée/sortie

Lors de cette phase le processus choisit la voie la plus prioritaire (voir
[Priorités][]) s'il y a des demandes et sinon la voie suivante en round robin.
Un signal est ensuite envoyé à l'interface pour le notifier d'un changement de
voie.

### Phase d'activation de la voie

Dans un premier temps, un timer de sept secondes est lancé. Cette période
correspond à la durée minimale d'activation. Les requêtes sont temporairement
ignorées.

Ensuite la queue des requêtes est vidée pour mettre à jour le tableau des
requêtes non satisfaites.

S'il y a une requête, la phase se termine immédiatement et sinon un timer de
23 secondes est lancé. En cas de requête, la queue est vidée et la phase se
termine.

### Phase de sécurité.

Un timer d'une seconde est lancé.

Priorités
=========

Les piétons sont prioritaires sauf si une autre requête risque de rater son
échéance.
