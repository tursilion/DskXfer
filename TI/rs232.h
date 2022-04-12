/* RS232 ROUTINES ADAPTED FROM FlipSide */
 
#asm
 REF DSRLNK,VMBW,VSBR,VMBR,VSBW,KSCAN
 REF VWTR
#endasm
 
#asm
RESET  EQU  31
LDIR   EQU  13
RBRL   EQU  21
RIENB  EQU  18
RTSON  EQU  16
XBRE   EQU  22
BAUD12 DATA 417    * BAUD RATE IS 500,000/BAUD
BAUD24 DATA 208
BAUDIN DATA 26     * 19200 baud
R12SAV BSS 2
UNIT   BSS 2
CNTRL  BYTE >83
       EVEN
#endasm
 
init12()
{ /* 8n1, 1200 baud */
#asm
  MOV @BAUD12,@UNIT
#endasm
 initmn();
}
 
init24()
{ /*8n1, 2400 baud */
#asm
  MOV @BAUD24,@UNIT
#endasm
 initmn();
}
 
initin()
{ /* 8n1, 500kbaud*/
#asm
  MOV @BAUDIN,@UNIT
#endasm
 initmn();
}
 
initmn()
{ /* initialize RS232 (baud code in UNIT) */
#asm
 MOV R12,@R12SAV
 LI R12,>002E     *CASSETTE CONTROL 2
 SBO 0            *(FOR DTR LINE)
 LI R12,>1340
 SBO  RESET
 SBZ  21
 SBZ  20
 SBZ  19
 SBZ  18          *INTERRUPT ENABLES
 SBZ  17          *NO BREAK
 SBZ  15          *TEST OFF
 LDCR @CNTRL,8
 SBZ  LDIR
 LDCR @UNIT,12
 MOV @R12SAV,R12
#endasm
}
 
rcvx()
{ /* recieve a char from rs */
  /* return ffff if no char */
#asm
       MOV R12,@R12SAV
       LI R12,>1340
       LI R8,>FFFF
       TB   RBRL
       JNE  NCHAR
       CLR R8
       STCR R8,8
       SBZ RIENB
       ANDI R8,>7F00
       SWPB R8
NCHAR  MOV @R12SAV,R12
#endasm
}
 
sndx(x) char x;
{ /* send character */
#asm
       MOV R12,@R12SAV
       LI R12,>1340
       MOVB @3(R14),R1
       SBO  RTSON
SND0   TB   XBRE
       JNE  SND0
       LDCR R1,8
       SBZ  RTSON
       MOV @R12SAV,R12
#endasm
}
 
delay(i) int i;
{
  while (i) { i++; i--; i--; }
}
 
clrrs()
{ /*clear all incoming characters from rs port*/
int f;
f=0;
while (f<5)
{ if (rcvx()!=0xffff) { f=0; while (rcvx()!=0xffff);}
  delay(200);
  f++;}
}
