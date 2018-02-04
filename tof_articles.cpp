/* ******************************************************************** **
** @@ tof_articles
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : All necessary include files (tci.h) and import library (tci.lib)
** @  Notes  : are installed with TCI SDK package into corresponding subdirectories.
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>

#include "tci.h"
#include "tberror.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef NDEBUG
#pragma optimize("gsy",on)
#pragma comment(linker,"/FILEALIGN:512 /MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR /IGNORE:4078")
#endif

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

#define TD_DB_NAME            "TECDOC_CD_3_2015@localhost:2024"
#define TD_TB_LOGIN           "tecdoc"
#define TD_TB_PASSWORD        "tcd_error_0"

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

extern DWORD      dwKeepError = 0;

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */
                           
static TCIEnvironment*        pEnv  = NULL;
static TCIError*              pErr  = NULL;
static TCIConnection*         pConn = NULL;
static TCIStatement*          pStmt = NULL;
static TCIResultSet*          pRes  = NULL;
static TCITransaction*        pTa   = NULL;

static const char*   _pszTable = "tof_articles.csv";

static FILE*         _pOut = NULL;

/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ AllocationError()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void AllocationError(char* what)
{
   printf("Can't allocate %s\n",what);

   if (pEnv)
   {
      TCIFreeEnvironment(pEnv);
   }

   exit(1);
}

