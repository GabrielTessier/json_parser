## Installation
```sh
# Clone le repo
git clone https://github.com/GabrielTessier/json_parser.git

# Compiler
cd json_parser
make
```

## Pour tester
```sh
cd tests
./json-01-test
```

## Pour utiliser la lib dans un projet
Option à ajouter à gcc :
- `-Lpath/to/repo/lib -ljson`
- `-Ipath/to/repo/include`
(en remplaçant `path/to/repo` par le chemin vers le repo `json_parser`)

Ajouter l'entête `#include <json.h>` dans les fichiers `.c`
