#  TODO

program API help

stimm- oder sprachauswahl für TTS

wenn beim export kein show-fenster offen ist, wird nichts exportiert (win10)

progress bar oder ähnliches bei langem rendern (export, show, l-system)

werte für winkel etc. mit up, down-ticker verstellbar machen

deviation von lsystemen deterministisch machen. groß ziehen des fensters sollte nicht die form verändern

automatischer updatehinweis
https://sparkle-project.org/documentation/
https://winsparkle.org


# LATER

title for examples utf8 compatible on windows (use std::wstring)

## show / animate
* skripte
* wie bei namshub osx

## interaktiv
* erweiterung von show?
* oder lua skripts?
* oder eine kombi aus beiden?

methods:
* suggest (siehe nam shub osx)

grammar: baum visualisieren

reenable mode "program"

word level markov


more Grammar features:
* string operationen
* MSG = MSG + test
* parameter für rules
-- modifier:    [X.inQuotes] [X.capitalizeAll]
-- choice function: RULE = {choice = __pos("POSRULE", [scale]), "a", "b", "c", "d", "e"}

sprache & stimme verstellbar machen


# DONE

file save

speak

syntax highlighting

- lsystem (mit graphics? oder erst später für die graphics einheit?)

- markov

- program

speak stop

parameter für markov einstellbar:
- prefix len
- generation length
- seed? random-seed?

remove unneccessary parameters when changing modes


more generation methods:
- namshub mini

modify selection


umlaute werden rot angezeichnet im tokeniser:
lsystem
grammar

lsystem tokenizer
lua tokeniser

undo

alle scopedpointer und ownedarrays etc. wegbekommen

* stretch
* part

more nam shub functions:
* condense

large editor font einstellbar machen

* permutate
* statevariablen

* arithmetische operationen, random
* COUNTER++

* rule conditions
* RULE{COUNTER == 3} = ...

disable mode "program" for episode 1

more examples

unsaved warnings bei quit
und bei example change (run type change)

icon

* österreichisches buchstabieralphabet (polizeifunk)

infinite recurson detection

windows version

* tempora

lsystem:
* rhythmus generator
* tutorial: permutate, order, variables, conditions, arithmetik

* wittgenstein

grammar:
* rhythmus generator


more examples
markov:
* konjunktionen

about

markov prefix len 0: so you can shuffle with a given output text length

sonderzeichen ersetzen. z.b. whitespace, newline mittels escape

* text turtle graphics
* + - drehen die turtle, buchstaben geben die buchstaben an,

* | 180 grad drehung

* zoom

* > < schriftgrössenänderung?

* save to image

L-system:
längere heads

* [] pushen und poppen den stack

* mehr beispiele


## visualisierung
* random angle deviation

change app name for win10 (on output in the project)

background funktioniert nicht bei program

crasht bei exit (font leak)

window grösse über program definieren

error und output von program reaktivieren

crasht wenn man das show fenster für program schliesst und wieder öffnet

doku für die program-befehle

video export

versionsanzeige

* animation des textes

progress bar beim export

keine eingabe von enter im namshub modus möglich

wenn export von program-mode gestartet wurde ohne vorherigem "run" dann crashts

mehr beispiele für program

videos mit dimension kleiner als 1920x1080 exportieren verzerrt

alle umlaute in examples.cpp in u-codes übersetzen, damit das file ohne BOM funktioniert

examples für win10 in das exe

win10 version

convert grammar to program

program: key input

waitForKey doesn't work

enter, space, esc, delete key abfragbar machen mit "enter", "space", ...
