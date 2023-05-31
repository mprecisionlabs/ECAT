Uses DOS,CRT,UNIT_a0,Unit_A1,Unit_A2,BASE_CON;

type
one_element = record
                 c:char;
                 attr:byte;
               end;
Screen_line = array [1..80] of one_element;
Screen_buffer= array [1..25] of Screen_line;
Screen_ptr = ^Screen_buffer;


var i:Integer;
    Video_mode   : byte absolute $0040:$0049;    { video mode address }
    ScreenPtr    : Screen_Ptr;                 { pointer to screen memory }
    c:Char;
    s:String;
    f:Text;
    lc:integer;
    org_color:byte;

{********************************************************************}
{ procedure to change a character attribute.                         }
{____________________________________________________________________}
procedure attrxy(x,y:integer;attribute:byte);
begin
 ScreenPtr^[y,x].attr :=attribute;
end;

{********************************************************************}
{ procedure to change a character value    .                         }
{____________________________________________________________________}
procedure charxy(x,y:integer; cv:Char);
begin
 ScreenPtr^[y,x].c :=cv;
end;

{********************************************************************}
{ procedure to change a character value and attribute                }
{____________________________________________________________________}
procedure charaxy(x,y:integer; cv:Char; ar:Byte);
begin
 ScreenPtr^[y,x].c :=cv;
 ScreenPtr^[y,x].attr :=ar;
end;



Procedure Box(x1,y1,x2,y2,col:Integer);
Var x,y:integer;
Begin
for x:=x1 to x2 do
    Begin
     charaxy(x,y1,chr(205),col);
     charaxy(x,y2,chr(205),col);
    End;
for y:=y1 to y2 do 
    Begin
     charaxy(x1,y,chr(186),col);
     charaxy(x2,y,chr(186),col);
    End;

charaxy(x1,y1,chr(201),col);
charaxy(x1,y2,chr(200),col);
charaxy(x2,y2,chr(188),col);
charaxy(x2,y1,chr(187),col);
End;



Const
          F1 = 59;
          F2 = 60;
          F3 = 61;
          F4 = 62;
          F5 = 63;
          F6 = 64;
          F7 = 65;
          F8 = 66;
          F9 = 67;
      	 F10 = 68;
          AF7 =110;
          AF8 =111;
	      End_Key=79;     {Code of the 'END' key }
         LF_Key=75;
         Rt_key=77;
         Pg_Up =73;
         Key_Ins=82;
         PG_DN =81;
         alt_i = 23;
         alt_j = 36;
         alt_k = 37;
         alt_f = 33;
         alt_s = 31;
         alt_p = 25;
Abort_wait   = 5; {Wait 5 seconds before you abort}
          Def_In_queue      = 32000;
          Def_Out_Queue     = 128;
          Const Down_K=780;

Var
    C_Stat,C_NUM:WORD;
    Timetic:Longint absolute $040:$06C;
    State:_Optionrecord; {Variable to hold the port parameters}
    Error_cause:String;
    Good,Bad:Integer;
    Baud_set:Word;
    W_Val:Word;
    CC:Char;
    T:text;

Function Uppercase(s:String):String;
Var i:integer;
    su:string;
Begin
su:=s;
for i:=1 to length(s)
 do su[i]:=upcase(su[i]);
 Uppercase:=su;
End;


function hexdig(c:char):integer;
Begin
if (c>'9')then  hexdig:=(ord(c)-ord('A'))+10
else
hexdig:=(ord(c)-ord('0'));

End;


Function Fileexists(s:String):boolean;
Var F:file;
Begin
{$I-}
assign(F,S);
Reset(f);
Close(f);
{$I+}
Fileexists:= (ioresult=0) and (s<>'');
End;


PROCEDURE LAST_ERROR(e:wORD; S:String);
Begin
Writeln('Had an error in the serial port system ');
Writeln('Please record the following: ',S,'  and  ',E);
Write('Hit enter or return to continue');
Readln;
End;

Function Char_Avail :Boolean;
Var ST,S,PS:Word;
    C:Char;
Begin
ST:=__IQSizeA1(C_Num,S,PS);
Char_Avail := (S >0) and (ST=0);
IF ST <> 0 then last_error(ST,'cHAR_aVAIL');
End;

