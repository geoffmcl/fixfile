#!/bin/sh

TMPEXE="fixfile"
if [ ! -x "$TMPEXE" ]; then
echo "Can NOT locate $TMPEXE! *** FIX ME ***"
exit 1
fi

TMPFIL1="temp1.txt"
TMPFIL2="temp1d.txt"
TMPFIL3="temp1u.txt"

if [ -f "$TMPFIL1" ]; then
 rm -v -f "$TMPFIL12"
fi

echo "Writing file $TMPFIL1 ..."
echo "Line 1" > $TMPFIL1
echo "Line 2" >> $TMPFIL1
echo "Line 3" >> $TMPFIL1

echo "Doing: './$TMPEXE $TMPFIL1 -v0 -n -o-$TMPFIL2' - ie Create DOS file, with CR/LF..."
./$TMPEXE $TMPFIL1 -n -o-$TMPFIL2

echo "Doing: './$TMPEXE $TMPFIL2 -v0 -u -n -o-$TMPFIL3' - ie Convert DOS back to Unix..."
./$TMPEXE $TMPFIL2 -u -n -o-$TMPFIL3

echo "DOING: 'diff -u $TMPFIL1 $TMPFIL3' - ie compare unix begin with unix created..."
diff -u $TMPFIL1 $TMPFIL3
if [ "$?" = "0" ]; then
    echo "No difference"
else
    echo "What is the difference!"
fi

# rem eof

