#!/usr/bin/env python
# -*- encoding: utf-8 -*-

from os import system

def readIntInRange( low, high ):
    while True:
        try:
            n = int( raw_input('Votre choix: ') )
            if n >= low and n <= high:
                return n
        except Exception:
            pass

PATTERN = """#!/usr/bin/env sh
mkdir -p %(MODE)s
cd %(MODE)s
cmake %(OPTIONS)s ..
make
cd ..
"""

options=[
    ("Choisissez votre mode de compilation :",
        ["release", "debug"],
        ["MODE", "BUILD_FLAGS"],
        [
            ["release", "debug"],
            ["", "-DCMAKE_BUILD_TYPE=Debug"]
        ]
    ),
    ("Voulez-vous utiliser OpenMP avec CPT ?",
        ["oui", "non"],
        ["CPT_OMP"],
        [
            ["-DCPT_WITH_OMP=1", ""]
        ]
    ),
    ("Voulez-vous utiliser OpenMP avec DAE ?",
        ["oui", "non"],
        ["DAE_OMP"],
        [
            ["-DDAE_WITH_OMP=1", ""]
        ]
    )
]

OPTIONS = "%(CPT_OMP)s %(DAE_OMP)s %(BUILD_FLAGS)s"

responses = {}

for question in options:
    print question[0]
    i = 1
    for answer in question[1]:
        print i, ")", answer
        i += 1
    choice = readIntInRange( 1, i-1 ) - 1
    for i in range(len(question[2])):
        responses[ question[2][i] ] = question[3][i][choice]


responses[ "OPTIONS" ] = OPTIONS % responses
del responses['CPT_OMP']
del responses['DAE_OMP']
del responses['BUILD_FLAGS']

print "\n\nScript bash correspondant : "
print PATTERN % responses

print "\nVoulez vous enregistrer le script dans un fichier ou bien l'exécuter directement ?"
print "1) Enregistrer"
print "2) Exécuter"
print "3) Ne rien faire et quitter"
choice = readIntInRange( 1, 3 )

if choice == 1:
    filename = raw_input("Choisissez un nom de fichier: (build.sh par défaut)")
    if filename == "":
        filename = "build.sh"
    script = file( filename, 'w' )
    script.write( PATTERN % responses )
    script.close()
    print "N'oubliez pas d'effectuer un chmod +x pour pouvoir lancer le script."
elif choice == 2:
    system( PATTERN % responses )
