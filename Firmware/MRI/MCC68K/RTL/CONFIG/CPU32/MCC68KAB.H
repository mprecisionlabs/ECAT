/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1992 Microtec Research, Inc.				    */
/* All rights reserved                                                      */
/****************************************************************************/
/*
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
*/



/************************************************************************/
/*									*/
/*	The following options are those which might be useful in the	*/
/*	building of compiler libraries.  This is not intended to be an	*/
/*	exhaustive list of options.					*/
/*									*/
/************************************************************************/


/************************************************************************/
/*	Specify Target Processor					*/
/************************************************************************/
/*									*/
/*	-p<processor>		Instruct compiler to generate code for	*/
/*				a particular processor.	 Please see the	*/
/*				User's Guide for allowable and default	*/
/*				values for <processor>.			*/
	#pragma option -pcpu32
/*									*/
/************************************************************************/


/************************************************************************/
/*	Set Addressing Mode for Sections				*/
/************************************************************************/
/*									*/
/*	-acc/-acd/-acr		"Const" data is	referenced according to	*/
/*				these options.				*/
/*				The -acc option causes "const" data to	*/
/*				be allocated in the same segment or	*/
/*				section as code.			*/
/*				The -acd option	causes "const" data to	*/
/*				be allocated in	the same group or	*/
/*				section as data.			*/
/*				The -acr option	causes "const" data to	*/
/*				be allocated to a segment named		*/
/*				{mod}{n}_ROM, of class FAR_ROM.  The	*/
/*				-acr option is available only on	*/
/*				certain processors using segmented	*/
/*				architectures.  Please see your		*/
/*				Reference Manual for more information.	*/
	#pragma option -acc
/*									*/
/*	-aic/-aid		Initialized variables, which are placed	*/
/*				in the "vars" section, are referenced	*/
/*				according to these options.		*/
/*				The -aic option causes items in "vars"	*/
/*				to be referenced according to the -Mc	*/
/*				option.					*/
/*				The -aid option causes items in "vars"	*/
/*				to be referenced according to the -Md	*/
/*				option.					*/
	#pragma option -aid
/*									*/
/*	-alc/-ald/-alr		Literal constants are referenced	*/
/*				according to these options.		*/
/*				The -alc option causes floating-point	*/
/*				constants and automatic aggregate	*/
/*				initializers to be allocated to the	*/
/*				same segment or section as code.	*/
/*				The -ald option causes literal		*/
/*				constants to be allocated in the same	*/
/*				group or section as data.		*/
/*				The -alr option	causes literal		*/
/*				constants to be allocated to a segment	*/
/*				named {mod}{n}_ROM, of class FAR_ROM.	*/
/*				The -alr option is available only on	*/
/*				certain processors using segmented	*/
/*				architectures.  Please see your		*/
/*				Reference Manual for more information.	*/
	#pragma option -alc
/*									*/
/*	-asc/-asd/-asr		String constants are referenced		*/
/*				according to these options.		*/
/*				The -asc option allocates string	*/
/*				constants to the same segment or	*/
/*				section as code.			*/
/*				The -asd option allocates string	*/
/*				constants to the same group or section	*/
/*				as data.				*/
/*				The -asr option allocates string	*/
/*				constants to a segment named		*/
/*				{mod}{n}_ROM, of class FAR_ROM.	 The	*/
/*				-asr option is available only on	*/
/*				certain processors using segmented	*/
/*				architectures.  Please see your		*/
/*				Reference Manual for more information.	*/
	#pragma option -asc
/*									*/
/************************************************************************/


/************************************************************************/
/*	Generate Instructions for Floating-Point Coprocessor		*/
/************************************************************************/
/*									*/
/*	-f/-nf			The -f option generates instructions	*/
/*				for a floating point coprocessor.	*/
/*				A default for this option is not	*/
/*				specified here because it may conflict	*/
/*				with the processor specified with the	*/
/*				-p option.				*/
/*									*/
/************************************************************************/