Function all_get:char;
Var ST,S,PS:Word;
    C:Char;
Begin
Repeat
St:=__RDCHA1(C_NUM,C,S,PS);
Until ST=0;
All_Get:=C;
End;


Function Get_one:Char;
Var ST,S,PS:Word;
    C:Char;
Begin
St:=__RDCHA1(C_NUM,C,S,PS);
IF ST <> 0 then last_error(ST,'get_one');
Get_One:=C;
End;

Procedure Write_one(C:Char);
Var ST:Word;
Begin
ST:=__WrtChA1(C_num,C);
IF ST <> 0 then last_error(ST,'Write_One');
End;


Procedure Flush_buff;
Var Stat:word;
Begin
Stat:=__IFlshA1(C_num);
If stat = 3
 Then  {Port not open}
  Begin
   Stat:=__OpenA2(C_Num,Def_In_Queue,Def_Out_Queue,0,0);
   If STAT <> 0 then Last_error(Stat,'Flush buf');
   STAT:=__SETOPA2(C_Num,State);
   If STAT <> 0 then Last_error(Stat,'fLUSH bUFF 1');
  End;
If STAT <> 0 then Last_error(Stat,'Flush buf2');
End;

Procedure shut_down;
Var I:Word;
Begin
I:=__CLOSEA2(C_num);
End;


Procedure Write_Com(S:String);
Var
I,L:Word;
Begin
L:=Length(S);
If l>0 then
 For I:=1 to l do
  Begin
  write_one(S[I]);
  Write(S[i]);
  End
Else
  Begin
   Writeln('Sent zero chars');
   Halt;
  End;
End;

Procedure Write_Echo_Com(S:String);
Var
I,L:Word;
C:Char;
Begin
L:=Length(S);
If l>0 then
 For I:=1 to l do
  Begin
  Write(S[i]);
  While  __WrtChA1(C_num,S[i]) =7 do delay(1);
  End
Else
  Begin
   Writeln('Sent zero chars');
   Halt;
  End;
End;

Procedure Write_NoEcho_Com(S:String);
Var              
I,L:Word;
C:Char;
Begin
L:=Length(S);
If l>0 then
 For I:=1 to l do
  Begin
  While  __WrtChA1(C_num,S[i]) =7 do delay(1);
  End
Else
  Begin
   Writeln('Sent zero chars');
   Halt;
  End;
End;

Procedure Write_NoEchod_Com(S:String);
Var              
I,L:Word;
C:Char;
bv:byte;
Begin
L:=Length(S);
If l>0 then
 For I:=1 to l do
  Begin
  While  __WrtChA1(C_num,S[i]) =7 do delay(1);
  End
Else
  Begin
   Writeln('Sent zero chars');
   Halt;
  End;
End;



VAR Paddr:Word;
{This procedure initializes the text file in and out for COM i/O}
Procedure Init_Port(b:integer);

{Now to setup the default IO port paramaters}
Const             
          Com_Num           = 1;    {Select COM1}
          Baud_rate         = 2400;
          Parity            = _none;
          DataBits          = 8;
          StopBits          = 1;
          RemoteFlowControl = FALSE;
          LocalFlowControl  = FALSE;
          Bit7Trimming      = False;
          Bit7forcing       = False;
          RequireCTS        = False;
          Error_Msg : array [0..4] of string  =
            ('OK no errors',
             'Unknown error',
             'Invalid port number',
             'Port not open error',
             'Invalid parameter error');

Var Error:Word;          {Variable to hold error codes}
    baud,Stat:Word;
    I:Integer;
    Divisor:Word;
    Addr:Word;
Begin
C_NUM:=COM_NUM;
baud:=2400;
 For I:=0 to paramcount do
 Begin
If (PARAMSTR(I)='COM2') Then C_num:=2;
If (PARAMSTR(I)='38400') Then baud:=38400;
If (PARAMSTR(I)='19200') Then baud:=19200;
If (PARAMSTR(I)='9600') Then baud:=9600;
If (PARAMSTR(I)='2400') Then baud:=2400;
End;
if (C_NUM=2) then ADDR:=$2F8
   else ADDR:=$3F8;