/* ******************************************************************** **
** @@ Init()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void Init()
{
   if (TCIAllocEnvironment(&pEnv))
   {
      AllocationError("environment handle");
   }

   if (TCIAllocError(pEnv,&pErr))
   {
      AllocationError("error handle");
   }

   if (TCIAllocTransaction(pEnv,pErr,&pTa))
   {
      AllocationError("transaction handle");
   }

   if (TCIAllocConnection(pEnv,pErr,&pConn))
   {
      AllocationError("connection handle");
   }

   if (TCIAllocStatement(pConn,pErr,&pStmt))
   {
      AllocationError("statement handle");
   }

   if (TCIAllocResultSet(pStmt,pErr,&pRes))
   {
      AllocationError("resultset handle");
   }
}

/* ******************************************************************** **
** @@ Cleanup()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void Cleanup()
{
   TCIFreeResultSet(pRes);
   TCIFreeStatement(pStmt);
   TCIFreeConnection(pConn);
   TCIFreeTransaction(pTa);
   TCIFreeError(pErr);
   TCIFreeEnvironment(pEnv);
}

/* ******************************************************************** **
** @@ TBEerror()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static TCIState TBEerror(TCIState rc)
{
   if (rc && (TCI_NO_DATA_FOUND != rc))
   {
      TCIState    erc = TCI_SUCCESS;

      char     errmsg[1024];
      char     sqlcode[6];

      Error    tberr = E_NO_ERROR;

      sqlcode[5] = 0;

      erc = TCIGetError(pErr,1,1,errmsg,sizeof(errmsg),&tberr,sqlcode);

      if (erc)
      {
         erc = TCIGetEnvironmentError(pEnv,1,errmsg,sizeof(errmsg),&tberr,sqlcode);

         if (erc)
         {
            // Error !
            ASSERT(0);
            printf("Can't get error info for error %d (reason: error %d)\n",rc,erc);
            exit(rc);
         }
      }

      // Error !
      ASSERT(0);
      printf("Transbase Error %d (SQLCode %s):\n%s\n",tberr,sqlcode,errmsg);

      exit(rc);
   }

   return rc;
}

/* ******************************************************************** **
** @@ ShowHelp()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

static void ShowHelp()
{
   const char pszCopyright[] = "-*-   tof_articles 1.0  *   Copyright (c) Gazlan, 2015   -*-";
   const char pszDescript [] = "TECDOC_CD_3_2015 DB TOF_ARTICLES dumper";
   const char pszE_Mail   [] = "complains_n_suggestions direct to gazlan@yandex.ru";

   printf("%s\n\n",pszCopyright);
   printf("%s\n\n",pszDescript);
   printf("Usage: tof_articles.com\n\n");
   printf("%s\n",pszE_Mail);
}

/* ******************************************************************** **
** @@ main()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

int main(int argc,char** argv)
{           
   if (argc > 1)
   {
      ShowHelp();
      return 0;
   }

   if (argc == 2)
   {
      if ((!strcmp(argv[1],"?")) || (!strcmp(argv[1],"/?")) || (!strcmp(argv[1],"-?")) || (!stricmp(argv[1],"/h")) || (!stricmp(argv[1],"-h")))
      {
         ShowHelp();
         return 0;
      }
   }

   struct TOF_ARTICLES
   {
      Int4           _iART_ID;               // INTEGER
      char*          _pszART_ARTICLE_NR;     // VARCHAR(66)
      Int2           _iART_SUP_ID;           // SMALLINT
      Int4           _iART_DES_ID;           // INTEGER
      Int4           _iART_COMPLETE_DES_ID;  // INTEGER
      Bits           _biART_CTM;             //  ÂITS(*)
      Int2           _iART_PACK_SELFSERVICE; // SMALLINT
      Int2           _iART_MATERIAL_MARK;    // SMALLINT
      Int2           _iART_REPLACEMENT;      // SMALLINT
      Int2           _iART_ACCESSORY;        // SMALLINT
      Int4           _iART_BATCH_SIZE1;      // INTEGER
      Int4           _iART_BATCH_SIZE2;      // INTEGER
   };
   
   _pOut = fopen(_pszTable,"wt");

   if (!_pOut)
   {
      // Error !
      ASSERT(0);
      printf("Err: Can't open [%s] for write.\n",_pszTable);
      return 0;
   }

   TOF_ARTICLES    tofArticles;

   memset(&tofArticles,0,sizeof(TOF_ARTICLES));

   Init();

   TBEerror(TCIConnect(pConn,TD_DB_NAME));
   TBEerror(TCILogin(pConn,TD_TB_LOGIN,TD_TB_PASSWORD));
   
   TBEerror(TCIExecuteDirectA(pRes,"SELECT * FROM TOF_ARTICLES ORDER BY ART_ID",1,0));

   int      iRow  = 0;

   Int2     Indicator = 0;

   TCIState    Err;

   // 1. ART_ID
   TBEerror(TCIBindColumnA(pRes,1,&tofArticles._iART_ID,sizeof(Int4),NULL,TCI_C_SINT4,&Indicator));
   // 2. ART_ARTICLE_NR
   TBEerror(TCIBindColumnA(pRes,2,&tofArticles._pszART_ARTICLE_NR,sizeof(Int4),NULL,TCI_C_SCHARPTR,&Indicator));
   // 3. ART_SUP_ID
   TBEerror(TCIBindColumnA(pRes,3,&tofArticles._iART_SUP_ID,sizeof(Int2),NULL,TCI_C_SINT2,&Indicator));
   // 4. ART_DES_ID
   TBEerror(TCIBindColumnA(pRes,4,&tofArticles._iART_DES_ID,sizeof(Int4),NULL,TCI_C_SINT4,&Indicator));
   // 5. ART_COMPLETE_DES_ID
   TBEerror(TCIBindColumnA(pRes,5,&tofArticles._iART_COMPLETE_DES_ID,sizeof(Int4),NULL,TCI_C_SINT4,&Indicator));
   // 6. iART_CTM
   TBEerror(TCIBindColumnA(pRes,6,&tofArticles._biART_CTM,sizeof(Bits),NULL,TCI_C_TBBITSLONG,&Indicator));
   // 7. ART_PACK_SELFSERVICE
   TBEerror(TCIBindColumnA(pRes,7,&tofArticles._iART_PACK_SELFSERVICE,sizeof(Int2),NULL,TCI_C_SINT2,&Indicator));
   // 8. ART_MATERIAL_MARK
   TBEerror(TCIBindColumnA(pRes,8,&tofArticles._iART_MATERIAL_MARK,sizeof(Int2),NULL,TCI_C_SINT2,&Indicator));
   // 9. ART_REPLACEMENT
   TBEerror(TCIBindColumnA(pRes,9,&tofArticles._iART_REPLACEMENT,sizeof(Int2),NULL,TCI_C_SINT2,&Indicator));
   // 10. ART_ACCESSORY
   TBEerror(TCIBindColumnA(pRes,10,&tofArticles._iART_ACCESSORY,sizeof(Int2),NULL,TCI_C_SINT2,&Indicator));
   // 11. ART_BATCH_SIZE1
   TBEerror(TCIBindColumnA(pRes,11,&tofArticles._iART_BATCH_SIZE1,sizeof(Int4),NULL,TCI_C_SINT4,&Indicator));
   // 12. ART_BATCH_SIZE2
   TBEerror(TCIBindColumnA(pRes,12,&tofArticles._iART_BATCH_SIZE2,sizeof(Int4),NULL,TCI_C_SINT4,&Indicator));

   fprintf(_pOut,"\"##\", \"ART_ID\", \"ART_ARTICLE_NR\", \"ART_SUP_ID\", \"ART_DES_ID\", \"ART_COMPLETE_DES_ID\", \"ART_CTM\", \"ART_PACK_SELFSERVICE\", \"ART_MATERIAL_MARK\", \"ART_REPLACEMENT\", \"ART_ACCESSORY\", \"ART_BATCH_SIZE1\", \"ART_BATCH_SIZE2\"\n");

   while ((Err = TCIFetchA(pRes,1,TCI_FETCH_NEXT,0)) == TCI_SUCCESS) 
   { 
      fprintf(_pOut,"%d",++iRow);

      // 1. ART_ID
      fprintf(_pOut,", %ld",tofArticles._iART_ID);
      // 2. ART_ARTICLE_NR
      fprintf(_pOut,", \"%s\"",tofArticles._pszART_ARTICLE_NR);
      // 3. ART_SUP_ID
      fprintf(_pOut,", %d",tofArticles._iART_SUP_ID);
      // 4. ART_DES_ID
      fprintf(_pOut,", %ld",tofArticles._iART_DES_ID);
      // 5. ART_COMPLETE_DES_ID
      fprintf(_pOut,", %ld",tofArticles._iART_COMPLETE_DES_ID);

      // 6. ART_CTM
      int   iBytes = BITS_TO_BYTE(tofArticles._biART_CTM.length);

      BYTE*    pArr = tofArticles._biART_CTM.bits;

      fprintf(_pOut,", ",tofArticles._biART_CTM);

      for (int ii = 0; ii < iBytes; ++ii)
      {
         fprintf(_pOut,"%02X",pArr[ii]);
      }

      // 7. ART_PACK_SELFSERVICE
      fprintf(_pOut,", %d",tofArticles._iART_PACK_SELFSERVICE);
      // 8. ART_MATERIAL_MARK
      fprintf(_pOut,", %d",tofArticles._iART_MATERIAL_MARK);
      // 9. ART_REPLACEMENT
      fprintf(_pOut,", %d",tofArticles._iART_REPLACEMENT);
      // 10. ART_ACCESSORY 
      fprintf(_pOut,", %d",tofArticles._iART_ACCESSORY);
      
      // 11. ART_BATCH_SIZE1
      fprintf(_pOut,", %ld",tofArticles._iART_BATCH_SIZE1);
      // 12. ART_BATCH_SIZE2
      fprintf(_pOut,", %ld",tofArticles._iART_BATCH_SIZE2);

      fprintf(_pOut,"\n");
   } 

   if (Err != TCI_NO_DATA_FOUND) 
   {
     TBEerror(Err); 
   }
   
   TBEerror(TCICloseA(pRes));

   TBEerror(TCICloseA(pRes));
   TBEerror(TCILogout(pConn));
   TBEerror(TCIDisconnect(pConn));

   Cleanup();

   fclose(_pOut);
   _pOut = NULL;

   return 0;
}
