//
//  Examples.cpp
//  txtgen - App
//
//  Created by joerg piringer on 05.10.17.
//

#include "Examples.hpp"

std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> grammarExamples() {
    return {
        
{"tutorial 1",
LR"(# tutorial 1
START = begin; # a START symbol is always needed
# don't forget the semicolon ; at the end of each line
)"
,{}
},
{"tutorial 2",
LR"(# tutorial 2
START = SECOND; # a START symbol is always needed
SECOND = all words written only in capital letters (and numbers) are substitued by their definition;
)"
    ,{}
},
{"tutorial 3",
LR"(# tutorial 3
START = EITHER | OR; # symbols separated by | are chosen randomly, try it a couple of times
EITHER = this;
OR = that;
)"
    ,{}
},
{"tutorial 4",
LR"(# tutorial 4
START = EITHER OR; # symbols separated by a space are joined
EITHER = this;
OR = that;
)"
    ,{}
},
{"tutorial 5",
LR"(# tutorial 5
START = FIRST+SECOND; # symbols separated by + are joined without a space
FIRST = ver;
SECOND = bindung;
)"
    ,{}
},
{"tutorial 6",
LR"(# tutorial 6
START = FIRST+"SECOND"; # characters in quotes " are not seen as symbols
FIRST = ver;
SECOND = bindung;
)"
    ,{}
},
{"tutorial 7",
LR"(# tutorial 7
START = first line\nsecond line; # to separate lines use the sequence \n (btw: n stands for newline)
)"
    ,{}
},
{"tutorial 8",
LR"(# tutorial 8
START = A A A A A A;
A = but | though| and | too| B;
B = i | you | he | she | START; # you can use symbols in symbol definitions
)"
    ,{}
},
{"tutorial 9",
LR"(# tutorial 9
START = infinite START; # but don't do this!!!
)"
    ,{}
},
{"tutorial 10",
LR"(# tutorial 10
START = A A A A A A A A A;
A {permutate} = a | b | c | d; # with permutate all options are exhausted before they can appear again
)"
    ,{}
},
{"tutorial 11",
LR"(# tutorial 11
START = A A A A A A A A A;
A {order} = a | b | c | d; # with order all options are selected in the order they appear
)"
    ,{}
},
{"tutorial 12",
LR"(# tutorial 12: conditions
START = {X = 1} C; # change the value behind "X = " to 2, 3, ...
C {X == 1} = eins;
C {X == 2} = zwei;
C {X >= 2} = viel;
)"
    ,{}
},
{"tutorial 13",
LR"(# tutorial 13: counter
START = {COUNTER = 0} C;
C {COUNTER <= 99} = {COUNTER++} COUNTER C;
C {COUNTER > 99} = ende;
)"
    ,{}
},
{"tutorial 14",
LR"(# tutorial 14: gerade ungerade
START = {COUNTER = 0} C;
C {COUNTER <= 99} = {COUNTER++} COUNTER C;
C {COUNTER > 99} = ende;
)"
    ,{}
},
{"tempora",
LR"(START = SATZ+.;

SATZ {permutate}    = TEMPPRAES SATZPRAES | TEMPPAST SATZPAST | TEMPFUT SATZFUT;

SATZPRAES {permutate} = VERBPRAESSING SATZCONTSING | VERBPRAESPLUR SATZCONTPLUR;
SATZPAST {permutate} = VERBPASTSING SATZCONTSING | VERBPASTPLUR SATZCONTPLUR;
SATZFUT {permutate} = VERBFUTSING1 SATZCONTSING VERBFUTSING2 | VERBFUTPLUR1 SATZCONTPLUR VERBFUTPLUR2;

VERBPRAESSING = ist;
VERBPRAESPLUR = sind;

VERBPASTSING = war;
VERBPASTPLUR = waren;

VERBFUTSING1 = wird;
VERBFUTSING2 = gewesen sein;
VERBFUTPLUR1 = werden;
VERBFUTPLUR2 = gewesen sein;

SATZCONTSING {permutate} = alles besser | nichts wie zuvor | alles vergeblich | alles einerlei | alles schlechter | uns alles einerlei;
SATZCONTPLUR {permutate} = wir glücklich | wir unerfahren | wir unvorsichtig | wir übervorsichtig | wir genau | wir entschlossen zu allem;

TEMPPRAES {permutate} = heute | momentan | jetzt | in diesem moment;
TEMPPAST {permutate} = vorgestern | gestern | vor einem jahr | vor einer woche | gerade eben | voriges monat;
TEMPFUT {permutate} = morgen | übermorgen | bald | gleich | in einer woche | in einem jahr | in der zukunft | in einem monat;
)"
    ,{}
},
{"wittgenstein",
LR"(START = {COUNTER=1} PARAGRAPH PARAGRAPH PARAGRAPH PARAGRAPHS PARAGRAPHS;

PARAGRAPHS = PARAGRAPH PARAGRAPHS | PARAGRAPH;
PARAGRAPH = COUNTER+. START1 START1 SENTENCES {COUNTER++}\n\n;
SENTENCES = START1 SENTENCES | START1;

START1 = START1A+. | START1A+. | START1A+. | START1A+. | PREFIX START1A+. | PREFIXQ START1A+? | FRAGE;
START1A = SATZ1 | SATZ2 | SATZ3 | SATZ4;

PREFIX    = ich kann mich darin nicht irren: | ich könnte sagen: | wenn ich sagte: | das heisst: | die antwort kann auch sein: | es ist seltsam: | man sagt nicht: | und dennoch: | wir sagen: | ich könnte auch so sagen: | man könnte auch sagen: | es scheint: | das wichtigste ist hier dies:;
PREFIXQ    = soll ich sagen: | heißt das nun: | ist es falsch, zu sagen:;

SATZ1 = SPTRANS OBJEKT;
SATZ2 = SPINTRANS;
SATZ1O2 = SATZ1 | SATZ2;
SATZ3 = SATZ1O2+, CONJ CONDSATZ;
SATZ4 = wenn CONDSATZ+, dann CONDSATZ2;


CONDSATZ = CONDSATZTRANS | CONDSATZINTRANS;
CONDSATZTRANS = SUBJEKTSING OBJEKTSING PREDTRANSSING | SUBJEKTPLUR OBJEKTPLUR PREDTRANSPLUR;
CONDSATZINTRANS    = SUBJEKTSING PREDINTRANSSING | SUBJEKTPLUR PREDINTRANSPLUR;

CONDSATZ2 = CONDSATZTRANS2 | CONDSATZINTRANS2;
CONDSATZTRANS2 = PREDTRANSSING SUBJEKTSING OBJEKTSING | PREDTRANSPLUR SUBJEKTPLUR OBJEKTPLUR;
CONDSATZINTRANS2 = PREDINTRANSSING SUBJEKTSING | PREDINTRANSPLUR SUBJEKTPLUR;

FRAGE = FRAGEPREFIX FRAGE1A | FRAGE1A | FRAGE1A;
FRAGEPREFIX = man möchte fragen: | frage:;
FRAGE1A = FRAGEWORT PREDINTRANSSING SUBJEKTSING+? | FRAGEWORT PREDINTRANSPLUR SUBJEKTPLUR+?;
FRAGEWORT = was | wie | wo;

SPTRANS = SUBJEKTSING PREDTRANSSING | SUBJEKTPLUR PREDTRANSPLUR;
SPINTRANS = SUBJEKTSING PREDINTRANSSING | SUBJEKTPLUR PREDINTRANSPLUR;

OBJEKT = OBJEKTSING | OBJEKTPLUR;

ARTIKELSINGW = die;
ARTIKELSINGM = der;
ARTIKELSINGN = das;

SUBJEKTSING1W = wahrheit | vorstellung | antwort | frage | negation | sprachverwendung | absicht | erwartung | philosophie | frage;
SUBJEKTSING1M = satz | verstand | sinn | mensch;
SUBJEKTSING1N = wort | erkennen | sprachspiel | wissen | experiment;

GENITIVSING1M = satzes | verstandes | sinns | menschens;
GENITIVSING1N = wortes | erkennens | sprachspiels | wissens | kindes;

GENITIVSING    = der SUBJEKTSING1W | des GENITIVSING1M | des GENITIVSING1N;

ADJSINGDET = unzweifelhafte | unbeabsichtigte | zweifelhafte | überprüfte | unfehlbare | zusammenhanglose;

SUBJEKTSING1A = ARTIKELSINGN SUBJEKTSING1N | ARTIKELSINGM SUBJEKTSING1M | ARTIKELSINGW SUBJEKTSING1W;
SUBJEKTSING1ADJ    = ARTIKELSINGN ADJSINGDET SUBJEKTSING1N | ARTIKELSINGM ADJSINGDET SUBJEKTSING1M | ARTIKELSINGW ADJSINGDET SUBJEKTSING1W;
SUBJEKTSING = SUBJEKTSING1A | SUBJEKTSING1ADJ | SUBJEKTSING1A GENITIVSING | SUBJEKTSING1ADJ GENITIVSING;
PREDTRANSSING = sieht | untersucht | teilt | liest;
PREDINTRANSSING    = spricht | lernt | irrt;
OBJEKTSING = den satz | mich | den sinn | den zweifel | das spiel | das gedächtnis;

SUBJEKTPLUR1 = gedanken | farben | sätze | antworten | fragen | worte | dinge | fragen;
SUBJEKTPLUR    = SUBJEKTPLUR1 | ADJPLUR SUBJEKTPLUR1;
PREDTRANSPLUR = sprechen | lehren | lesen;
PREDINTRANSPLUR    = sprechen | spielen | beginnen | lernen;
OBJEKTPLUR = die sätze | die zweifel | die wörter;

ADJPLUR = eindeutige | einzigartige | verschiedene | unzweifelhafte | unmittelbare;

CONJ = während | wo | wenn | weshalb | worauf;
)"
    ,{}
},
{"grammarrhythm",
LR"(START = S+.\n+S+.\n+S+.\n+S+.;
S = tri XI | ti XF | dha XH;
XI = ikt dha tri ikt dha ge na XD | ikt XJ | ikt XG;
XF = dha XJ | XG;
XH = ti dha tri ikt XB | tri ikt dha XC;
XD = dha ti dha ge dhee na ge na;
XJ = tri ikt XA;
XG = dha ti XA;
XB = dha tri ikt XD;
XC = ti XB | ti dha ti XD | tri ikt dha XE;
XA = dha XB;
XE = dha XD | ge XD;
)"
    ,{}
}
};
}