/************************************************************************/
/*	Control of Debug Information					*/
/************************************************************************/
/*									*/
/*	-Gd/-nGd		The -Gd option generates debug		*/
/*				information for preprocessor macros.	*/
/*				This option is used in conjunction with */
/*				the -g option.				*/
	#pragma option -Gd
/*									*/
/*	-Gf/-nGf		The -Gf option places fully qualified	*/
/*				pathnames for source files into object	*/
/*				file. This option is used in		*/
/*				conjunction with the -g option. 	*/
	#pragma option -nGf
/*									*/
/*	-Gl/-nGl		The -Gl option insert line number	*/
/*				labels of the form LL<nn> where <nn> is	*/
/*				the line number	into the object file.	*/
/*				These line number labels will not be	*/
/*				recognized by XRAY, but may be used in	*/
/*				other debugging environments.  The -Gl	*/
/*				and the -g options are mutually		*/
/*				exclusive.				*/
	#pragma option -nGl
/*									*/
/*	-Gm/-nGm		The -Gm option enables debug		*/
/*				information which allows stepping by	*/
/*				individual statements on lines which	*/
/*				contain more than one statement.	*/
/*				Disabling this feature may be useful if	*/
/*				using old debugger versions. The -Gm	*/
/*				option is used in conjunction with the	*/
/*				-g option.				*/
	#pragma option -nGl -Gm
/*									*/
/* 	-Gr/-nGr		The -Gr option generates restricted	*/
/*				debug information.  This level of debug	*/
/*				information is used for production	*/
/*				libraries.				*/
/*				This option is incompatible with the -g	*/
/*				option.					*/
	#pragma option -ng -Gr
/*									*/
/*	-Gs/-nGs		The -Gs option generates debug		*/
/*				information for XRAY Source Explorer.	*/
/*				The -Gs option is used in conjunction	*/
/*				with the -g option. 			*/
	#pragma option -nGs
/*									*/
/*	-g/-ng			The -g option generates full debug	*/
/*				information.				*/
	#pragma option -ng
/*									*/
/************************************************************************/


/************************************************************************/
/*	HP 64000 Support						*/
/************************************************************************/
/*									*/
/*	-h/-nh			The -h option generates code for use on */
/*				Hewlett-Packard 64000 Series		*/
/*				Development Systems.  The code includes	*/
/*				two special labels, Rlabel and Elabel,	*/
/*				that the HP 64000 uses to identify each	*/
/*				function's exit and end points for	*/
/*				debugging and timing purposes.  Rlabel	*/
/*				represents the function's return point,	*/
/*				and Elabel represents the function's	*/
/*				end address.  The label portion of the  */
/*				symbol name is the name of the		*/
/*				function.				*/
	#pragma option -nh
/*									*/
/************************************************************************/


/************************************************************************/
/*	Search Paths for Include Files					*/
/************************************************************************/
/*									*/
/*	-I<dir>			The -I option includes <dir> in your	*/
/*				search path for	include files specified	*/
/*				with double quotes (" ") in "#include"	*/
/*				statements.				*/
/*									*/
/*	-J<dir>			The -J option includes <dir> in your	*/
/*				search path for	include files specified	*/
/*				with angle brackets (< >) in "#include"	*/
/*				statements.				*/
	#if _UNIX
	    #pragma option -J../include
	#else
	    #pragma option -J..\include
	#endif
/*									*/
/************************************************************************/


/************************************************************************/
/*	Code Generation Variations					*/
/************************************************************************/
/*									*/
/*	-Kc/-nKc		The -Kc option generates "CLR"		*/
/*				instructions for assigning zero to	*/
/*				non-register variables.	The -nKc option	*/
/*				does not use the "CLR" instruction.	*/
	#pragma option -nKc
/*									*/
/*	-Kf/-nKf		The -Kf option disables certain		*/
/*				prologue and epilogue optimizations.	*/
/*				Consult the manual as to the precise	*/
/*				nature of these	optimizations.		*/
	#pragma option -nKf
