# Build Variables
SATDIR = sattrack
INCDIR = include
BUILDDIR = temp
C_OPTS = -O -DIOCTL -c -I$(INCDIR)
C_LOPTS = -lm
C_XOPTS = -I/usr/include/X11

# Link SatTrack
SATTRACK_OBJECTS = $(BUILDDIR)/satargs.o $(BUILDDIR)/satcalc.o $(BUILDDIR)/satchar.o $(BUILDDIR)/satcity.o $(BUILDDIR)/satctrl.o $(BUILDDIR)/satdisp.o $(BUILDDIR)/satelem.o $(BUILDDIR)/satgraph.o $(BUILDDIR)/satheli.o $(BUILDDIR)/satlice.o $(BUILDDIR)/satmath.o $(BUILDDIR)/satnute.o $(BUILDDIR)/satprec.o $(BUILDDIR)/satprop.o $(BUILDDIR)/satread.o $(BUILDDIR)/satrefr.o $(BUILDDIR)/sattele.o $(BUILDDIR)/sattest.o $(BUILDDIR)/sattime.o $(BUILDDIR)/sattrack.o $(BUILDDIR)/satvect.o
sattrack: $(SATTRACK_OBJECTS) $(INCDIR)/sattrack.h $(INCDIR)/satglobals.h $(INCDIR)/satglobalsp.h $(INCDIR)/satglobalsx.h $(INCDIR)/satglobalspx.h $(INCDIR)/satprop.h $(INCDIR)/satstrings.h $(INCDIR)/hpterm.h $(INCDIR)/vt100.h
	gcc $(SATTRACK_OBJECTS) $(C_LOPTS) -o $(BUILDDIR)/sattrack

# Utilities
clean:
	rm -f $(BUILDDIR)/*.o
	rm $(BUILDDIR)/sattrack

# Compile Object Files
$(BUILDDIR)/satargs.o: $(SATDIR)/satargs.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h
	gcc $(C_OPTS) $(SATDIR)/satargs.c -o $(BUILDDIR)/satargs.o

$(BUILDDIR)/satcalc.o: $(SATDIR)/satcalc.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h
	gcc $(C_OPTS) $(SATDIR)/satcalc.c -o $(BUILDDIR)/satcalc.o

$(BUILDDIR)/satchar.o: $(SATDIR)/satchar.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h
	gcc $(C_OPTS) $(SATDIR)/satchar.c -o $(BUILDDIR)/satchar.o

$(BUILDDIR)/satcity.o: $(SATDIR)/satcity.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h
	gcc $(C_OPTS) $(SATDIR)/satcity.c -o $(BUILDDIR)/satcity.o

$(BUILDDIR)/satctrl.o: $(SATDIR)/satctrl.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h
	gcc $(C_OPTS) $(SATDIR)/satctrl.c -o $(BUILDDIR)/satctrl.o

$(BUILDDIR)/satdisp.o: $(SATDIR)/satdisp.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h $(INCDIR)/hpterm.h $(INCDIR)/vt100.h
	gcc $(C_OPTS) $(SATDIR)/satdisp.c -o $(BUILDDIR)/satdisp.o

$(BUILDDIR)/satelem.o: $(SATDIR)/satelem.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h $(INCDIR)/hpterm.h $(INCDIR)/vt100.h
	gcc $(C_OPTS) $(SATDIR)/satelem.c -o $(BUILDDIR)/satelem.o

$(BUILDDIR)/satgraph.o: $(SATDIR)/satgraph.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h $(INCDIR)/hpterm.h $(INCDIR)/satstrings.h $(INCDIR)/shuttle.h $(INCDIR)/mir.h $(INCDIR)/world.xbm
	gcc $(C_OPTS) $(SATDIR)/satgraph.c $(C_XOPTS) -o $(BUILDDIR)/satgraph.o

$(BUILDDIR)/satheli.o: $(SATDIR)/satheli.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h
	gcc $(C_OPTS) $(SATDIR)/satheli.c -o $(BUILDDIR)/satheli.o

$(BUILDDIR)/satlice.o: $(SATDIR)/satlice.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h
	gcc $(C_OPTS) $(SATDIR)/satlice.c -o $(BUILDDIR)/satlice.o

$(BUILDDIR)/satmath.o: $(SATDIR)/satmath.c $(INCDIR)/sattrack.h
	gcc $(C_OPTS) $(SATDIR)/satmath.c -o $(BUILDDIR)/satmath.o

$(BUILDDIR)/satnute.o: $(SATDIR)/satnute.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h
	gcc $(C_OPTS) $(SATDIR)/satnute.c -o $(BUILDDIR)/satnute.o

$(BUILDDIR)/satprec.o: $(SATDIR)/satprec.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h
	gcc $(C_OPTS) $(SATDIR)/satprec.c -o $(BUILDDIR)/satprec.o

$(BUILDDIR)/satprop.o: $(SATDIR)/satprop.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h $(INCDIR)/satglobalspx.h $(INCDIR)/satprop.h
	gcc $(C_OPTS) $(SATDIR)/satprop.c -o $(BUILDDIR)/satprop.o

$(BUILDDIR)/satread.o: $(SATDIR)/satread.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h $(INCDIR)/hpterm.h $(INCDIR)/vt100.h
	gcc $(C_OPTS) $(SATDIR)/satread.c -o $(BUILDDIR)/satread.o

$(BUILDDIR)/satrefr.o: $(SATDIR)/satrefr.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h
	gcc $(C_OPTS) $(SATDIR)/satrefr.c -o $(BUILDDIR)/satrefr.o

$(BUILDDIR)/sattele.o: $(SATDIR)/sattele.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h
	gcc $(C_OPTS) $(SATDIR)/sattele.c -o $(BUILDDIR)/sattele.o

$(BUILDDIR)/sattest.o: $(SATDIR)/sattest.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h
	gcc $(C_OPTS) $(SATDIR)/sattest.c -o $(BUILDDIR)/sattest.o

$(BUILDDIR)/sattime.o: $(SATDIR)/sattime.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h
	gcc $(C_OPTS) $(SATDIR)/sattime.c -o $(BUILDDIR)/sattime.o

$(BUILDDIR)/sattrack.o: $(SATDIR)/sattrack.c $(INCDIR)/sattrack.h $(INCDIR)/satglobals.h $(INCDIR)/satglobalsp.h $(INCDIR)/satglobalsx.h $(INCDIR)/satglobalspx.h $(INCDIR)/satprop.h $(INCDIR)/satstrings.h $(INCDIR)/hpterm.h $(INCDIR)/vt100.h
	gcc $(C_OPTS) $(SATDIR)/sattrack.c -o $(BUILDDIR)/sattrack.o

$(BUILDDIR)/satvect.o: $(SATDIR)/satvect.c $(INCDIR)/sattrack.h $(INCDIR)/satglobalsx.h
	gcc $(C_OPTS) $(SATDIR)/satvect.c -o $(BUILDDIR)/satvect.o