if b <>0 then baud:=b;
Paddr:=addr;
Baud_Set:=baud;
 With State do
  Begin
if (baud>19200) then _Baudrate           := 19200
else _Baudrate           := Baud;
   _Parity              :=Parity;
   _DataBits            :=DataBits;
   _StopBits            :=StopBits;
   _RemoteFlowControl   :=RemoteFlowControl;
   _LocalFlowControl    :=LocalFlowControl;
   _Bit7Trimming        :=Bit7Trimming;
   _Bit7forcing         :=Bit7forcing;
   _RequireCTS          :=RequireCTS;
  End;

Stat:=__OpenA2(C_Num,Def_In_Queue,Def_Out_Queue,0,0);
If STAT <> 0 then Last_error(Stat,'Init 1');
STAT:=__SETOPA2(C_Num,State);
If STAT <> 0 then Last_error(Stat,'Init 1');

IF Baud > 19200 then
Begin
 Divisor:=round(1843200/(16.0*Baud));
 port[addr+3]:=$80; {Set Dlab }
 port[addr]:=(divisor and $FF);
 port[addr+2]:= ((divisor div 256) and $ff);
 port[addr+3]:=$03; {Set Dlab }

End;
Baud_Set:=baud;

End;{Init port}

Procedure Show_Ques;
Var ST,S,PS:Word;
    C:Char;
Begin
ST:=__IQSizeA1(C_Num,S,PS);
Writeln('In queue=',S);
ST:=__OQSizeA1(C_Num,S);
Writeln('In queue=',S);
REadln;
End;

Procedure Get_record(Var F:text; FC:String);                                
Var c:Char;
    line:word;
    x,y:Integer;
Begin                      
line:=0;
x:=wherex;
y:=wherey;
Write_echo_com(FC+chr(13));
Repeat
C:=all_get;
write(F,c);
If (c=chr(13)) then
   Begin
    inc(line);
    IF (line mod 20)=0 then
     Begin
      gotoxy(x,y);
      write('Line #',line,'       ');
     End;
   End;

Until (C='g');
End;

var lastaddr:longint;

procedure Init_download;
Begin
lastaddr:=0;
Writeln('Init download');
Write_ECHO_com('GO 8E280'+chr(13));
End;


procedure Init_downloadx;
Begin
lastaddr:=0;
Writeln('Init download');
Write_noEcho_com('xxxxxx'+chr(13));
delay(1000);
while(char_avail) do c:=get_one;
Write_noECHO_com('LOADANDPRAY'+chr(13));
delay(1000);
End;




function hexval(c2:char; c1:char):byte;
   var v:byte;
      begin
    if (c2>'9') then
       Begin
        v:=ord(upcase(c2))-ord('A')+10;
       End
       else v:=ord(c2)-ord('0');
      v:=v*16;
    if (c1>'9') then
       Begin
        v:=v+ord(upcase(c1))-ord('A')+10;
       End
       else v:=v+ord(c1)-ord('0');

hexval:=v;

   End;



procedure download_line(s:string);
   var addr :longint;
       cnt  :integer;
         v  :byte;
         i  :integer;
         bv :byte;
       stmp :string[20];
       code:integer;
   Begin