std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> LSystemExamples() {
    return {
        {"alge",
            L"# algenwachstum\na = ab;\nb = a;", {}},
        
{"b-sprache",
LR"(# b-sprache
a = aba;
e = ebe;
i = ibi;
o = obo;
u = ubu;
y = yby;
ä = äbä;
ö = öbö;
ü = übü;
    )"
    ,{}
},
        {"buchstabieralphabet",
LR"(# österreichisches buchstabieralphabet
a = anton ;
b = berta ;
c = cäsar ;
d = dora ;
e = emil ;
f = friedrich ;
g = gustav ;
h = heinrich ;
i = ida ;
j = julius ;
k = konrad ;
l = ludwig ;
m = martha ;
n = nordpol ;
o = otto ;
p = paula ;
q = quelle ;
r = richard ;
s = samuel ;
t = theodor ;
u = ulrich ;
v = viktor ;
w = wilhelm ;
x = xaver ;
y = ypsilon ;
z = zeppelin ;
ä = ärger ;
ö = ökonom ;
ü = übermut ;
ß = scharfes s ;)"
            ,{}
},
{"rhythm 1",
    LR"(# rhythm 1 - start with s
s = ska;
t = atts;
k = kakap;
p = kpki kik po po;
o = op rop po;
)"
    ,{}
},
{"rhythm 2",
LR"(# rhythm 2 - start with l
l = lü;
ü = ülüm;
m = ilimmi;
i = linnih;
h = heumü;
)"
    ,{}
},
{"rhythm 3",
LR"(# rhythm 3 - start with i
i = ix;
x = xixa;
a = aksi;
k = kassik;
)"
    ,{}
},
{"rhythm 4",
LR"(# rhythm 4 - start with a
a = aaif;
i = iof;
o = oaouf;
u = ufuue;
e = effei;
)"
    ,{}
},
{"snowflake",
LR"(# snowflake
# axiom: X--X--X
# set angle to 60
# press Show
X = X+X--X+X;
)"
,{L"axiom:X--X--X", L"angle:60"}
},
{"snowflake 2",
LR"(# snowflake 2
# axiom: XXXX--XXXX--XXXX
# angle: 60
# press Show
XXXX = XXXX+XXXX--XXXX+XXXX;
)"
,{L"axiom:XXXX--XXXX--XXXX", L"angle:60"}
},
{"dragon-curve",
LR"(# dragon-curve
# axiom: RRR
# angle: 45
# press Show
RRR = +RRR--LLL+;
LLL = -RRR++LLL-;
)"
,{L"axiom:RRR", L"angle:45"}
},
{"triangle",
LR"(# triangle
# axiom: A
# angle: 60
# press Show
A = B-A-B;
B = A+B+A;
)"
,{L"axiom:A", L"angle:60"}
},
{"hilbert curve",
LR"(# hilbert
# axiom: X
# angle: 90
# press Show
X = -YF+XFX+FY-;
Y = +XF-YFY-FX+;
)"
,{L"axiom:X", L"angle:90"}
},
{"tree",
LR"(#tree
# axiom: FFFFX
# angle: 30
# press Show
X = <[+FFFFX][-FFFFX];
)"
,{L"axiom:FFFFX", L"angle:30"}
},
{"plant 1",
LR"(# plant 1
# axiom: F
# angle: 25
# press Show
F = F[+F]F[-F]F;
)"
    ,{L"axiom:F", L"angle:25"}
},
{"plant 2",
LR"(# plant 2
# axiom: F
# angle: 22
# press Show
F = FF-[-F+F+F]+[+F-F-F];
)"
,{L"axiom:F", L"angle:22"}
}
};
}

