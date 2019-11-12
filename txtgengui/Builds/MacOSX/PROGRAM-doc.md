#  program mode

the program mode is using Lua as a scripting language.
please go here for a documentation of Lua:
[https://www.lua.org/docs.html](https://www.lua.org/docs.html)

there are some additional functions to display text and text to speech output.
you could also have a look at the examples of the program mode.

functions:

`screen(width, height)`
show the window & set the screen/window size

`show(text, seconds)` 
show a text for a certain time

`showAndSpeak(text, seconds)`
show a text for a certain time and speak it

`id = addText(text)`
add a text node. returns an id to do things with the text

`removeText(id)`
remove a certain text

`clear()`
clear all texts

`color(color)`
set the text color

`background(color)`
set the background color

`left()`
left adjust the following text

`right()`
right adjust the following text

`center()`
center text

`position(x, y)`
move next text to x, y. x and y are in the range of -1 to 1. 0 is in the center of the screen.

`randomPosition()`
move next text to a random position

`rotation(degree)`
rotate next text

`randomRotation()`
rotate next text randomly

`size(fontSize)`
set the size of the following text

`wait(x)`
wait for x seconds

`speak(text)`
speak text

`onKey(key, func)`
call the function when key is pressed. this only works as long as the program is running. you might want to use forever() at the end of an interactive program.

`waitForKey()`
waits until a key was pressed

`forever()`
waits forever or until user interrupt. useful to create interactive programs where the user can press keys.

`grammar(grammarString)`
generate from a grammar

`LSystem(rules, text)`
returns a modified text with the rules of the LSystem