if ((S[1]='S') and (S[2]='3')) then
   Begin
   repeat
    stmp:='$'+S[3]+S[4];
    val(stmp,cnt,code);
     cnt:=cnt-5;  
    stmp:='$'+copy(s,5,8);
    val(stmp,addr,code);
    if ((lastaddr+1)=addr)
     then
      Begin
       stmp:=chr(cnt or 64);
       Write_NoEchod_Com(stmp);
      End
    else
    if ((lastaddr)<>addr)
     then 
     Begin {New Address}
     stmp:=chr(cnt+128);
     for i:=3 to 6 do
      stmp:=stmp+chr(hexval(s[i*2-1],s[i*2]));
     Write_NoEchod_Com(stmp);
     End {New Address}
     else
     Begin{Inc Address}
     stmp:=chr(cnt);
     Write_NoEchod_Com(stmp);
     End; {Inc Address}

    lastaddr:=cnt+addr;
    stmp:='';
    for i:=7 to 6+cnt do
      Begin
       stmp[i-6]:=chr(hexval(S[i*2-1],S[i*2]));
      End;
       Stmp[0]:=chr(cnt);
       Write_NoEchod_com(stmp);
       inc(i);
       bv:=hexval(S[i*2-1],S[i*2]);
       {Get result into v}
       while(not(char_avail)) do;
       v:=ord(get_one);
      while (char_avail)
       do write('!',get_one,'!');
    if (v<>bv)
    then      
      Begin
       Writeln('Csum Error Was ',v,' ',byte_to_hex(v),' SB ',bv,' ',byte_to_hex(bv));
       Writeln('S=',s);
       readln;
      End;
   until (v=bv);
   End   
   else
   if ((S[1]='S') and (S[2]='7')) then
      Begin
       s:=copy(s,5,8);
      End;
End;

procedure set_baud(b:word);
Var divisor:Word;
Begin
 Divisor:=round(1843200/(16.0*b));
 port[paddr+3]:=$80; {Set Dlab }
 port[paddr]:=(divisor and $FF);
 port[paddr+2]:= ((divisor div 256) and $ff);
 port[paddr+3]:=$03; {Set Dlab }
End;

procedure do_update;
   Var
    FC,FN:string;
    F:text;
    LC,XP,YP:Integer;
    v :byte;
     old_baud:Word;
    
Begin
FN:='UPDATE.ABS';
 For I:=1 to paramcount do
Begin
 If (PARAMSTR(I)<>'COM2') 
 and  (PARAMSTR(I)<>'COM1') 
 and  (PARAMSTR(I)<>'38400') 
 and  (PARAMSTR(I)<>'19200') 
 and  (PARAMSTR(I)<>'9600') 
 and  (PARAMSTR(I)<>'2400') 
 Then 
    Begin
     FN:=paramstr(i);
   End;
End;

IF pos('.',FN)=0 then FN:=FN+'.ABS';
 IF fileexists(fn) then
 Begin
  clrscr;
  XP:=wherex;
  YP:=wherey;
  Init_Downloadx;
  Delay(500);
  While(char_avail) do c:=get_one;
  Old_Baud:=Baud_Set;
  set_baud(19200);
  Writeln;
  LC:=0;
  Assign(F,'RAMDL.ABS');
  Reset(F);
   While not(eof(F)) do
    Begin
     REadln(F,FC);
     IF FC[1]='S' then
    begin
     Download_line(FC);
    end;
       inc(lc);
     IF (LC mod 10)=0 then
    Begin
     Gotoxy(xp,yp);
     Write('Sent :',LC,' Lines');
    End;
   End;
  Close(F);
  Write_noecho_com(chr(254));
  Write_noecho_com(chr(0));
  Write_noecho_com(chr(0));
  Write_noecho_com(chr(48));{Hex 30 }
  Write_noecho_com(chr(0));
  delay(500);
  Writeln;
  Writeln('Erasing EEPROMS');
  repeat
   while(not(char_avail)) do;
   v:=ord(get_one);
  until (v=$AA);
  Write_noecho_com(chr(13));
  Writeln('EEPROMS Erased');
  Delay(1000);
  Writeln('About to download file:',fn);
  while(char_avail) do c:=get_one;
  lastaddr:=0;
  LC:=0;
  Assign(F,FN); 
  Reset(F);
   While not(eof(F)) do
    Begin
     REadln(F,FN);
     IF FN[1]='S' then
    begin
     Download_line(FN);
    end;
       inc(lc);
     IF (LC mod 10)=0 then
    Begin
     Gotoxy(xp,yp);
     Write('Sent :',LC,' Lines');
    End;
   End;
  Close(F);
  Write_noecho_com(chr(255));
  clrscr;
  Writeln('Download complete Turn the unit power off then back on.');
  delay(1000);
   While(char_avail) do write(get_one);
  set_baud(old_baud);
 End
 Else
  Begin                   
   Writeln;                                  
   Writeln(Fn,' Dosen''t exist.');
  End;                
End;           