std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> markovExamples() {
    return {
        {"abendlied",
LR"(der mond ist aufgegangen,
die goldnen sternlein prangen
am himmel hell und klar;
der wald steht schwarz und schweiget,
und aus den wiesen steiget
der weiße nebel wunderbar.

wie ist die welt so stille,
und in der dämmrung hülle
so traulich und so hold!
als eine stille kammer,
wo ihr des tages jammer
verschlafen und vergessen sollt.

seht ihr den mond dort stehen?
er ist nur halb zu sehen,
und ist doch rund und schön!
so sind wohl manche sachen,
die wir getrost belachen,
weil unsre augen sie nicht sehn.

wir stolze menschenkinder
sind eitel arme sünder
und wissen gar nicht viel;
wir spinnen luftgespinste
und suchen viele künste
und kommen weiter von dem ziel.

gott, laß uns dein heil schauen,
auf nichts vergänglichs trauen,
nicht eitelkeit uns freun!
laß uns einfältig werden
und vor dir hier auf erden
wie kinder fromm und fröhlich sein!

wollst endlich sonder grämen
aus dieser welt uns nehmen
durch einen sanften tod!
und, wenn du uns genommen,
laß uns in himmel kommen,
du unser herr und unser gott!

so legt euch denn, ihr brüder,
in gottes namen nieder;
kalt ist der abendhauch.
verschon uns, gott! mit strafen,
und laß uns ruhig schlafen!
und unsern kranken nachbar auch!)"
            ,{}
},
{"woerter",
LR"(abgekratzter
abkoche
abgefülltem
abgaskanals
admiralsgattin
abbildungsabschnitts
abstimmfrequenzen
abreist
abstrahlend
abzuhörende
achtflächner
absprunggeschwindigkeiten
abgenabelten
abwasseraufbereitung
abfertigungsleitern
abkürzen
abfindungsanspruches
ablageschränke
ablaufgerüsten
abzäunend
abmahnten
abtrat
abrechnungsroutinen
abenteuerpfades
abzusehende
achtzehnter
abenteuerurlauben
abfallhalde
abzupellen
abgebuchte
abzulaichen
abschussvariante
abbekommens
abgekratzten
admiralstabschefs
ablötender
abschnittszahl
achtzehnstöckigem
abgrund
abputzenden
abkürzungserläuterung
abschlussfigur
afghanen
abklaubtest
absolutzahlen
abrollgewichtes
achtsilbe
abschreibungsliste
absatztheorie
abwehrreihen
abtrennung
ackerkommissionen
abstoppst
abmontierendes
ackerbausysteme
absicherungsmitteln
abfahrtsrennen
achtsitzig
abrunden
abwurftests
abstiegskonkurrenten
adelsporträts
abhöre
absatzkampf
aberwitziger
absturzstelle
abstößen
abschlusstrakten
ackerlosem
abzahlungsdarlehen
abgestaubten
abflauend
abfallwirtschaftsverbandes
absatztrennung
abflussrohrs
abzuklopfenden
abenteuerurlauberin
abpasst
abkratzen
abzeichnenden
affenstunde
abfertigungsposition
abkürzungswörterbüchern
abrüstungsangeboten
abschreibungsrücklage
abwicklungsbank
adressabgleich
aberwitzigen
afghane
abgetretenen
abmildre
affenstalles
abgekämpft
ableitungsterms
abgefeuertem
addierbefehle
abwiegelnder
abteilungsgefügen
additiven
abgeschmirgelte
abspülen
abziehwerkzeugs
abschiedsfete
abwehrkämpfen
abspieldrehzahl
ableseverfahren
abtransportierbarer
adoptiertest
abgekoppelt
abzugsfähigkeiten
abflussmittels
abstellkapazität
abstrahlrichtung
administrationstool
abschaltspannung
abgrenzungsstrukturen
abbruchfalls
abseitsregeln
abdominallinie
ableitungssystemen
abnutzungsgefechte
abarbeitendes
absicherungsformen
abgasbeheizt
adressermittlung
abdampfflut
abtasteinheiten
abzuwirtschaftende
abbruchmaske
ablassventil
abdruckes
abriebvorgang
abflugplan
adjunktor
adressensuchdienstes
abschalttermins
ablaufprozesse
abgezockten
abaxialer
abzweigdose
abstandsquadratgesetze
abnutzungspigment
abtastsignals
abbaugeräuschen
abgabenbelastungen
abspülst
achtarmigen
abrissgeschwindigkeiten
abfertigungsvorschrift
abendländisch
abendnetz
abbildungsnotationen
abzulegen
adlerfanges
abbaubeschleunigung
abschrägt
abfräst
abwaschendem
ablaufvorgang
achselstück
abspielmethoden
abstoppte
abzuhören
adressendienst
ablieferten
abgestützter
abfärbendem
abwehrfähigkeit
abgehängtes
abwehrfahrzeugs
adapterringen
abzuleitendem
abziehbarer
acrylkronen
abgeflauter
absahnpolitik
abgeschlagenem
abfüllbetrieben
aberration
ablenkbar
abgeschattete
abendgage
abhörqualität
abbaumaschine
administrationssystems
ackerbaubetrieb
abgefressener
abgeschlepptes
abgezählten
abgespannteres
abschalttemperatur
abgebaggert
ablaufverfolgungsprogrammen
abwärtskompatiblem
abrichtwerkzeuge
abgemurkst
affensteinen
abreißkante
ablenkgeschwindigkeit
achterpacks
abzugskapitals
ablieferungsauflage
abtreibungslager
abwesenheitsprozessen
abrechnungsverbindlichkeit
abflussgewässern
afghanisch
abgrenzungsvereinbarung
abzahlten
abfallschublade
abrissen
abfertigungsleiter
abschert
abgehäutete
abschreckungskonzepten
achsträger
abendhauch
abseitstors
achtsaitiges
achtstelliges
absatzanteils
abfangjagd
ableitungsbegriffs
adäquatem
abschirmleiter
abbruchkrans
abhängigkeitspotenzial
achsstellung
abspielprogramms
absicherungsmaßnahme
abgeordnetenhauswahlen
abknöpftest
abzufindende
abbauleitzentrale
absorptionsminimums
actionelemente
abmarschierens
abstinenzgebote
abgeordnetenliste
abspanntextes
abfallbeseitigungsgesetz
abgebrachte
abgewürgtes
abnahmezeugnissen
additionsworten
abonnierbar
abgedrehtem
absetzraum
achsellymphgefäße
abwasserkonzepts
achtsemestrige
abwehrmechanismus
abgesägten
abzutreibendes
abrichtanschlages
abstimmungsbereich
abkürzungsteile
abstiegsreife
achsdifferentialen
achsenbau
ackermaß
absatzgliederungen
abblendbarer
abstellhebeln
abwaschwassers
abblockendem
abtratest
abgabenstrafverfahren
achsenwinkel
abschlusszugabe
achsausrichtung
aberntend
abschaltpunktes
achterbahnzug
abrollbergs
abgabefreier
abspiegelnde
abdruckspur
abonnementvertrages
abstimmungshinweisen
abschreibungsparameters
achtens
abschiedsbrief
adlerhaupt
adressensysteme
abzudeckende
abgequälten
abgerupfte
absperrorganen
abwurfdächern
abnahmesysteme
abfüllfilialen
abriegelungen
abrüstungskomitees
abstraktionsprinzipien
abschlusssatz
abzuzappeln
abzuschneidender
aderisolierung
ablösend
abbaurate
abzuwehren
abtsstühle
abfallprodukts
abwickelndes
abnormale
adriger
abschlussreferate
abwärtstransporten
abflussreinigern
abwehrprojekten
abgeguckt
abzugssystems
abspenstige
abschlussnomenklaturen
abstiegssorgen
achtelfinalspiele
absorptionskühlschrank
absehens
abgesangs
achtundvierzig
abbruchsiege
abzahlungssysteme
abbröckelnden
acrylharzes
abstrahierend
abstammungsnachweises
abzuklauben
abgeglittenem
abgeschmetterter
abgenützter
abschleppvorgang
ablaufleistungen
abriebvorganges
abteilbarer
achterbereich
achteten
abgezirkelt
abhängigkeitserfahrung
abdrosselndem
adlerflugs
achtundzwanzigfacher
abwanderungsbereitschaft
abenteuersucht
abdominalorgans
abwirtschaften
absteppender
adressenfreigaben
abwandert
ablasshilfen
abnormalere
abwesenheitsprozesse
achtbaren
abenteuerformates
abzusonderndes
abdankende
abnahm
abwicklungssysteme
abschlussquote
abstandszahlungen
abarbeitungsgeschwindigkeiten
affären
affenkultur
affenstil
administrationsbereich
abgeschlachteten
abschließenden
abwertungslandes
ablaufrechners
abbindest
adressendienste
ablasspraxis
abrate
abwehrgeschützen
abenteuerreichem
abschreibungsstichtag
abstimmkreises
abkürzungswegen
abstimmungsverluste
affektbrücken
ableitungsmaschinen
abnutzungsanzeigers
abänderungsbedürftiges
abstrampelns
ablötetet
achterschläge
abgeheilter
abbildungslösung
abzäuntet
abtakelnder
absetzapparat
abschlachtenden
absolviert
abfragegeräten
abgewetzten
ablagepunkte
achterbecke
abkratzen
abgangsballistik
adventswassers
achteckseiten
abgabezeitpunkte
adressleisten
abzublockendes
abschreckungsfunktion
abgespritztem
abbauregion
abdeckdrucke
abfallproblematik
abbrandoberflächen
affekterzeugung
abschicktet
abgekratztem
abdichten
abfülltechnologien
abschmiertet
abblendscheinwerfern
abfahrtsweltmeistertitel
abkoppelnder
abgezäuntes
abgenommen
achsenwechsel
abbaurevieren
adelstraktate
abdrosseltet
abtastmechanismen
abkürzungsorgien
aceton
adressierend
abschlussposition
absetzbecken
abergläubischster
ablaufsteuerwerke
abbruchfalls
äffe
adoptionen
adaptivem
abtropfenden
abreist
adelst
ackern
abbaugeräten
abrissreifes
abbuchungssystems
adelsfraktionen
adelstage
adelsrechts
abwegigeres
abhobelnde
abwasserkreislauf
abkuppelten
abschrauben
adressspeichers
abfaulender
actionregisseurin
abtropfen
abstreichend
abhinge
abzugleichender
abschlecke
abfangbewaffnung
abseitsstehen
adressseite
abbruchreaktion
abzutastender
abgeheiltes
abwälzung
adressblöcken
abgeordnetenhauswahlkreisen
abwurflinie
abdampfte
adlerschnäbeln
administrationsministern
ackerrechte
adelsgeschichte
abstiegsbedrohter
addiervorrichtungen
abwehrfähigkeit
abzuspülen
abschleifenden
abarbeitende
abschwellend
abneigungen
absurderen
abziehbare
abspanntest
abquältet
abriebfestem
abzuringende
abschneidefunktion
abstaubenden
abstrusitäten
abflog
adressmarkierungen
abgeschätztem
abbrechers
abgebüßtes
abhörschnittstelle
abschlussveranstaltungen
abgeordnetenwahl
abtastpositionen
abfertigungsgruppen
abklingkurven
abtrennenden
abzustecken
abschneidens
abfahrtstagen
abziehplakat
ableitungsversuch
abflussquerschnitten
abbindeverhaltens
abschlussventilventil
abschaltgeschwindigkeit
adventistischem
absorptionssystems
abfangjäger
abtippt
abgebrachten
acrylbeschichtung
achtermann
abgemindertem
abstimmungswillen
abwärtsfahrt
achtzigster
abgenäht
abfülle
abgasentgiftungen
abliefern
abgeplagter
abzuwerten
adaption
adventivwurzeln
abzweiggleis
abflussgittern
achtsamkeit
abbaubereiche
abstandsrohre
abschwenktet
affektwirkung
absatzprozesses
abbrennen
abzweigklemme
abgestattetem
ablehnungsbescheiden
abflaute
abbrausend
abdeckhüllen
abklingcharakteristik
abwasserreinigung
abschiedsworts)"
    ,{}
       },
        {"genesis",
            L"Im Anfang war das Wort, und das Wort war bei Gott, und das Wort war Gott. Im Anfang war es bei Gott. Alles ist durch das Wort geworden und ohne das Wort wurde nichts, was geworden ist. In ihm war das Leben und das Leben war das Licht der Menschen. Und das Licht leuchtet in der Finsternis und die Finsternis hat es nicht erfasst. Es trat ein Mensch auf, der von Gott gesandt war; sein Name war Johannes. Er kam als Zeuge, um Zeugnis abzulegen für das Licht, damit alle durch ihn zum Glauben kommen. Er war nicht selbst das Licht, er sollte nur Zeugnis ablegen für das Licht. Das wahre Licht, das jeden Menschen erleuchtet, kam in die Welt. Er war in der Welt und die Welt ist durch ihn geworden, aber die Welt erkannte ihn nicht. Er kam in sein Eigentum, aber die Seinen nahmen ihn nicht auf. Allen aber, die ihn aufnahmen, gab er Macht, Kinder Gottes zu werden, allen, die an seinen Namen glauben, die nicht aus dem Blut, nicht aus dem Willen des Fleisches, nicht aus dem Willen des Mannes, sondern aus Gott geboren sind. Und das Wort ist Fleisch geworden und hat unter uns gewohnt und wir haben seine Herrlichkeit gesehen, die Herrlichkeit des einzigen Sohnes vom Vater, voll Gnade und Wahrheit. Johannes legte Zeugnis für ihn ab und rief: Dieser war es, über den ich gesagt habe: Er, der nach mir kommt, ist mir voraus, weil er vor mir war. Aus seiner Fülle haben wir alle empfangen, Gnade über Gnade. Denn das Gesetz wurde durch Mose gegeben, die Gnade und die Wahrheit kamen durch Jesus Christus. Niemand hat Gott je gesehen. Der Einzige, der Gott ist und am Herzen des Vaters ruht, er hat Kunde gebracht."
            ,{}
        }
        };
}

std::vector<std::tuple<std::string,std::wstring, std::vector<std::wstring>>> programExamples() {
    return {
        {"hello, world!",
        LR"(print "hello, world!")"
        ,{}
        }
  };
}