/*									*/
/*	-Kh<n>			The -Kh option reserves the single	*/
/*				register <n>.  This option cannot be	*/
/*				used if a -Md<n> option is used.	*/
/*									*/
/*									*/
/*	-KI/-nKI		The -KI option initializes all local	*/
/*				variables to zero.  Local variables	*/
/*				specifically initialized in your code	*/
/*				will subsequently be reinitialized with	*/
/*				the specified value.  This option does	*/
/*				not affect parameters.  This option is	*/
/*				designed to aid debugging.  This option	*/
/*				may significantly decrease run-time	*/
/*				performance for functions containing a	*/
/*				large number of local variables or a	*/
/*				large local variable.			*/
	#pragma option -nKI
/*									*/
/*	-Km/-nKm		The -Km option generates messenger	*/
/*				symbols. The -nKm option suppresses the	*/
/*				generation of these symbols. Please see */
/*				the Reference Manual for more		*/
/*				information.				*/
	#pragma option -Km
/*									*/
/*	-KP/-nKP		The -KP option makes every structure	*/
/*				and union packed unless they are	*/
/*				explicitly declared as unpacked.  This 	*/
/*				option will apply not only to		*/
/*				structures in source files, but also to	*/
/*				all structures in header files.		*/
	#pragma option -nKP
/*									*/
/*	-Kq/-nKq		The -Kq option causes floating point	*/
/*				"doubles" to be compiled as 32-bit,	*/
/*				single-precision "floats".		*/
	#pragma option -nKq
/*									*/
/*	-Kr/-nKr		The -Kr option forces interrupt		*/
/*				procedures to return using a "return	*/
/*				from subroutine" type assembly		*/
/*				instruction.  Normally	these		*/
/*				procedures return with a "return from	*/
/*				interrupt" type assembly instruction.	*/
	#pragma option -nKr
/*									*/
/*	-Kt/-nKt		The -Kt option generates tags for each	*/
/*				function's entry and exit points.	*/
/*				These tags may be used with analysis	*/
/*				tools such as Cadre Soft Analyst.	*/
	#pragma option -nKt
/*									*/
/*	-Ku/-nKu		The -Ku option causes "char" variables	*/
/*				declared without a "signed" or		*/
/*				"unsigned" keyword to be considered	*/
/*				"unsigned".				*/
	#pragma option -nKu
/*									*/
/*	-Kv/-nKv		The -Kv option causes bit fields	*/
/*				declared without a "signed" or		*/
/*				"unsigned" keyword to be considered	*/
/*				"unsigned".  The building of the 	*/
/*				libraries is insensitive to this	*/
/*				option.					*/
	#pragma option -nKv
/*									*/
/************************************************************************/


/************************************************************************/
/*	Generate Position Independent Code and Data			*/
/************************************************************************/
/*									*/
/*	-Mcp/-Mca		The -Mcp option uses program counter	*/
/*				relative addressing for all code	*/
/*				references.  The -Mca option uses	*/
/*				absolute addressing.			*/
	#pragma option -Mca
/*									*/
/*	-Md<n>/-Mda		The -Md<n> option uses <n>-register-	*/
/*				relative addressing for all static data	*/
/*				references.  On processors which	*/
/*				support it, -Mdp uses program counter	*/
/*				relative addressing.  -Mda uses		*/
/*				absolute addressing.			*/
	#pragma option -Mda
/*									*/
/*	-Ml			Where applicable, the -Ml options use	*/
/*				32-bit displacements for 		*/
/*				<n>-register-relative addressing.	*/ 
/*									*/
/************************************************************************/


/************************************************************************/
/*	Specify Non-Standard Names for Sections	and Compiled Module	*/
/************************************************************************/
/*									*/
/*	-NC<name>		The -NC option places "const" data into	*/
/*				the section specified by <name>.	*/
	#pragma option -NCcode
/*									*/
/*	-NI<name>		The -NI option places initialized data	*/
/*				into the section specified by <name>.	*/
/*									*/
/*	-NL<name>		The -NL option places literals into the	*/
/*				section	specified by <name>.		*/
	#pragma option -NLcode
/*									*/
/*	-NM<name>		The -NM option sets the module name to	*/
/*				<name>.					*/
/*									*/
/*	-NS<name>		The -NS option places strings in the	*/
/*				section specified by <name>.		*/
	#pragma option -NScode
