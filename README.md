# Arbeitstitel: Escape Tower
(SoSe2021_Padi22)

Bei Escape Tower handelt es sich um ein RPG / Dungeon Game. Grundlage soll eine zweidimensionale Datenstruktur bilden, welche RAM-sparsam sehr große Maps zulässt, in denen sich der Spieler frei bewegen kann.

Das muss natürlich ausgenutzt werden! Da ich jedoch nicht das halbe Semester damit zubringen möchte, Maps in Textfiles zu tippen und es sicherlich eine andere Baustelle wäre, einen Mapeditor zu entwickeln, reizt mich das [**S**]-Kriterium der **prozedural generierten Map**. Obwohl ich es durchaus als Herausforderung ansehe, mich in die Algorithmik der dynamischen Maperstellung einzuarbeiten, halte ich es (im Hinblick auf eine gute Note :wink:) für in der mir zur Verfügung stehenden Zeit schaffbar, ein *Dungeon-Game* zu bauen, welches seine Etagen beim Betreten frisch berechnet. Das ermöglicht natürlich **beliebig viele Level** und **diverse Schwierigkeitsgrade**.

Ziel des Spielers ist es, auf jeder Etage des Dungeons das Ziel (Treppe oder Ausgang) zu erreichen um auf die nächste Etage zu gelangen, bis er irgendwann alle Etagen absolviert und damit seine Freiheit erspielt hat.

Dabei stehen dem Spieler in dem Labyrinth aus Räumen einer Etage natürlich diverse Hindernisse im Weg. Vorstellbar wären:
- Gegner, die den Spieler töten (oder nur verletzen? :thinking:) können
- Verschlossene Türen, für die erst ein Schlüssel gefunden werden muss
- Darkrooms, für die ein Licht-Item hilfreich (aber nicht erforderlich) ist
- ... sowie andere Quests / Hindernisse

Ein Speichern und Laden des Spielstands soll möglich sein, aber evtl. nicht zu jedem Zeitpunkt. Vielleicht nur beim betreten einer Etage oder durch ausnutzen eines speziellen Speicher-Items.

## Bewertungskriterien
... aus der [PADI-Tabelle](https://graphics.tu-bs.de/teaching/ss21/padi)
### Pflichtkriterien
Diese Kriterien möchte ich auf jeden Fall umsetzen, damit nach o.g. Konzept ein gutes Game entsteht.
- prozedural generierte Map [S]
  - bzw. auch: Levelgeneration im Spiel [A] 
  - **Wo ist da der Unterschied?**
- Quest / Inventar - Management [A]
- 3+ (beliebig viele) Level [C]
- Schwierigkeitsgrade [C]
- Speichern/Laden [C]

### Wenn es die Zeit zulässt
Diese Kriterien wären allgemein wünschenswert und sind (vermutlich) nicht allzu schwer zu implementieren und werden daher umgesetzt, sollte es die Zeit zulassen.
- Einfache Animationen [B]
- Controllerunterstützung [B]
- Betriebssystemunabhängigkeit [B]

### Nice to have
Diese Kriterien würden noch gut ins Spielkonzept passen, werde ich aber vermutlich aufgrund des begrenzten zeitlichen Rahmens und der Maßgabe Qualität > Quantität leider nicht umsetzen können.
- Netzwerk Multiplayer [A]
- Ingame Voicechat (Netzwerk) [A]
- Levelgeneration aus einer Textdatei [B]