   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.24  06/05/06            */
   /*                                                     */
   /*                 DEFTEMPLATE MODULE                  */
   /*******************************************************/

/*************************************************************/
/* Purpose: Defines basic deftemplate primitive functions    */
/*   such as allocating and deallocating, traversing, and    */
/*   finding deftemplate data structures.                    */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Donnell                                     */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                  (extensions to run command)              */
/*                                                           */
/* Revision History:                                         */
/*      6.23: Added support for templates maintaining their  */
/*            own list of facts.                             */
/*                                                           */
/*      6.24: Renamed BOOLEAN macro type to intBool.         */
/*                                                           */
/*            Corrected code to remove run-time program      */
/*            compiler warnings.                             */
/*                                                           */
/*************************************************************/

#define _TMPLTDEF_SOURCE_

#include "setup.h"

#if DEFTEMPLATE_CONSTRUCT

#include <stdio.h>
#define _STDIO_INCLUDED_

#include "memalloc.h"
#include "exprnops.h"
#include "cstrccom.h"
#include "network.h"
#include "tmpltpsr.h"
#include "tmpltbsc.h"
#include "tmpltutl.h"
#include "tmpltfun.h"
#include "router.h"
#include "modulpsr.h"
#include "modulutl.h"
#include "cstrnchk.h"
#include "envrnmnt.h"

#if BLOAD || BLOAD_ONLY || BLOAD_AND_BSAVE
#include "bload.h"
#include "tmpltbin.h"
#endif

#if CONSTRUCT_COMPILER && (! RUN_TIME)
#include "tmpltcmp.h"
#endif

#if FUZZY_DEFTEMPLATES  
#include "fuzzypsr.h"
#endif

#include "tmpltdef.h"

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   static void                   *AllocateModule(void *);
   static void                    ReturnModule(void *,void *);
   static void                    ReturnDeftemplate(void *,void *);
   static void                    InitializeDeftemplateModules(void *);
   static void                    DeallocateDeftemplateData(void *);
   static void                    DestroyDeftemplateAction(void *,struct constructHeader *,void *);
   static void                    DestroyDeftemplate(void *,void *);

/******************************************************************/
/* InitializeDeftemplates: Initializes the deftemplate construct. */
/******************************************************************/
globle void InitializeDeftemplates(
  void *theEnv)
  {
   globle struct entityRecord deftemplatePtrRecord = { "DEFTEMPLATE_PTR",
                                                           DEFTEMPLATE_PTR,1,0,0,
                                                           NULL,
                                                           NULL,NULL,
                                                           NULL,
                                                           NULL,
                                                           DecrementDeftemplateBusyCount,
                                                           IncrementDeftemplateBusyCount,
                                                           NULL,NULL,NULL,NULL };
   AllocateEnvironmentData(theEnv,DEFTEMPLATE_DATA,sizeof(struct deftemplateData),DeallocateDeftemplateData);

   memcpy(&DeftemplateData(theEnv)->DeftemplatePtrRecord,&deftemplatePtrRecord,sizeof(struct entityRecord));   

   InitializeFacts(theEnv);

   InitializeDeftemplateModules(theEnv);

   DeftemplateBasicCommands(theEnv);

   DeftemplateFunctions(theEnv);

   DeftemplateData(theEnv)->DeftemplateConstruct =
      AddConstruct(theEnv,"deftemplate","deftemplates",ParseDeftemplate,EnvFindDeftemplate,
                   GetConstructNamePointer,GetConstructPPForm,
                   GetConstructModuleItem,EnvGetNextDeftemplate,SetNextConstruct,
                   EnvIsDeftemplateDeletable,EnvUndeftemplate,ReturnDeftemplate);

   InstallPrimitive(theEnv,(ENTITY_RECORD_PTR) &DeftemplateData(theEnv)->DeftemplatePtrRecord,DEFTEMPLATE_PTR);
  }
  
