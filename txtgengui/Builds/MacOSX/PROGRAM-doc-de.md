#  programm-modus

der programm-modus verwendet Lua als skript-sprache.
die dokumentation von Lua finden Sie hier:
[https://www.lua.org/docs.html](https://www.lua.org/docs.html)

zusätzlich gibt es einige funktionen für text- und sprachausgabe.
die beispiele des programm-modus helfen auch beim verständnis dieser funktionen. 

funktionen:

`screen(width, height)`
zeigt das ausgabefenster und stellt die grösse desselben ein.

`show(text, seconds)`
zeigt text für eine bestimmte zeit an.

`showAndSpeak(text, seconds)`
zeigt text für eine bestimmte zeit an und spricht ihn.

`id = addText(text)`
fügt einen text hinzu. liefert eine id zurück, die für die manipulation des textes verwendet werden kann.

`removeText(id)`
entfernt einen bestimmten text.

`clear()`
löscht alle texte.

`color(color)`
setzt die textfarbe der nachfolgenden texte.

`background(color)`
setzt die hintergrundfarbe.

`left()`
zeigt den nächsten text rechtsbündig an.

`right()`
zeigt den nächsten text linksbündig an.

`center()`
zeigt den nächsten text zentriert an.

`position(x, y)`
zeigt den nächsten text an der position x,y an. x und y sind im bereich von -1 bis 1. 0 ist im zentrum des fensters.

`randomPosition()`
zeigt den nächsten text an einer zufallsposition an.

`rotation(degree)`
dreht den nächsten text um degree grad.

`randomRotation()`
dreht den nächsten text um einen zufälligen winkel.

`size(fontSize)`
setzt die grösse des nächsten texts.

`wait(x)`
wartet für x sekunden.

`speak(text)`
spricht den text.

`onKey(key, func)`
ruft die funktion func auf, wenn eine bestimmte taste gedrückt wurde. das funktioniert nur solange das programm läuft. es kann sinnvoll sein, die funktion forever() einzusetzen um auf tastendrücke zu warten.

`waitForKey()`
wartet bis eine taste gedrückt wurde.

`forever()`
wartet ewig. oder bis das programm unterbrochen wird. kann für interaktive programme hilfreich sein, die auf tastendrücke warten.

`grammar(grammarString)`
generiere text aus einer grammatik.

`t = LSystem(rules, text)`
liefert einen text, der durch die regeln eines LSystems modifiziert wurde zurück.

