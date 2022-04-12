/* BASIC SERIAL DISK TRANSFER */
/* COMMUNICATION:             */
/* >HELLO: <DISK NAME>\n      */
/* -sends HELLP: if full disk */
/* <OK\n                      */
/* >SIZE: <size in sectors>\n */
/* <READY\n                   */
/* >Disk sector, 256 bytes +  */
/*  1 byte checksum           */
/* <Y if sector good, N if bad*/
/*  (Y or N has the \n after) */
 
#include "DSK1.STDIO"
#include "DSK1.RS232/H"
#include "DSK1.SECTOR/H"
 
char sec0[256];
char *pBmp;
int  BmpMask;
char buf[257];
char str[64];
 
waitkey() {
  puts("Press any key to continue.\n");
  while (!poll(0)) {
#asm
       LIMI 2
       LIMI 0
#endasm
  }
}
 
getmode() {
  int c;
  int no_h;
 
  no_h=0;
  puts("Press 1 for bitmap mode\n");
  puts("Press 2 for full disk mode\n");
  puts("Press H for help\n");
  puts("Press QUIT to exit\n");
  for (;;) {
    while (!(c=poll(0))) {
#asm
       LIMI 2
       LIMI 0
#endasm
    }
    switch (c) {
      case '1': return 1;
      case '2': return 2;
      case 'H':
        if (no_h == 1) break;
        no_h=1;
        puts("\nBitmap mode will copy a disk\n");
        puts(" copying only the sectors marked\n");
        puts(" as used in the disk bitmap\n");
        puts("\nFull disk mode will copy all\n");
        puts(" sectors on the disk whether\n");
        puts(" marked as in-use or not. This\n");
        puts(" will take longer but may be\n");
        puts(" needed for some disks.\n");
        puts("\nIn either case, the disk must\n");
        puts(" be in drive DSK1. It should work\n");
        puts(" with any size disk but is only\n");
        puts(" tested with the TI controller.\n");
        puts("\nThe serial link is 19.2k at 8N1\n");
        puts(" and has been tested on a stock\n");
        puts(" console with the Corcomp RS232/PIO\n");
        puts(" card in the PEB, but should work\n");
        puts(" with the TI card.\n");
        break;
    }
  }
}
 
send(str) char *str;
{ while (*str) {
    sndx(*str);
    str++;
  }
  sndx('\n');
}
 
int add;
int val;
 
recv() {
  char *p;
  int x;
 
  x=0xffff;
  p=str;
  while (x != '\n') {
    x=0xffff;
    while (x==0xffff) {
#asm
       LIMI 2
       LIMI 0
#endasm
      x=rcvx();
      if (poll(0)=='~') {
        puts("\n** Aborted **\n");
        str[0]='F';
        str[1]=0;
        return;
      }
    }
    *p=x;
    add=p-str;
    val=x<<8;
#asm
       MOV @ADD,R0
       MOV @VAL,R1
       BLWP @VSBW
#endasm
    p++;
  }
  p--;
  *p='\0';
}
 
readsect(x) int x;
{
  char c;
  int i;
 
  rdsect(x);
  /* copy the VDP buffer into buf */
  /* while we're at it, checksum it */
#asm
       LI R0,>0000
       LI R1,BUF
       LI R2,256
       BLWP @VMBR
#endasm
  c=0;
  for (i=0; i<256; i++) {
    c=c+buf[i];
  }
  buf[256]=c;
}
 
strcpy(d,s) char *d,*s;
{
  while (*s) {
    *(d++)=*(s++);
  }
  *d='\0';
}
 
strlen(s) char *s;
{
  int l;
 
  l=0;
  while (*(s++)) l++;
 
  return l;
}
 
strcmp(s,t) char *s,*t;
{
  for (;;) {
    if (*s > *t) return 1;
    if (*t > *s) return -1;
    if (*s == 0) return 0;
    s++; t++;
  }
}
 
putnum(s, i) char *s; int i;
{
  int n;
 
  n=i;
  *(s++)=n/10000+48;
  n=n%10000;
  *(s++)=n/1000+48;
  n=n%1000;
  *(s++)=n/100+48;
  n=n%100;
  *(s++)=n/10+48;
  *(s++)=n%10+48;
  *s=0;
}
 
int nsect,tmpout;
 
main() {
  int i,j;
  int mode;
 
  /* drop below the disc buffer */
  puts("\n\n\n\n\n\n\n\n\n\n");
  initin();
  puts("Here we go! Please insert a disk\n");
  puts("While waiting for response, press\n");
  puts("~ to abort transfer.\n");
mylp:
  mode=getmode();
  clrrs();
  readsect(0);
  /* store it for the bitmap */
  for (i=0; i<256; i++) {
    sec0[i]=buf[i];
  }
  pBmp=&sec0[0x38];
  BmpMask=0x01;
  /* get the disk name into str */
  strcpy(str, "HELLO: ");
  if (mode == 2) str[4]='P';
  i=strlen(str);
  for (j=0; j<10; j++) {
    if (buf[j]==' ') break;
    str[i++]=buf[j];
  }
  str[i]='\0';
  puts(str);
  puts("\n");
  send(str);
  recv();  /* wait for OK */
  if (strcmp(str,"OK")) {
    puts("Failed to get OK from PC\n");
    goto mylp;
  }
  /* get the number of sectors */
  strcpy(str, "SIZE: ");
  i=strlen(str);
  j=buf[10]*256+buf[11];
  nsect=j;
  putnum(&str[i], j);
  puts(str);
  puts("\n");
  send(str);
  recv();  /* wait for READY */
  if (strcmp(str,"READY")) {
    puts("Failed to get READY from PC\n");
    goto mylp;
  }
  for (i=0; i<nsect; i++) {
    /* check if we want this sector */
    if ((mode != 2) &&
      (!((*pBmp)&BmpMask))) goto skipit;
    strcpy(str,"\rReading...");
    putnum(&str[strlen(str)],i);
    puts(str);
    readsect(i);
    strcpy(str,"\rSending...");
    putnum(&str[strlen(str)],i);
    puts(str);
    for (j=0; j<257; j++) {
      /* send 256 bytes plus checksum */
       tmpout=j;
#asm
       MOV @TMPOUT,R0
       LI R1,>2A00
       BLWP @VSBW
#endasm
      sndx(buf[j]);
    }
    strcpy(str,"\rWaiting...");
    putnum(&str[strlen(str)],i);
    puts(str);
    recv();  /* wait for Y or N */
    if (str[0] != 'Y') {
      if (str[0] == 'F') {
        puts("\rFailed on PC side\n");
        goto mylp;
      }
      puts("\rDidn't get an ack, retrying.\n");
      i--;
    } else {
      puts("\rReceived ok!");
    }
skipit:
    /* next mask */
    BmpMask=BmpMask<<1;
    if (BmpMask>255) {
      pBmp++;
      BmpMask=0x01;
    }
  }
 
  puts("\nDisk transfer complete!\n");
 
  goto mylp;
}
 