/*									*/
/*	-NT<name>		The -NT option places executable code	*/
/*				in the section specified by <name>.	*/
/*									*/
/*	-NZ<name>		The -NZ option places uninitialized	*/
/*				static data in the section specified by	*/
/*				<name>.					*/
/*									*/
/************************************************************************/


/************************************************************************/
/*	Control of Compiler Optimizations				*/
/************************************************************************/
/*									*/
/*	-Ob/-nOb		If "-Og" is turned on, turn on global	*/
/*				variable optimization.  This		*/
/*				optimization assumes that global	*/
/*				variables are always referenced		*/
/*				explicitly, never by pointers.		*/
/*				This option should not be used on	*/
/*				programs that use global variables as	*/
/*				operating system memory locations or	*/
/*				I/O device registers.			*/
/*				This should be considered an "unsafe"	*/
/*				optimization.  Consult the manual	*/
/*				before using this option.		*/
	#pragma option -nOb
/*									*/
/*	-Oc/-nOc		The -Oc option turns on stack pop	*/
/*				optimization.  The stack will be	*/
/*				allowed to grow and will be popped	*/
/*				after several function calls.  The -nOc */
/*				option causes the stack to be popped	*/
/*				after each function call.		*/
	#pragma option -Oc
/*									*/
/*	-Oe/-nOe		The -Oe option generates only one exit	*/
/*				(return) for each function.		*/
	#pragma option -nOe
/*									*/
/*	-Og/-nOg		The -Og option enables global flow	*/
/*				optimization.				*/
	#pragma option -Og
/*									*/
/*	-Oi/-nOi		The -Oi option enables function		*/
/*				in-lining optimizing.  This option must */
/*				be used in conjunction with the -Og	*/
/*				option.					*/
/*				In-lining is a code generation		*/
/*				technique which moves the code of a	*/
/*				called function into the caller.  This	*/
/*				technique eliminates the procedure call */
/*				overhead (branching and stack		*/
/*				allocation) and allows for the		*/
/*				possibility of improved code		*/
/*				optimization such as constant folding	*/
/*				and loop optimizations.			*/
	#pragma option -Oi
/*									*/
/*	-Ol/-nOl		The -Ol option enables local		*/
/*				optimizations:  code hoisting and cross	*/
/*				jump optimizations.  This option must	*/
/*				be used with the -Og option.		*/
	#pragma option -Ol
/*									*/
/*	-OR/-nOR		The -nOR option works only with -nOg.	*/
/*				If the global optimizer is used, -Og,	*/
/*				local variables will be allocated to	*/
/*				registers using a register coloring	*/
/*				algorithm.  If the global optimizer is	*/
/*				not used (the compiler default is	*/
/*				-nOg), heavily used local variables	*/
/*				will be allocated to registers (-OR).	*/
/*				The -nOR option may be used to disable	*/
/*				the allocation of heavily used variable	*/
/*				to registers.				*/
	#pragma option -OR
/*									*/
/*	-Or/-nOr		The -Or option enables instruction	*/
/*				scheduling optimization.  Instructions	*/
/*				will be reordered to take advantage of	*/
/*				the target processor's architecture.	*/
	#pragma option -Or
/*									*/
/*	-Os/-Ot			If -Os is specified, optimize in favor	*/
/*				of code size rather than execution	*/
/*				time.  If -Ot is specified, optimize in	*/
/*				favor of execution time rather than	*/
/*				code size.  These options may be used	*/
/*				if the -Og option is used.		*/
	#pragma option -Ot
/*									*/
/************************************************************************/


/************************************************************************/
/*      Control Generation of Symbol Names                              */
/************************************************************************/
/*                                                                      */
/*	-upd/-upu/-us		Prepend a dot, underscore, or nothing,	*/
/*				respectively, to public symbol names.	*/
/*				Also, -up. can be used for -upd and	*/
/*				-up_ can be used for -upu.		*/
	#pragma option -upu
