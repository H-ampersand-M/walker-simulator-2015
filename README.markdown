[Walker Simulator 2015][]
=========================

Walker Simulator, 2015 premium edition.

Projet de l'UE « Système & Programmation Temps-Réel », Master 1 RISE.

[Université de Strasbourg][], [UFR Mathématique-Informatique][] 2014-2015.

Récupérer Walker Simulator 2015
-------------------------------

```bash
git clone https://github.com/H-ampersand-M/walker-simulator-2015.git
```

Pré-requis
----------

### Nécessaires

- Outils de développement GNU
- Readline

#### Ubuntu

```bash
$ apt-get install build-essential libreadline-dev
```

#### Fedora

```bash
$ yum groupinstall "Development Tools"
$ yum install man-pages readline readline-devel
```

### Optionnels

- Cmake
- Doxygen
- Graphviz / Dot

#### Ubuntu

```bash
$ apt-get install doxygen graphviz cmake
```

#### Fedora

```bash
$ yum install doxygen graphviz cmake
```

Compilation
-----------

2 solutions :

- utiliser l'avant-gardiste `cmake`
- ne pas utiliser `cmake`

Note : `make install` peut nécessiter les droits `root`.

### `cmake`

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
$ make install
```

### Pas `cmake`

```bash
$ ./configure
$ make
$ make install
```

Lancement
---------

Par défaut, l'éxécutable se trouvera dans `build/bin` :

```bash
$ build/bin/walker-simulator-2015
```

Documentation
-------------

### Utilisateurs

Si `make install` a été éxécuté ou si `man/` se
trouve dans la variable d'environnement `$MANPATH` :

```bash
$ man walker
$ man walker-simulator-2015
```

Sinon :

```bash
$ man man/man1/walker.1
$ man man/man1/walker-simulator-2015.1
```

### Développeurs

```bash
$ make doc
```

Archiver
--------

```bash
$ make distclean
$ git archive --format=tar HEAD | gzip >success.tar.gz
```

License
-------
Copyright © 2015 SCHMITT Maxime, RAZANAJATO RANAIVOARIVONY Harenome

Ce projet est libre. Vous pouvez le redistribuer ou le modifier selon les termes
de la license « Do What The Fuck You Want To Public License », Version 2, comme
publiée par Sam Hocevar. Pour de plus amples informations, veuillez vous référer
au fichier COPYING, ou bien http://www.wtfpl.net/.

[Université de Strasbourg]: https://www.unistra.fr
[UFR Mathématique-Informatique]: https://mathinfo.unistra.fr/
[Walker Simulator 2015]: https://github.com/h-ampersand-m/walker-simulator-2015
