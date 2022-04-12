/* Sector read function from SSA */
int gsect;
 
rdsect(x) int x;
{ /* read sector x into VDP >0000 (256) */
/* not sure where the stack is */
gsect=x;
 
#asm
 LI R0,>01FF  *DRIVE 1 READ
 MOV R0,@>834C
 LI R0,>0000
 MOV R0,@>834E
*MOV @3(R14),@>8350
 MOV @GSECT,@>8350
 LI R0,>1FF0
 LI R1,PAB10
 LI R2,2
 BLWP @VMBW
 MOV R0,@>8356
 BLWP @DSRLNK
 DATA >A
#endasm
}
 
#asm
PAB10 DATA >0110
#endasm
 
 