/******************************************************/
/* DeallocateDeftemplateData: Deallocates environment */
/*    data for the deftemplate construct.             */
/******************************************************/
static void DeallocateDeftemplateData(
  void *theEnv)
  {
#if ! RUN_TIME
   struct deftemplateModule *theModuleItem;
   void *theModule;
#endif
#if BLOAD || BLOAD_AND_BSAVE
   if (Bloaded(theEnv)) return;
#endif

   DoForAllConstructs(theEnv,DestroyDeftemplateAction,DeftemplateData(theEnv)->DeftemplateModuleIndex,FALSE,NULL); 

#if ! RUN_TIME
   for (theModule = EnvGetNextDefmodule(theEnv,NULL);
        theModule != NULL;
        theModule = EnvGetNextDefmodule(theEnv,theModule))
     {
      theModuleItem = (struct deftemplateModule *)
                      GetModuleItem(theEnv,(struct defmodule *) theModule,
                                    DeftemplateData(theEnv)->DeftemplateModuleIndex);
      rtn_struct(theEnv,deftemplateModule,theModuleItem);
     }
#endif
  }
  
/*****************************************************/
/* DestroyDeftemplateAction: Action used to remove   */
/*   deftemplates as a result of DestroyEnvironment. */
/*****************************************************/
#if IBM_TBC
#pragma argsused
#endif
static void DestroyDeftemplateAction(
  void *theEnv,
  struct constructHeader *theConstruct,
  void *buffer)
  {
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(buffer)
#endif
   struct deftemplate *theDeftemplate = (struct deftemplate *) theConstruct;
   
   if (theDeftemplate == NULL) return;
   
   DestroyDeftemplate(theEnv,theDeftemplate);
  }


/*************************************************************/
/* InitializeDeftemplateModules: Initializes the deftemplate */
/*   construct for use with the defmodule construct.         */
/*************************************************************/
static void InitializeDeftemplateModules(
  void *theEnv)
  {
   DeftemplateData(theEnv)->DeftemplateModuleIndex = RegisterModuleItem(theEnv,"deftemplate",
                                    AllocateModule,
                                    ReturnModule,
#if BLOAD_AND_BSAVE || BLOAD || BLOAD_ONLY
                                    BloadDeftemplateModuleReference,
#else
                                    NULL,
#endif
#if CONSTRUCT_COMPILER && (! RUN_TIME)
                                    DeftemplateCModuleReference,
#else
                                    NULL,
#endif
                                    EnvFindDeftemplate);

#if (! BLOAD_ONLY) && (! RUN_TIME) && DEFMODULE_CONSTRUCT
   AddPortConstructItem(theEnv,"deftemplate",SYMBOL);
#endif
  }

/***************************************************/
/* AllocateModule: Allocates a deftemplate module. */
/***************************************************/
static void *AllocateModule(
  void *theEnv)
  {    
   return((void *) get_struct(theEnv,deftemplateModule)); 
  }

/*************************************************/
/* ReturnModule: Deallocates a deftemplate module. */
/*************************************************/
static void ReturnModule(
  void *theEnv,
  void *theItem)
  {   
   FreeConstructHeaderModule(theEnv,(struct defmoduleItemHeader *) theItem,DeftemplateData(theEnv)->DeftemplateConstruct);
   rtn_struct(theEnv,deftemplateModule,theItem);
  }

/****************************************************************/
/* GetDeftemplateModuleItem: Returns a pointer to the defmodule */
/*  item for the specified deftemplate or defmodule.            */
/****************************************************************/
globle struct deftemplateModule *GetDeftemplateModuleItem(
  void *theEnv,
  struct defmodule *theModule)
  {   
   return((struct deftemplateModule *) GetConstructModuleItemByIndex(theEnv,theModule,DeftemplateData(theEnv)->DeftemplateModuleIndex)); 
  }

/*****************************************************/
/* EnvFindDeftemplate: Searches for a deftemplate in */
/*   the list of deftemplates. Returns a pointer to  */
/*   the deftemplate if  found, otherwise NULL.      */
/*****************************************************/
globle void *EnvFindDeftemplate(
  void *theEnv,
  char *deftemplateName)
  {  
   return(FindNamedConstruct(theEnv,deftemplateName,DeftemplateData(theEnv)->DeftemplateConstruct)); 
  }