/*									*/
/*	-ut<number>/-ut0	The -ut<number> option truncates	*/
/*				program	identifiers to a length of	*/
/*				<number>.  The -ut0 option causes	*/
/*				identifiers to not be truncated.	*/
	#pragma option -ut0
/*									*/
/************************************************************************/


/************************************************************************/
/*	Control of Uninitialized Global Data				*/
/************************************************************************/
/*									*/
/*	-Xc/-Xp/-X0		These options control the placement of	*/
/*				weak externals.  A "weak external" is	*/
/*				a global-level variable that is not	*/
/*				initialized on its declaration.  These	*/
/*				three options are mutually exclusive.	*/
/*									*/
/*				The -Xc option will cause weak		*/
/*				externals to be declared as "C common".	*/
/*				The -Xp option will cause weak		*/
/*				externals to be declared with storage	*/
/*				allocated but not initialized.  The -X0	*/
/*				option causes weak externals to	be	*/
/*				allocated and initialized to zero.	*/
/*									*/
/*				Please consult the manual for more	*/
/*				information.				*/
	#pragma option -Xc
/*									*/
/************************************************************************/


/************************************************************************/
/*	Specify Structure Alignment					*/
/************************************************************************/
/*									*/
/*	-Za2/-Za4		The -Za2 option forces non-char		*/
/*				structure members to be even-byte	*/
/*				aligned.  The -Za4 option forces 4-byte	*/
/*				structure members to be quad-byte	*/
/*				aligned; 2-byte structure members are	*/
/*				even-byte aligned. The -Za options are	*/
/*				the preferred form of the -Z options.	*/
	#pragma option -Za2
/*									*/
/*	-Ze/-Zo			When the -Zp2 option is being used, one	*/
/*				may control whether a character is	*/
/*				pushed on the stack on an odd address	*/
/*				or an even address.  The -Ze option	*/
/*				will cause characters to be pushed on	*/
/*				an even address. The -Zo option will	*/
/*				cause characters to be pushed on an odd */
/*				address.				*/
	#pragma option -Zo
/*									*/
/*	-Zm<number>		The -Zm option makes any structure size	*/
/*				a multiple of <number>.  A warning	*/
/*				message is generated when padding bytes	*/
/*				are added to a structure.		*/
	#pragma option -Zm1
/*									*/
/*	-Zp2/-Zp4		The -Zp2 option pushes 16 bits on the	*/
/*				stack for function parameters when the	*/
/*				parameter is less that 16 bits and	*/
/*				belong to a prototyped function. The	*/
/*				-Zp4 option causes 32 bits to be pushed.*/
	#pragma option -Zp4
/*									*/
/************************************************************************/


/************************************************************************/
/*	PRINTF/SCANF CUSTOMIZATIONS:					*/
/*									*/
/*	The following preprocessor symbols can be used for the		*/
/*	customization of the sprintf, fprintf, printf, sscanf, fscanf,	*/
/*	and scanf routines.  						*/
/************************************************************************/
/*									*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_h_OPT			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_l_OPT			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_L_OPT			*/
/*				Defining these preprocessor symbols 	*/
/*				disables the "h", "l", and "L" options,	*/
/*				respectively, in formatted i/o		*/
/*				functions.				*/
/*									*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_STAR_OPT			*/
/*				Defining this preprocessor symbol	*/
/*				disables the "*" option in formatted	*/
/*				output functions.			*/
/*									*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_ASSGN_SUPP			*/
/*				Defining this preprocessor symbol	*/
/*				disables the assignment suppression	*/
/*				option, "*", of formatted input		*/
/*				functions.				*/
/*									*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_MINUS_FLAG			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_PLUS_FLAG			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_SPACE_FLAG			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_SHARP_FLAG			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_ZERO_FLAG			*/
/*				Defining these preprocessor symbols	*/
/*				disable the "-", "+", " ", "#", and "0"	*/
/*				flags, respectively, in formatted	*/
/*				output functions.			*/
/*									*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_INT_FMT			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_DEC_FMT			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_OCT_FMT			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_UNS_FMT			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_HEX_FMT			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_FLOAT_FMT			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_CHAR_FMT			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_STR_FMT			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_PNT_FMT			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_NUMB_FMT			*/
/*	#pragma option -DEXCLUDE_FORMAT_IO_BRAKT_FMT			*/
/*				Defining these preprocessor symbols	*/
/*				disables the "i", "d", "o", "u", "x",	*/
/*				"f", "c", "s", "p", "n", and "["	*/
/*				formats, respectively, in formatted	*/
/*				input and output routines.		*/
/*									*/
/*				Note in particular that if the "f"	*/
/*				format is disabled, the formatted input	*/
/*				and output routines will not force the	*/
/*				linking of floating point support	*/
/*				routines.				*/
/*									*/
/************************************************************************/


