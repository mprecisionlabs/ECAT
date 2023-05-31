CC=$(ECATMRIBIN)
LD=$(ECATMRILNK)

CFLAGS=-c -nQ -nKc -A -Xp -zc $(BETA_RELEASE) -o$@

EXEC_LOC=$(ECATPROJ)
SOURCE_LOC=$(ECATSOURCE)
OBJ_LOC=$(ECATSOURCE)

BIN=$(EXEC_LOC)\update.bin
ABS=$(EXEC_LOC)\update.abs
LIB=$(ECATLIBSOURCE)\keytek.lib
LIBRELEASE=$(ECATLIB)\keytek.lib

%.obj:%.c
	$(CC) $(CFLAGS) $*.c

%.obj:%.src
	$(CC) $(CFLAGS) $*.src

OBJECTS=$(OBJ_LOC)\ecat.obj\
		$(OBJ_LOC)\front.obj\
		$(OBJ_LOC)\lpray.obj\
		$(OBJ_LOC)\fixit.obj\
		$(OBJ_LOC)\csel.obj\
		$(OBJ_LOC)\intface.obj\
		$(OBJ_LOC)\tiocntl.obj\
		$(OBJ_LOC)\fp_srg.obj\
		$(OBJ_LOC)\fp_main.obj\
		$(OBJ_LOC)\fp_eft.obj\
		$(OBJ_LOC)\cmd_srg.obj\
		$(OBJ_LOC)\cmd_bay.obj\
		$(OBJ_LOC)\cmd_li.obj\
		$(OBJ_LOC)\cmd_root.obj\
		$(OBJ_LOC)\cmd_meas.obj\
		$(OBJ_LOC)\cmd_eft.obj\
		$(OBJ_LOC)\cmd_data.obj\
		$(OBJ_LOC)\krnl_srg.obj\
		$(OBJ_LOC)\krnl_eft.obj\
		$(OBJ_LOC)\krnl_pqf.obj\
		$(OBJ_LOC)\fp_pqf.obj\
		$(OBJ_LOC)\ad_trap.obj\
		$(OBJ_LOC)\ads_trap.obj\
		$(OBJ_LOC)\acrms.obj\
		$(OBJ_LOC)\ehv.obj\
		$(OBJ_LOC)\krnl_emc.obj\
		$(OBJ_LOC)\startup.obj

all: $(LIB) $(ABS) $(BIN)

$(BIN): $(ABS)
	$(ECAT_SREC2BIN) -q -oo 80000 $(ABS) $(BIN)

$(ABS): $(LIB) $(OBJECTS) 
	$(LD)  -l$(LIBRELEASE) -l$(MRI_RUNTIME_LIB) -c $(ECATTOOLS)\rom.cmd -o$(ABS) -m >ecat.map

$(LIB):
	@call $(ECATTOOLS)\call_build-lib.bat