/***********************************************************************/
/* EnvGetNextDeftemplate: If passed a NULL pointer, returns the first  */
/*   deftemplate in the ListOfDeftemplates. Otherwise returns the next */
/*   deftemplate following the deftemplate passed as an argument.      */
/***********************************************************************/
globle void *EnvGetNextDeftemplate(
  void *theEnv,
  void *deftemplatePtr)
  {   
   return((void *) GetNextConstructItem(theEnv,(struct constructHeader *) deftemplatePtr,DeftemplateData(theEnv)->DeftemplateModuleIndex)); 
  }

/***********************************************************/
/* EnvIsDeftemplateDeletable: Returns TRUE if a particular */
/*   deftemplate can be deleted, otherwise returns FALSE.  */
/***********************************************************/
globle intBool EnvIsDeftemplateDeletable(
  void *theEnv,
  void *vTheDeftemplate)
  {
   struct deftemplate *theDeftemplate = (struct deftemplate *) vTheDeftemplate;

   if (! ConstructsDeletable(theEnv))
     { return FALSE; }

   if (theDeftemplate->busyCount > 0) return(FALSE);
   if (theDeftemplate->patternNetwork != NULL) return(FALSE);

   return(TRUE);
  }

/**************************************************************/
/* ReturnDeftemplate: Returns the data structures associated  */
/*   with a deftemplate construct to the pool of free memory. */
/**************************************************************/
static void ReturnDeftemplate(
  void *theEnv,
  void *vTheConstruct)
  {
#if (MAC_MCW || IBM_MCW) && (RUN_TIME || BLOAD_ONLY)
#pragma unused(theEnv,vTheConstruct)
#endif

#if (! BLOAD_ONLY) && (! RUN_TIME)
   struct deftemplate *theConstruct = (struct deftemplate *) vTheConstruct;
   struct templateSlot *slotPtr;

   if (theConstruct == NULL) return;

   /*====================================================================*/
   /* If a template is redefined, then we want to save its debug status. */
   /*====================================================================*/

#if DEBUGGING_FUNCTIONS
   DeftemplateData(theEnv)->DeletedTemplateDebugFlags = 0;
   if (theConstruct->watch) BitwiseSet(DeftemplateData(theEnv)->DeletedTemplateDebugFlags,0);
#endif

#if FUZZY_DEFTEMPLATES
   /*===========================================*/
   /* Free storage used by the fuzzy template.  */
   /*                                           */
   /* NOTE: no DeinstallDeftemplate - this is it*/
   /*===========================================*/

   if (theConstruct->fuzzyTemplate != NULL)
     {
       DeinstallFuzzyTemplate(theEnv,theConstruct->fuzzyTemplate);
       /* for a fuzzy deftemplate the call to RemoveConstraint below
          for its single slot will decrement the busyCount for the
          deftemplate (restrictionList has ptr to this deftemplate)
          so it should be incremented or the count will go negative.
       */
       if ( theConstruct->slotList->constraints != NULL)
          theConstruct->busyCount++;
      }
#endif

   /*===========================================*/
   /* Free storage used by the templates slots. */
   /*===========================================*/

   slotPtr = theConstruct->slotList;
   while (slotPtr != NULL)
     {
      DecrementSymbolCount(theEnv,slotPtr->slotName);
      RemoveHashedExpression(theEnv,slotPtr->defaultList);
      slotPtr->defaultList = NULL;
      RemoveConstraint(theEnv,slotPtr->constraints);
      slotPtr->constraints = NULL;
      slotPtr = slotPtr->next;
     }

   ReturnSlots(theEnv,theConstruct->slotList);

   /*==================================*/
   /* Free storage used by the header. */
   /*==================================*/

   DeinstallConstructHeader(theEnv,&theConstruct->header);

   rtn_struct(theEnv,deftemplate,theConstruct);
#endif
  }
  
