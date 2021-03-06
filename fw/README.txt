 USING THE BLUETOOTH CLOCK
===========================

1. Connecting for the first time
--------------------------------

   1. Press the button on the controller board for a second or more, then release it.
   2. The clock will choose a random Bluetooth PIN and display it for 30 seconds.
   3. Search for Bluetooth devices on your computer, pair with "BT Clock", use the PIN you saw.

   
2. Connecting to the clock for control
--------------------------------------

After pairing, the clock will offer a Bluetooth Serial Port. On my system, I actually saw two; if that happens, pick the first.
Connect to the serial port using 9600 8N1 (9600 Baud, 8 data bits, no parity and one stop bit).


3. What the clock can do
------------------------

  - Display the current time in 24-hour format, with blinking second dot
  - Display the current date in DD.MM. format
  - Display up to five arbitrary lines of text up to 43 characters each
     - If a line is longer than four characters, it will be scrolled across the display
     - Two scroll modes exist: Data Display mode and Marquee mode
     - Data Display mode will hold the beginning and end of the line for a short while; ideal for displaying data
     - Marquee mode will smoothly scroll the whole line through the display; ideal for messages or slogans
  - Configurable timed sequence of the above options
  - Configurable blank time during which the clock display will be turned off
  - Time is backed by a large capacitor and will continue running for about half a day if the power is interrupted
  - Configuration is stored in non-volatile memory and will survive power outages for as long as it takes 


4. How to set up the clock
--------------------------

The clock understands a few simple commands (as in "simple to parse by a microcontroller" ;).
Each command must be completed by a newline (LF, to be exact, CRs will be tolerated, but ignored).
The clock will respond "OK" if the command was correct, otherwise it will respond "No|" followed by the string it received.
You can use that to debug connection/terminal problems.

T=YYMMDDhhmmssWW
    Set the time, 24-hour format. Invalid values will be rejected.
    WW designates the day-of-week, with 00=Monday, 01=Tuesday, ..., 06=Sunday
    
T?
	Query the current time. The clock will respond the current time before "OK".

<N>=[!]<text>
	Set a text line, where <N> is a number between 1 and 5 and <text> is arbitrary text up to 45 characters.
	Valid characters are 1-9, a-z, A-Z and ., but case does not matter. It's hard enough to invent a font for 7-segment displays as it is ;)
	Also, there are some special characters:
	   '=' will yield a dash
	   '<' will yield a capital C (as opposed to the lower-case c you get with 'c' and 'C')
	   '@' will yield a '�' (degree) symbol -- so "@<" will yield "�C" in case you want to display a temperature ;)
	All other characters will be mapped to space.
	If you put a '!' before the line, it will be scrolled in Marquee mode, otherwise in Data Display mode.
	
<N>?
    Query the <N>-th line. Will respond with the N-th line and "OK". 

S=Xttt[,Xttt[,...]]
    Configure the display sequence. 
    The sequence defines what will be displayed on the clock, in which order, and for how long.
    The sequence can consist of up to ten elements, separated by commas. It repeats after the last element.
    Each element starts with one character defining what should be displayed (the X), followed (without space) by a (decimal) time in seconds.
    For X, you have a choice of:
      "T" - display the current time
      "D" - display the current date
      "1".."5" - display text line 1..5
    So, for example, "S=T30,D10,120" will display the time for 30s, then the date for 10s, then line 1 for 20s, then repeat from the start.
    Please note that there is no space in the command, especially not after the commas.  
    
S?
    Query the sequence. Will respond with the sequence and "OK".

B=HHMM-HHMM,X
	Set the blank time. Starting with the first time, and up to the second time, the clock will blank its display.
	Additionally, specify X as "1" if the clock shall run during the weekend, "2" if the clock shall turn off during the weekend.
	For example, "B=1800-0800,2" will keep the display off outside normal work hours.
	Likewise, "B=0800-1600,1" seems like a sensible setting for a hackerspace ;)

B?
	Query the blank time setting. Will respond with the blank time and "OK".

C=HHMM-HHMM,L
	Set line L to be displayed starting with the first time and up to the second time.
	During the programmed time, this line will override the normal display sequence, but not the blank times.
	For example, "C=1200-1230,4" would display line 4 between 12:00 and 12:30.

C?
	Query the special line setting.

D=HHMM-HHMM,L
D?
	This is a second special line setting, similar to C=...
	
0=[!]<text>
	"Line 0" is sent directly to the display and will be shown for 42 seconds, without being stored to EEPROM.
	Note there is no query command for this line. You can use this feature to display random, intermittent information
	without putting write stress on the EEPROM.


5. Example configuration:
-------------------------

T=130225193030
1= 132
4=!    scrum
B=1900-0800
C=1245-1250,1
S=T30,D5,115

This means, in order:
 * Set the current time
 * Set line one to display, say the current number of FIXMEs in our code ;)
   Note how the line starts with a space so the three-figure number is right-aligned.
 * Set line four to scroll "scrum" in Marquee Mode. Add some spaces so there is a bit of pause between the "scrum"s.
 * Set the clock to blank outside normal office hours
 * Announce Scrum each day from 1245 to 1250.
 * Set the normal sequence to display the time for 30s, the date for 5s and the number of FIXMEs for 15s.
 