/************************************************************************/
/*	Line Buffering and Terminal Simulation				*/
/*									*/
/*	The following group of options controls the functioning of line	*/
/*	buffering and terminal simulation.				*/
/************************************************************************/
/*									*/
	#pragma option -DEXCLUDE_LINE_BUFFER_DEFAULT
/*				If you wish stdin and stdout to be line	*/
/*				buffered by default, disable the above	*/
/*				preprocessor symbol.  If you wish stdin	*/
/*				and stdout to be unbuffered by default,	*/
/*				enable the above preprocessor symbol.	*/
/*									*/
/*				Normally, if the line buffering default	*/
/*				is used, both the			*/
/*				EXCLUDE_LINE_BUFFER_DEFAULT and		*/
/*				EXCLUDE_TERMINAL_SIMULATION		*/
/*				preprocessor symbols are disabled.	*/
/*									*/
	#pragma option -DEXCLUDE_TERMINAL_SIMULATION
/*				If the above preprocessor symbol is not	*/
/*				defined, the "read" function assumes	*/
/*				that file descriptor 0 is being used to */
/*				communicate with a terminal-like device */
/*				and no others are, at least for input.	*/
/*				This assumption may not be valid in the	*/
/*				actual application.			*/
/*									*/
/*				If you do not wish to have read()	*/
/*				treat input from file descriptor 0 	*/
/*				as if it were coming from a terminal,	*/
/*				define the above preprocessor symbol.	*/
/*				Note: generally file descriptor 0 is	*/
/*				stdin.					*/
/*									*/
/*				Normally, if the line buffering default	*/
/*				is used, both the			*/
/*				EXCLUDE_LINE_BUFFER_DEFAULT and		*/
/*				EXCLUDE_TERMINAL_SIMULATION		*/
/*				preprocessor symbols are disabled.	*/
/*									*/
	#pragma option -DLINE_BUFFER_SIZE=128
/*				The above preprocessor symbol defines	*/
/*				the buffer size in bytes used for line	*/
/*				buffering.  The size of buffer used for	*/
/*				full buffering is specified by the	*/
/*				symbol BUFSIZ which is located in	*/
/*				<stdio.h>.				*/
/*									*/
/************************************************************************/