function  Verify_Connection:Boolean;
   var s:string;
   Begin
    Write_NoEcho_Com(chr(13));
    delay(500);
    While(char_avail) do c:=get_one;
    Write_NoEcho_Com('*idn?'+chr(13));
    delay(1500);
    s:='';
    while(char_avail) do s:=s+get_one;
{    Writeln('Verify=',s);
    Readln;
}
    IF pos('Keytek',s)<> 0 then Verify_Connection:=True
    else
    IF pos('KeyTek',s)<> 0 then Verify_Connection:=True
    else Verify_Connection:=False;
   End;


Begin
clrscr;
  IF video_mode = 7 then            { Determine start of screen mem. addr. }
    ScreenPtr := ptr($B000,$0000)   { Monochrome mode screen memory address }
  Else
    ScreenPtr := ptr($B800,$0000);
org_color:=ScreenPtr^[1,1].attr;
TextColor(Yellow);
if (fileexists('Info.dat'))
then
   Begin{File Exists}
      assign(f,'Info.dat');
      reset(f);
      clrscr;
      lc:=1;
      Writeln;
      While (not(eof(f))) do
          Begin
           Readln(f,s);
           if lc=1 then
            Begin
             Writeln('    Customer:',s);
             Writeln;
             Writeln('   unit(s):   Type                            Serial Number' );
             Writeln('   ÕÍÍÍÍÍÍÍÍÍØÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍØÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¸');
             lc:=lc+4;
            End
           else
             Begin
              if (lc<>5) then
               Begin
                Writeln('   ÃÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´');
                lc:=lc+2;
               End
               else lc:=lc+1;
              Writeln('   ', s);
             End;
          End;
              Writeln('   ÀÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ');

      Writeln;
      Writeln('    Does this match your configuration? (Y/N)');
      Lc:=Lc+3;
      box(1,1,79,lc+1,red);
      while not(keypressed) do;
      repeat
       C:=Readkey;
       c:=upcase(c);
      until (c='Y') or (c='N');
      if (c='N') then Begin 
      clrscr;
      Writeln;                            
      Writeln('           If the configurations are not identical');
      Writeln;
      Writeln('           This upgrade may cause the unit to stop functioning.');
      Writeln;
      Writeln('           Thus the program cannot update the ECAT system  ');
      Writeln;
      Writeln('           at this time. Please contact Keytek Instrument CO.');
      Writeln;
      Writeln('           for further assistance. ');
      box(1,1,79,15,red);
      while not(keypressed) do;
      textcolor(org_color); clrscr; halt(0); 
     End;

   End;{File Exists}
   Error_cause:='None';
Init_port(0);
if not(Verify_Connection) then
    Begin
      clrscr;
      Writeln;                            
      Writeln('           The program was unable to establish communications with the');
      Writeln;
      Writeln('           ECAT system. Please check the communications connections ');
      Writeln;
      Writeln('           and Try again. ');
      Writeln;                            
      Writeln('           Please contact Keytek Instrument CO.');
      Writeln;
      Writeln('           for further assistance. ');
      box(1,1,79,15,red);
      while not(keypressed) do;
      Begin textcolor(org_color); clrscr; halt(0); End;
    End;
clrscr;
Writeln;
Writeln;
Writeln('                ******       WARNING   WARNING     ******');
Writeln;
Writeln('           You are about to download a new firmware revision to your ');
Writeln;
Writeln('           ECAT system. If the Power or comunications link fails during ');
Writeln;
Writeln('           this process, your ECAT unit will no longer be operational. ');
Writeln;
Writeln('           If this happens you will require new EEPROMS from KeyTek.   ');
Writeln;
Writeln('           The comunications link has been verified, and the update process');
Writeln;
Writeln('           will take 1 to 2 minutes');
Writeln;
Writeln('           Do you wish to continue (Y/N)?                 ');

box(1,1,79,18,red);
while not(keypressed) do;
repeat
   C:=Readkey;
   c:=upcase(c);
until (c='Y') or (c='N');
if (c='N') then Begin clrscr; textcolor(org_color); clrscr; halt(0); End;

textcolor(org_color);
clrscr;
flush_buff;
Do_Update;
Shut_Down;
End.










