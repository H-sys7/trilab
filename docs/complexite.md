# La complexité, en clair

À lire une fois l'exercice 07 validé : tu as mesuré, maintenant on nomme.

La complexité ne mesure pas des secondes : elle dit comment le travail grandit quand la
quantité de données grandit.

## L'idée, avec une image

Tu dois ranger un paquet de n cartes. Méthode A : pour chaque carte, tu la compares à
presque toutes les autres, soit environ n × n opérations. Méthode B : tu coupes le paquet
en deux, ranges chaque moitié, puis fusionnes, soit environ n × log₂(n) opérations.

Pour 10 cartes, la différence est faible. Pour un million, c'est la différence entre une
fraction de seconde et un quart d'heure.

## Pourquoi on ne compte pas en secondes

Le temps dépend de la machine, du compilateur et de ce qui tourne à côté. On compte plutôt
les opérations de base : comparaisons, échanges, décalages. C'est le rôle de
`insertion_sort_counted` dans l'exercice 07.

## La notation O

On garde seulement le terme qui grandit le plus vite et on retire les constantes. Le tri
par insertion sur un tableau inversé fait exactement ce nombre de comparaisons :

    n(n − 1) / 2  =  n²/2 − n/2

Le terme qui domine est n², donc on écrit O(n²). Le « divisé par 2 » et le « moins n/2 »
ne changent presque rien quand n devient grand.

## Le test du doublement

C'est la colonne « ratio » de `make bench`, et le moyen le plus simple de reconnaître une
complexité.

| Si n double, le temps est multiplié par… | Complexité | Nom courant |
| --- | --- | --- |
| environ 1 | O(1) | constante |
| environ 2 | O(n) | linéaire |
| un peu plus de 2 | O(n log n) | quasi linéaire |
| environ 4 | O(n²) | quadratique |

## Les ordres de grandeur

| n | n log₂ n | n² |
| --- | --- | --- |
| 1 000 | ≈ 10 000 | 1 000 000 |
| 1 000 000 | ≈ 20 000 000 | 1 000 000 000 000 |

À environ un milliard d'opérations simples par seconde (ordre de grandeur), trier un
million d'éléments en n log n prend ≈ 0,02 s. En n², cela prend ≈ 1 000 s, soit environ
17 minutes.

## Meilleur, moyen et pire cas

Un même algorithme peut se comporter très différemment selon les données. Le tri par
insertion fait n comparaisons sur un tableau déjà trié, mais n²/2 sur un tableau inversé.
Le tri rapide avec un mauvais pivot retombe en n². C'est pour cela que `make bench` teste
4 profils de données.

## La mémoire a aussi une complexité

Certains tris travaillent sur place, avec une mémoire en plus constante (tas, insertion).
D'autres ont besoin d'un tableau auxiliaire de taille n (fusion), ou d'une table de
comptage de taille k, où k est l'écart entre la plus petite et la plus grande valeur.

## La stabilité

Ce n'est pas de la complexité, mais on en parle avec. Un tri est stable si deux éléments
égaux gardent leur ordre de départ. Exemple : des élèves rangés par ordre alphabétique,
puis triés par note ; avec un tri stable, les élèves de même note restent dans l'ordre
alphabétique.

## La limite des tris par comparaison

Un tri qui ne fait que comparer des paires d'éléments ne peut pas faire mieux que de
l'ordre de n log n comparaisons dans le pire cas. Le comptage et le radix passent sous
cette limite parce qu'ils ne comparent pas. En échange, ils exigent des clés entières dans
une plage connue.

## Tableau récapitulatif

Le O est omis pour la lisibilité ; k = écart entre la plus petite et la plus grande valeur.

| Algorithme | Meilleur cas | Cas moyen | Pire cas | Mémoire en plus | Stable |
| --- | --- | --- | --- | --- | --- |
| Bulle (arrêt anticipé) | n | n² | n² | 1 | Oui |
| Sélection | n² | n² | n² | 1 | Non |
| Insertion | n | n² | n² | 1 | Oui |
| Cocktail | n | n² | n² | 1 | Oui |
| Peigne | n log n | variable, souvent proche de n log n | n² | 1 | Non |
| Shell (écarts de Ciura) | n log n | pas de formule connue | pas de formule connue | 1 | Non |
| Fusion | n log n | n log n | n log n | n | Oui |
| Rapide (médiane de trois) | n log n | n log n | n² | log n (pile) | Non |
| Arbre ABR | n log n | n log n | n² | n | Oui\* |
| Arbre AVL | n log n | n log n | n log n | n | Oui\* |
| Tas | n log n | n log n | n log n | 1 | Non |
| Comptage | n + k | n + k | n + k | n + k | Oui |
| Radix LSD (4 passes sur des `int` 32 bits) | n | n | n | n | Oui |
| Seaux (valeurs bien réparties) | n | n | n² | n | Oui |
| Introsort | n log n | n log n | n log n | log n | Non |
| Timsort | n | n log n | n log n | n | Oui |

\* À condition d'insérer les éléments égaux à droite.