/************************************************************************/
/*	Initialization 							*/
/*									*/
/*	The following group of options modifies the behavior of the C	*/
/*	initialization routine: csys.c.					*/
/************************************************************************/
/*									*/
/*	#pragma option -DEXCLUDE_INITDATA				*/
/*				The default behavior of csys.c is to 	*/
/*				execute _initcopy() to initialize	*/
/*				memory.	If the above preprocessor	*/
/*				symbol is used, _initcopy is not called.*/
/*				If the user's application does not use	*/
/*				initialized memory, this option may be	*/
/*				used. Note the the libraries do not use */
/*				initialized memory, so they do not	*/
/*				require the use of initcopy().		*/
/*									*/
/*	#pragma option -DINCLUDE_BUILD_ARGV				*/
/*				The default behavior of csys.c is to	*/
/*				not include build_argv(). If this	*/
/*				preprocessor option is set, build_argv	*/
/*				is included. This routine parses a	*/
/*				command line, located in _com_line, and	*/
/*				establishes argc and argv. These two	*/
/*				parameters are then passed to main().	*/
/*									*/
/*	#pragma option -DEXCLUDE_ATEXIT					*/
/*				If the above preprocessor symbol is	*/
/*				defined, data supporting atexit() is	*/
/*				removed from csys.c. Also, the		*/
/*				execution of the atexit functions by	*/
/*				exit() is removed.			*/
/*									*/
/*	#pragma option -DEXCLUDE_SIGNAL_RAISE				*/
/*				If the above preprocessor symbol is	*/
/*				defined, the definition of data		*/
/*				supporting signal() and	raise() is	*/
/*				removed from csys.c.			*/
/*									*/
/*	#pragma option -DEXCLUDE_IOB					*/
/*				_iob is an array of structs which is	*/
/*				used by the C i/o. If the application 	*/
/*				routine does not use C i/o, then _iob	*/
/*				can be removed.	Defining the above	*/
/*				preprocessor symbol will remove the	*/
/*				definition of _iob from csys.c.		*/
/*									*/
/*	#pragma option -DEXCLUDE_ERRNO					*/
/*				If the application routine does not	*/
/*				call any library routine which sets	*/
/*				errno, then errno need not be defined.	*/
/*				Setting the above preprocessor symbol	*/
/*				removes the definition of errno from	*/
/*				csys.c.					*/
/*									*/
/*									*/
/*	#pragma option -DEXCLUDE_STRTOK					*/
/*				The string function strtok() uses the	*/
/*				static variable _lastp. If the		*/
/*				application routine does not call	*/
/*				strtok(), then the above preprocessor	*/
/*				symbol may be defined. This removes	*/
/*				the definition of _lastp from csys.c.	*/
/*									*/
/*	#pragma option -DEXCLUDE_RAND					*/
/*				The functions rand() and srand() use	*/
/*				the static variable _randx. If the	*/
/*				application routine does not call these	*/
/*				two functions, then the above		*/
/*				preprocessor symbol may be defined.	*/
/*				This removes the definition of _randx	*/
/*				from csys.c.				*/
/*									*/
/*	#pragma option -DEXCLUDE_TIME					*/
/*				Several functions in <time.h> use	*/
/*				static variables. In particular,	*/
/*				asctime() and ctime() use _ctbuf; and	*/
/*				gmtime() and localtime() use _xtm. If	*/
/*				the application routine does not call	*/
/*				these functions, then the above		*/
/*				preprocessor symbol may be defined.	*/
/*				This removes the definition of _ctbuf	*/
/*				and _xtm from csys.c.			*/
/*									*/
/*	#pragma option -DEXCLUDE_MALLOC					*/
/*				The memory management routines malloc(),*/
/*				calloc(), free(), realloc(), and	*/
/*				zalloc() use static variables. Also the	*/
/*				C i/o functions call malloc() and	*/
/*				free(). If the application routine does	*/
/*				not call the memory mangement routines	*/
/*				and does not make use of C i/o		*/
/*				functions, then the above preprocessor	*/
/*				symbol may be defined. This removes the	*/
/*				definition of these variables from	*/
/*				csys.c.					*/
/*									*/
/************************************************************************/


/************************************************************************/
/*	Math Library Customizations					*/
/************************************************************************/
/*									*/
/*	#pragma option -DINCLUDE_FAST_POW				*/
/*				The default pow () function is	quite	*/
/*				accurate.  This function should	be used	*/
/*				in all situations where	accuracy is of	*/
/*				extreme importance. However, if your	*/
/*				application requires faster speed, you	*/
/*				may want to define the above		*/
/*				preprocessor symbol. This enables a	*/
/*				faster pow () function which is not	*/
/*				quite as accurate, but should suffice	*/
/*				in many applications. The faster	*/
/*				pow ()'s main drawback is inaccuracy as	*/
/*				the first argument gets close to 1. The	*/
/*				fast pow() runs about twice as fast as	*/
/*				the default.				*/
/*									*/
/*				Note that if the -Kq option is used,	*/
/*				the setting of the above symbol will	*/
/*				not affect the selection of the		*/
/*				pow () function. There is only one	*/
/*				single precision version of pow ().	*/
/*									*/
/************************************************************************/


