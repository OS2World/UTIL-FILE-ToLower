/* (c) Copyright by Tels 1997. All Rigths Reserved.

  HACKed together from two examples from the online 
  help - DosFindFirst/DosFindNext & rename	

  [1997-09-19] Tels: Initial Version
  [2020-02-02] Version 1.1 compiled by Martin Iturbide

*/

  #define INCL_DOSFILEMGR   /* File Manager values */
  #define INCL_DOSERRORS    /* DOS error values */
  #include <os2.h>
  #include <stdio.h>
  #include <string.h>
  #include <ctype.h>

 int convert(char* filename)
 {
    char NewName[256];
    char dummyname [13] = "$$$$$$$$.TMP";
    int i,j;

    printf (" %s ",filename);
    j = strlen(filename);
    strcpy (NewName, filename);
    for (i=0; i<j; i++)
       {
       NewName[i] = tolower (NewName[i]);
       }
    if (strcmp (filename, NewName ) != 0)
       {
       if (rename(filename, dummyname) != 0)
          {
          printf(" Error: Could not rename file to $$$$$$$$.TMP. Skipped.");
          }
       else
          if (rename(dummyname, NewName) != 0)
             printf(" Error: Could not rename $$$$$$$$.TMP to file. Skipped.");
          else
             printf(" File is renamed to \"%s\".", NewName);
       }
    else
       {
       printf (" Warning: File already in lowercase. Skipped.");
       }
   printf ("\n");
   return 0;
 }


int main (VOID) {
     HDIR          hdirFindHandle = HDIR_SYSTEM;
     FILEFINDBUF3  FindBuffer     = {0};      /* Returned from FindFirst/Next */
     ULONG         ulResultBufLen = sizeof(FILEFINDBUF3);
     ULONG         ulFindCount    = 1;        /* Look for 1 file at a time    */
     APIRET        rc             = NO_ERROR; /* Return code                  */

    printf ("ToLower  v1.1 \n\n");

     rc = DosFindFirst( "*.*",                /* File pattern - all files     */
                        &hdirFindHandle,      /* Directory search handle      */
                        FILE_NORMAL,          /* Search attribute             */
                        &FindBuffer,          /* Result buffer                */
                        ulResultBufLen,       /* Result buffer length         */
                        &ulFindCount,         /* Number of entries to find    */
                        FIL_STANDARD);        /* Return level 1 file info     */

     if (rc != NO_ERROR) {
        printf("DosFindFirst error: return code = %u\n",rc);
        return 1;
     } else {
        convert (FindBuffer.achName);
//        printf ("%s\n", FindBuffer.achName);   /* Print file name             */

     } /* endif */

     /* Keep finding the next file until there are no more files */
     while (rc != ERROR_NO_MORE_FILES) {
        ulFindCount = 1;                      /* Reset find count.            */

        rc = DosFindNext(hdirFindHandle,      /* Directory handle             */
                         &FindBuffer,         /* Result buffer                */
                         ulResultBufLen,      /* Result buffer length         */
                         &ulFindCount);       /* Number of entries to find    */

        if (rc != NO_ERROR && rc != ERROR_NO_MORE_FILES) {
           printf("DosFindNext error: return code = %u\n",rc);
           return 1;
        } else {
           convert (FindBuffer.achName);
     //      printf ("%s\n", FindBuffer.achName);    /* Print file name */

        }
     } /* endwhile */

     rc = DosFindClose(hdirFindHandle);    /* Close our directory handle */
     if (rc != NO_ERROR) {
        printf("DosFindClose error: return code = %u\n",rc);
        return 1;
     }
     return NO_ERROR;
     printf ("\nAll done. Have a nice day.\n");
  }