/**************************************************************/
/* DestroyDeftemplate: Returns the data structures associated */
/*   with a deftemplate construct to the pool of free memory. */
/**************************************************************/
static void DestroyDeftemplate(
  void *theEnv,
  void *vTheConstruct)
  {
#if (MAC_MCW || IBM_MCW) && (RUN_TIME || BLOAD_ONLY)
#pragma unused(vTheConstruct)
#endif

   struct deftemplate *theConstruct = (struct deftemplate *) vTheConstruct;
#if (! BLOAD_ONLY) && (! RUN_TIME)
   struct templateSlot *slotPtr, *nextSlot;
#endif
   if (theConstruct == NULL) return;
  
#if (! BLOAD_ONLY) && (! RUN_TIME)
   slotPtr = theConstruct->slotList;

   while (slotPtr != NULL)
     {
      nextSlot = slotPtr->next;
      rtn_struct(theEnv,templateSlot,slotPtr);
      slotPtr = nextSlot;
     }

#if FUZZY_DEFTEMPLATES
   if (theConstruct->fuzzyTemplate != NULL)
     {
      struct primary_term *pt, *this_pt;
          
      pt = theConstruct->fuzzyTemplate->primary_term_list;
      while (pt != NULL)
        {
         this_pt = pt;
         pt = pt->next;
         rtn_struct(theEnv,primary_term,this_pt);
        }
      
      rtn_struct(theEnv,fuzzyLv,theConstruct->fuzzyTemplate);
     }
#endif
      
#endif

   DestroyFactPatternNetwork(theEnv,theConstruct->patternNetwork);
   
   /*==================================*/
   /* Free storage used by the header. */
   /*==================================*/

#if (! BLOAD_ONLY) && (! RUN_TIME)
   DeinstallConstructHeader(theEnv,&theConstruct->header);

   rtn_struct(theEnv,deftemplate,theConstruct);
#endif
  }
  
/***********************************************/
/* ReturnSlots: Returns the slot structures of */
/*   a deftemplate to free memory.             */
/***********************************************/
globle void ReturnSlots(
  void *theEnv,
  struct templateSlot *slotPtr)
  {
#if (MAC_MCW || IBM_MCW) && (RUN_TIME || BLOAD_ONLY)
#pragma unused(theEnv,slotPtr)
#endif

#if (! BLOAD_ONLY) && (! RUN_TIME)
   struct templateSlot *nextSlot;

   while (slotPtr != NULL)
     {
      nextSlot = slotPtr->next;
      ReturnExpression(theEnv,slotPtr->defaultList);
      RemoveConstraint(theEnv,slotPtr->constraints);
      rtn_struct(theEnv,templateSlot,slotPtr);
      slotPtr = nextSlot;
     }
#endif
  }

/*************************************************/
/* DecrementDeftemplateBusyCount: Decrements the */
/*   busy count of a deftemplate data structure. */
/*************************************************/
globle void DecrementDeftemplateBusyCount(
  void *theEnv,
  void *vTheTemplate)
  {
   struct deftemplate *theTemplate = (struct deftemplate *) vTheTemplate;

   if (! ConstructData(theEnv)->ClearInProgress) theTemplate->busyCount--;
  }

/*************************************************/
/* IncrementDeftemplateBusyCount: Increments the */
/*   busy count of a deftemplate data structure. */
/*************************************************/
#if IBM_TBC
#pragma argsused
#endif
globle void IncrementDeftemplateBusyCount(
  void *theEnv,
  void *vTheTemplate)
  {
   struct deftemplate *theTemplate = (struct deftemplate *) vTheTemplate;
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#endif

   theTemplate->busyCount++;
  }
  
/*******************************************************************/
/* EnvGetNextFactInTemplate: If passed a NULL pointer, returns the */
/*   first fact in the template's fact-list. Otherwise returns the */
/*   next template fact following the fact passed as an argument.  */
/*******************************************************************/
#if IBM_TBC
#pragma argsused
#endif
globle void *EnvGetNextFactInTemplate(
  void *theEnv,
  void *theTemplate,
  void *factPtr)
  {
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#endif
   if (factPtr == NULL)
     { return((void *) ((struct deftemplate *) theTemplate)->factList); }

   if (((struct fact *) factPtr)->garbage) return(NULL);

   return((void *) ((struct fact *) factPtr)->nextTemplateFact);
  }

#endif /* DEFTEMPLATE_CONSTRUCT */


