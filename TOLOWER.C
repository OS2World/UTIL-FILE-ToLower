/* (c) Copyright by Tels 1997. All Rigths Reserved.

  HACKed together from two examples from the online
  help - DosFindFirst/DosFindNext & rename      

  [09/19/1997] Tels: Initial Version
  [02/10/2021] Restored OS/2-compatibility, modifications, fixes
  [02/10/2021] Still broken, though: processing of same file name twice

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
      printf("DosAcknowledgeSignalException error: return code = SYS%04u\n",rc);
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


int main(void)
{
APIRET rc=NO_ERROR;
EXCEPTIONREGISTRATIONRECORD RegRec={0};
FILEFINDBUF3 FindBuffer={0};      
HDIR hdirFindHandle=HDIR_SYSTEM;
ULONG ulFindCount=1,ulResultBufLen=sizeof(FILEFINDBUF3);

RegRec.ExceptionHandler=(ERR)EmergencyExit;
DosSetExceptionHandler(&RegRec);

printf("\nToLower v1.0 (c) by tels@pobox.com 1997.  All Rights Reserved.\n\n");

rc=DosFindFirst("*",&hdirFindHandle,FILE_NORMAL,&FindBuffer,ulResultBufLen,&ulFindCount,FIL_STANDARD);

if (rc!=NO_ERROR)
   {
   if (rc==ERROR_NO_MORE_FILES)
      {
      printf(" Warning: no file(s) found in current working directory.\n");
      return 0;
      }
   else
      printf("DosFindFirst error: return code = SYS%04u\n",rc);
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
      printf("DosFindNext error: return code = SYS%04u\n",rc);
      return 1;
      }
   }

DosFindClose(hdirFindHandle);    
DosUnsetExceptionHandler(&RegRec);

return NO_ERROR;
}

