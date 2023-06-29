/* (c) Copyright by Tels 1997. All Rigths Reserved.

  HACKed together from two examples from the online
  help - DosFindFirst/DosFindNext & rename      

  [09/19/1997] Tels: Initial Version
  [02/10/2021] Restored OS/2-compatibility, modifications, fixes
  [02/10/2021] Still broken, though: processing of same file name twice
  [02/14/2021] Drag & drop support (new argument: directory or file name)

*/


#define INCL_DOSERRORS
#define INCL_DOSEXCEPTIONS
#define INCL_DOSFILEMGR

#include <ctype.h>
#include <os2.h>
#include <stdio.h>
#include <string.h>


BOOL continued=TRUE;
char dummyname[13]="$$$$$$$$.TMP",NewName[CCHMAXPATH];
size_t i,j;


void convert(char* filename)
{
printf(" %-16s ",filename);
j=strlen(filename);
strcpy(NewName,filename);
for (i=0;i<j;i++)
   NewName[i]=tolower(NewName[i]);
if (strcmp(filename,NewName)!=0)
   {
   if (rename(filename,dummyname)!=0)
      printf(" Error: Could not rename file to \"$$$$$$$$.TMP\". Skipped.");
   else if (rename(dummyname,NewName)!=0)
      printf(" Error: Could not rename \"$$$$$$$$.TMP\" to file. Skipped.");
   else
      printf(" File is renamed to \"%s\".",NewName);
   }
else
   printf(" Warning: File already in lowercase. Skipped.");
printf("\n");
}


ULONG _System EmergencyExit(PEXCEPTIONREPORTRECORD p1,PEXCEPTIONREGISTRATIONRECORD p2,PCONTEXTRECORD p3,PVOID pv)
{
APIRET rc=NO_ERROR;
if (p1->ExceptionNum==XCPT_SIGNAL)
   {
   rc=DosAcknowledgeSignalException(XCPT_SIGNAL_BREAK);
   if (rc!=NO_ERROR)
      {
      printf("DosAcknowledgeSignalException error: return code = SYS%04lu\n",rc);
      return 1;
      }
   else
      {
      continued=TRUE;
      return XCPT_CONTINUE_EXECUTION;
      }
   }
return XCPT_CONTINUE_SEARCH;
}


int main(int argc, char *argv[])
{
APIRET rc=NO_ERROR;
char filespec[CCHMAXPATH*4]="*\0";
EXCEPTIONREGISTRATIONRECORD RegRec={0};
FILEFINDBUF3 FindBuffer={0};      
HDIR hdirFindHandle=HDIR_SYSTEM;
ULONG ulFindCount=1,ulResultBufLen=sizeof(FILEFINDBUF3);

RegRec.ExceptionHandler=(ERR)EmergencyExit;
DosSetExceptionHandler(&RegRec);

printf("\nToLower v1.1. \n\n");

if (argc>2)
   {
   printf("Error: invalid argument(s), usage: TOLOWER.EXE [dir|drive|file]\n");
   return 1;
   }

if (argc==2 && strlen(argv[1]))
   {
   if (argv[1][0]=='/')
      {
      printf("Usage: TOLOWER.EXE [dir|drive|file]\n");
      return 1;
      }
   strcpy(filespec,argv[1]);
   if (!DosSetCurrentDir((PCSZ) argv[1]))
      {
      if (argv[1][strlen(argv[1])-1]!='\\')
         strcat(filespec,"\\\0");
      strcat(filespec,"*\0");
      }
   if (strlen(filespec)>=CCHMAXPATH)      
      {
      printf("Error: invalid argument(s), full file specification is too long\n%ld",rc);
      return 1;
      }
   }   

rc=DosFindFirst((PCSZ) filespec,&hdirFindHandle,FILE_NORMAL,&FindBuffer,ulResultBufLen,&ulFindCount,FIL_STANDARD);

if (rc!=NO_ERROR)
   {
   if (rc==ERROR_NO_MORE_FILES)
      {
      printf(" Warning: no file(s) found in current working directory.\n");
      return 0;
      }
   else
      printf("DosFindFirst error: return code = SYS%04lu\n",rc);
   return 1;
   }

/* Keep finding the next file until there are no more files */
while (rc!=ERROR_NO_MORE_FILES)
   {
   if (!continued)
      {
      DosUnsetExceptionHandler(&RegRec);
      return 0;
      }

   convert(FindBuffer.achName);

   ulFindCount=1;
   
   rc=DosFindNext(hdirFindHandle,&FindBuffer,ulResultBufLen,&ulFindCount);

   if (rc!=NO_ERROR && rc!=ERROR_NO_MORE_FILES)
      {
      printf("DosFindNext error: return code = SYS%04lu\n",rc);
      return 1;
      }
   }

DosFindClose(hdirFindHandle);    
DosUnsetExceptionHandler(&RegRec);

return NO_ERROR;
}

