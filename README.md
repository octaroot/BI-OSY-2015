# BI-OSY 2014/15
## Task #1 - Spekulace s pozemky
### Zadání
Úkolem je realizovat sadu funkcí, které budou umožňovat rychle naceňovat pozemky podle různých kritérií.

### Řešení
Realizace pomocí POSIXových vláken a synchronizačních primitiv (semafor, mutex)

```
cmake CMakeLists.txt
make
./ProgtestOSY1
```

### Podrobnější specifikace (zadání)
Dnešní burzovní operace jsou závislé na rychlém zpracování a rozhodování. Úkolem bude vytvořit takovou podporu pro spekulace s pozemky. Předpokládáme, že máme mapu pozemků k prodeji/koupi. Tato mapa má čtvercový tvar a je rozdělena na N x N čtvercových polí (parcel). Parcela je dále nedělitelná a má pro další úvahu jednotkovou velikost. Chceme vyhledávat obdélníkový výřez mapy (tedy X x Y parcel, X ≤ N, Y ≤ N) podle následujících kritérií:

  1. Nalezení co největší části mapy, kde celková cena parcel je nižší nebo rovná zadané mezi. Na vstupu je mapa NxN parcel, pro každou parcelu známe její cenu. Cena může být kladná i záporná (na parcele je závazek). Úkolem je najít obdélníkový výřez s co největší plochou, kde celková částka nepřekračuje zadanou mez. Takových výřezů může být i více, úkolem je najít alespoň jeden či nahlásit, že za danou cenu nejde nic koupit.
  2. Nalezení co největší části mapy, kde míra kriminality na žádné parcele nepřekračuje zadanou mez. Na vstupu je mapa NxN parcel, pro každou parcelu známe hodnotu zločinnosti (průměrný počet zločinů spáchaných v daném místě během nějakého sledovaného období, nezáporné desetinné číslo). Úkolem je najít obdélníkový výřez s co největší plochou, kde na žádné parcele výřezu nepřekročí míra kriminality zadanou mez. Takových výřezů může být opět více, úkolem je najít alespoň jeden či nahlásit, že za daných podmínek nelze nic koupit.

Vaším úkolem je realizovat software, který dokáže tyto problémy řešit. Protože se jedná o výpočetně náročné problémy, je potřeba výpočet rozdělit do vláken a tím výpočet urychlit.

