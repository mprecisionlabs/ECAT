CC=$(ECATMRIBIN)
LB=$(ECATMRILIB)

CFLAGS=-c -nQ -nKc -A -Xp -zc $(BETA_RELEASE) -o$@

EXEC_LOC=$(ECATLIB)
SOURCE_LOC=$(ECATLIBSOURCE)
OBJ_LOC=$(ECATLIBSOURCE)

%.obj:%.c
	$(CC) $(CFLAGS) $*.c

%.obj:%.src
	$(CC) $(CFLAGS) $*.src

OBJECTS=$(OBJ_LOC)\entry.obj\
	$(OBJ_LOC)\csys68k.obj  \
	$(OBJ_LOC)\tpu_efrq.obj \
	$(OBJ_LOC)\tpu_init.obj \
	$(OBJ_LOC)\tpu_chin.obj \
	$(OBJ_LOC)\tpu_wrte.obj \
	$(OBJ_LOC)\tpu_read.obj \
	$(OBJ_LOC)\tpu_tcr1.obj \
	$(OBJ_LOC)\tpu_dlay.obj \
	$(OBJ_LOC)\tpu_list.obj \
	$(OBJ_LOC)\tpu_pwm.obj  \
	$(OBJ_LOC)\tpu_spwm.obj \
	$(OBJ_LOC)\tpu_trap.obj \
	$(OBJ_LOC)\tpu_sfrq.obj \
	$(OBJ_LOC)\tpu_rfrq.obj \
	$(OBJ_LOC)\tpu_sevt.obj \
	$(OBJ_LOC)\tpu_revt.obj \
	$(OBJ_LOC)\tpu_shot.obj \
	$(OBJ_LOC)\tpu_intr.obj \
	$(OBJ_LOC)\tpu_pint.obj \
	$(OBJ_LOC)\tpu_dint.obj \
	$(OBJ_LOC)\tpu_trig.obj \
	$(OBJ_LOC)\tpu_kill.obj \
	$(OBJ_LOC)\tpu_sph.obj	\
	$(OBJ_LOC)\brst_one.obj \
	$(OBJ_LOC)\brst_beg.obj \
	$(OBJ_LOC)\brst_end.obj \
	$(OBJ_LOC)\brst_do.obj  \
	$(OBJ_LOC)\brst_upd.obj \
	$(OBJ_LOC)\qsm_init.obj \
	$(OBJ_LOC)\qsm_dset.obj \
	$(OBJ_LOC)\qsm_12st.obj \
	$(OBJ_LOC)\qsm_halt.obj \
	$(OBJ_LOC)\qsm_dmin.obj \
	$(OBJ_LOC)\qsm_dmax.obj \
	$(OBJ_LOC)\qsm_drms.obj \
	$(OBJ_LOC)\qsm_davg.obj \
	$(OBJ_LOC)\qsm_drst.obj \
	$(OBJ_LOC)\qsm_read.obj \
	$(OBJ_LOC)\qsm_sclc.obj \
	$(OBJ_LOC)\qsm_eclc.obj \
	$(OBJ_LOC)\qsm_trap.obj \
	$(OBJ_LOC)\mem_wrte.obj \
	$(OBJ_LOC)\mem_read.obj \
	$(OBJ_LOC)\yy_parse.obj \
	$(OBJ_LOC)\yy_lprse.obj \
	$(OBJ_LOC)\yy_lex.obj	\
	$(OBJ_LOC)\yy_error.obj \
	$(OBJ_LOC)\yy_reprt.obj \
	$(OBJ_LOC)\gpb_rsts.obj \
	$(OBJ_LOC)\gpb_ssts.obj \
	$(OBJ_LOC)\gpb_dbug.obj \
	$(OBJ_LOC)\gpb_wgpb.obj \
	$(OBJ_LOC)\gpb_gchr.obj \
	$(OBJ_LOC)\gpb_fini.obj \
	$(OBJ_LOC)\gpb_init.obj \
	$(OBJ_LOC)\gpb_gbuf.obj \
	$(OBJ_LOC)\gpb_sbuf.obj \
	$(OBJ_LOC)\gpb_ssrq.obj \
	$(OBJ_LOC)\gpb_rsrq.obj \
	$(OBJ_LOC)\gpb_wsrq.obj \
	$(OBJ_LOC)\gpb_intr.obj \
	$(OBJ_LOC)\gpb_cavl.obj \
	$(OBJ_LOC)\buf_init.obj \
	$(OBJ_LOC)\buf_free.obj \
	$(OBJ_LOC)\buf_kill.obj \
	$(OBJ_LOC)\buf_rset.obj \
	$(OBJ_LOC)\buf_wchr.obj \
	$(OBJ_LOC)\buf_rchr.obj \
	$(OBJ_LOC)\buf_left.obj \
	$(OBJ_LOC)\pit_init.obj \
	$(OBJ_LOC)\trp_hndl.obj \
	$(OBJ_LOC)\key_rset.obj \
	$(OBJ_LOC)\key_rchr.obj \
	$(OBJ_LOC)\key_rraw.obj \
	$(OBJ_LOC)\key_beep.obj \
	$(OBJ_LOC)\key_wait.obj \
	$(OBJ_LOC)\key_trap.obj \
	$(OBJ_LOC)\key_rled.obj \
	$(OBJ_LOC)\key_led.obj  \
	$(OBJ_LOC)\lcd_krnl.obj \
	$(OBJ_LOC)\lcd_putc.obj \
	$(OBJ_LOC)\lcd_puts.obj \
	$(OBJ_LOC)\lcd_rset.obj \
	$(OBJ_LOC)\lcd_getx.obj \
	$(OBJ_LOC)\lcd_gety.obj \
	$(OBJ_LOC)\lcd_goxy.obj \
	$(OBJ_LOC)\lcd_curs.obj \
	$(OBJ_LOC)\lcd_bmap.obj \
	$(OBJ_LOC)\lcd_lite.obj \
	$(OBJ_LOC)\lcd_norm.obj \
	$(OBJ_LOC)\lcd_draw.obj \
	$(OBJ_LOC)\lcd_line.obj \
	$(OBJ_LOC)\lcd_crcl.obj \
	$(OBJ_LOC)\lcd_box.obj  \
	$(OBJ_LOC)\sci_init.obj \
	$(OBJ_LOC)\sci_trap.obj \
	$(OBJ_LOC)\sci_kill.obj \
	$(OBJ_LOC)\sci_cavl.obj \
	$(OBJ_LOC)\sci_tx.obj   \
	$(OBJ_LOC)\sci_rx.obj   \

$(OBJ_LOC)\keytek.lib: $(OBJECTS)
	@if exist $(OBJ_LOC)\keytek.lib del $(OBJ_LOC)\keytek.lib
	$(LB) < $(ECATLIBTOOLS)\buildlib.cmd
	@call copy $(OBJ_LOC)\keytek.lib $(ECATLIB)

