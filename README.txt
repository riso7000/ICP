====================================================
|   ICP projekt: Nástroj na tvorbu Petriho sietí   |
====================================================

Autori:
Richard Henček  (xhencer00)
Šimon Varga     (xvargas00)


Prehľad
-------

Program umožňuje interaktívne navrhovať a upravovať Petriho siete. Podporuje ich exportovanie do súboru vo formáte JSON. Taktiež umožňuje sieť spustiť a interagovať so spustenou sieťou. Počas behu siete program zapisuje log v hlavnom okne.


Implementovaná funkcionalita
----------------------------

Návrh siete:
* Vytváranie prvkov siete kliknutím na miesto v scéne pomocou odpovedajúceho nástroja z panelu nástrojov
* Upravovanie vlastnosti prvkov dvojitým kliknutím na prvok s nástrojom Select
* Mazanie prvkov zo scény
* Definovanie vstupov, výstupov a premenných v bočnom paneli
* Podporované dátové typy premenných sú int, float a char
* Ukladanie a načítanie siete z/do formátu JSON.

Simulácia siete:
* Sieť možno spustiť stlačením tlačítka Run a zastaviť tlačítkom Stop
* Možnosť sieti zasielať hodnoty na vstupy počas behu
* Aktualizácie výstupov a premenných je možné sledovať v bočnom paneli
* Jednotlivé udalosti počas behu siete sú zapisované do logu
* Podpora všetkých vstavaných funkcii inskripčného jazyka vyžadovaných základným zadaním
* Interpretácia inskripčného jazyka pomocou knižnice CFlat

Výber implementácie vo funckiách:
* defined("in_name") - vracia pravdivostnú hodnotu, či bol vstup nastavený od posledného spracovania
* elapsed("place") - bola implementovaná ako čas od poslednej zmeny počtu tokenov


Neimplementovaná funkcionalita a odklony od zadania
---------------------------------------------------

Interpretácia siete bola na začiatku vývoja koncipovaná úzkym prepojením s grafickou reprezentáciou, čo postupne viedlo k odklonu od zadaním vyžadovaného generovania interpretu siete a v konečnom dôsledku je interpretácia siete uskutočňovaná hlavným programom.


Knižnice tretích strán
----------------------

V projekte bola použitá knižnica od tretích strán CFlat, ktorá je uložená v adresári ./third_party/Cflat/* a je verejne dostupná na:
https://github.com/arturocepeda/Cflat/tree/master
