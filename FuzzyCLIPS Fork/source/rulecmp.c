
   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.24  05/17/06            */
   /*                                                     */
   /*            DEFRULE CONSTRUCTS-TO-C MODULE           */
   /*******************************************************/

/*************************************************************/
/* Purpose: Implements the constructs-to-c feature for the   */
/*    defrule construct.                                     */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Brian L. Donnell                                     */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*      6.24: Removed DYNAMIC_SALIENCE and                   */
/*            LOGICAL_DEPENDENCIES compilation flags.        */
/*                                                           */
/*************************************************************/

#define _RULECMP_SOURCE_

#include "setup.h"

#if DEFRULE_CONSTRUCT && (! RUN_TIME) && CONSTRUCT_COMPILER

#include <stdio.h>
#define _STDIO_INCLUDED_
#include <string.h>

#include "envrnmnt.h"
#include "factbld.h"
#include "reteutil.h"

#if FUZZY_DEFTEMPLATES
#include "fuzzyval.h"
#include "prntutil.h"
#endif

#include "rulecmp.h"

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

   static int                     ConstructToCode(void *,char *,int,FILE *,int,int);
   static void                    JoinToCode(void *,FILE *,struct joinNode *,int,int);
   static void                    DefruleModuleToCode(void *,FILE *,struct defmodule *,int,int,int);
#if FUZZY_DEFTEMPLATES  
   static void                    DefruleToCode(void *,FILE *,struct defrule *,int,int,int,int,int);
   static void                    CloseDefruleFiles(void *,FILE *,FILE *,FILE *,FILE *,int);
#else
   static void                    DefruleToCode(void *,FILE *,struct defrule *,int,int,int);
   static void                    CloseDefruleFiles(void *,FILE *,FILE *,FILE *,int);
#endif
   static void                    BeforeDefrulesCode(void *);

/***********************************************************/
/* DefruleCompilerSetup: Initializes the defrule construct */
/*   for use with the constructs-to-c command.             */
/***********************************************************/
globle void DefruleCompilerSetup(
  void *theEnv)
  {
   DefruleData(theEnv)->DefruleCodeItem = AddCodeGeneratorItem(theEnv,"defrules",0,BeforeDefrulesCode,
#if FUZZY_DEFTEMPLATES  
   /* extra file required for Fuzzy Values -- to hold ptrs to fuzzy values in patterns */
                                          NULL,ConstructToCode,4);
#else
                                          NULL,ConstructToCode,3);
#endif
  }

/**************************************************************/
/* BeforeDefrulesCode: Assigns each defrule and join with a   */
/*   unique ID which will be used for pointer references when */
/*   the data structures are written to a file as C code      */
/**************************************************************/
static void BeforeDefrulesCode(
  void *theEnv)
  {
   long int moduleCount, ruleCount, joinCount;

   TagRuleNetwork(theEnv,&moduleCount,&ruleCount,&joinCount);
  }

/*********************************************************/
/* ConstructToCode: Produces defrule code for a run-time */
/*   module created using the constructs-to-c function.  */
/*********************************************************/
static int ConstructToCode(
  void *theEnv,
  char *fileName,
  int fileID,
  FILE *headerFP,
  int imageID,
  int maxIndices)
  {
   int fileCount = 1;
   struct defmodule *theModule;
   struct defrule *theDefrule;
   struct joinNode *theJoin;
   int joinArrayCount = 0, joinArrayVersion = 1;
   int moduleCount = 0, moduleArrayCount = 0, moduleArrayVersion = 1;
   int defruleArrayCount = 0, defruleArrayVersion = 1;
   FILE *joinFile = NULL, *moduleFile = NULL, *defruleFile = NULL;
#if FUZZY_DEFTEMPLATES  
   struct fzSlotLocator *thePatternFv;
   int patternFvArrayCount = 0, patternFvArrayVersion = 1;
   FILE *patternFvFile = NULL;
#endif

   /*==============================================*/
   /* Include the appropriate defrule header file. */
   /*==============================================*/

   fprintf(headerFP,"#include \"ruledef.h\"\n");

   /*=========================================================*/
   /* Loop through all the modules, all the defrules, and all */
   /* the join nodes writing their C code representation to   */
   /* the file as they are traversed.                         */
   /*=========================================================*/
   
#if FUZZY_DEFTEMPLATES  
   /*=========================================================*/
   /* ALSO write the patternFv arrays as required if Fuzzy    */
   /*=========================================================*/
#endif

   for (theModule = (struct defmodule *) EnvGetNextDefmodule(theEnv,NULL);
        theModule != NULL;
        theModule = (struct defmodule *) EnvGetNextDefmodule(theEnv,theModule))
     {
      /*=========================*/
      /* Set the current module. */
      /*=========================*/

      EnvSetCurrentModule(theEnv,(void *) theModule);

      /*==========================*/
      /* Save the defrule module. */
      /*==========================*/

      moduleFile = OpenFileIfNeeded(theEnv,moduleFile,fileName,fileID,imageID,&fileCount,
                                    moduleArrayVersion,headerFP,
                                    "struct defruleModule",ModulePrefix(DefruleData(theEnv)->DefruleCodeItem),
                                    FALSE,NULL);

      if (moduleFile == NULL)
        {
#if FUZZY_DEFTEMPLATES  
         CloseDefruleFiles(theEnv,moduleFile,defruleFile,joinFile,patternFvFile,maxIndices);
#else
         CloseDefruleFiles(theEnv,moduleFile,defruleFile,joinFile,maxIndices);
#endif
         return(0);
        }

      DefruleModuleToCode(theEnv,moduleFile,theModule,imageID,maxIndices,moduleCount);
      moduleFile = CloseFileIfNeeded(theEnv,moduleFile,&moduleArrayCount,&moduleArrayVersion,
                                     maxIndices,NULL,NULL);

      /*=========================================*/
      /* Loop through all of the defrules (and   */
      /* their disjuncts) in the current module. */
      /*=========================================*/

      theDefrule = (struct defrule *) EnvGetNextDefrule(theEnv,NULL);

      while (theDefrule != NULL)
        {
         /*===================================*/
         /* Save the defrule data structures. */
         /*===================================*/

         defruleFile = OpenFileIfNeeded(theEnv,defruleFile,fileName,fileID,imageID,&fileCount,
                                        defruleArrayVersion,headerFP,
                                        "struct defrule",ConstructPrefix(DefruleData(theEnv)->DefruleCodeItem),
                                        FALSE,NULL);
         if (defruleFile == NULL)
           {
#if FUZZY_DEFTEMPLATES 
            CloseDefruleFiles(theEnv,moduleFile,defruleFile,joinFile,patternFvFile,maxIndices);
#else
            CloseDefruleFiles(theEnv,moduleFile,defruleFile,joinFile,maxIndices);
#endif
            return(0);
           }

#if FUZZY_DEFTEMPLATES 
         DefruleToCode(theEnv,defruleFile,theDefrule,imageID,maxIndices,
                        moduleCount, patternFvArrayCount, patternFvArrayVersion);
#else
         DefruleToCode(theEnv,defruleFile,theDefrule,imageID,maxIndices,
                        moduleCount);
#endif
         defruleArrayCount++;
         defruleFile = CloseFileIfNeeded(theEnv,defruleFile,&defruleArrayCount,&defruleArrayVersion,
                                         maxIndices,NULL,NULL);
                                         
#if FUZZY_DEFTEMPLATES  
         /* write out the patternFv array of fzSlotLocator structs */
         thePatternFv = theDefrule->pattern_fv_arrayPtr;
         if (thePatternFv != NULL)
           {
             int i;
             int numFzSlots = theDefrule->numberOfFuzzySlots;

             patternFvFile = OpenFileIfNeeded(theEnv,patternFvFile,fileName,fileID,imageID,&fileCount,
                                      patternFvArrayVersion,headerFP,
                                      "struct fzSlotLocator",PatternFvPrefix(),FALSE,NULL);
             if (patternFvFile == NULL)
               {
                 CloseDefruleFiles(theEnv,moduleFile,defruleFile,joinFile,patternFvFile,maxIndices);
                 return(0);
                }

             for (i=0; i< numFzSlots; i++)
                {
                  fprintf(patternFvFile, "{%d, %d, ",
                          thePatternFv[i].patternNum, thePatternFv[i].slotNum);
                  PrintFuzzyValueReference(theEnv,patternFvFile,thePatternFv[i].fvhnPtr);

                  fprintf(patternFvFile, "}");

                  if (i != numFzSlots-1)
                     fprintf(patternFvFile, ",");
                }

             patternFvArrayCount += numFzSlots;
             patternFvFile = CloseFileIfNeeded(theEnv,patternFvFile,&patternFvArrayCount,&patternFvArrayVersion,
                                            maxIndices,NULL,NULL);
           }
#endif

         /*================================*/
         /* Save the join data structures. */
         /*================================*/

         for (theJoin = theDefrule->lastJoin;
              theJoin != NULL;
              theJoin = GetPreviousJoin(theJoin))
           {
            if (theJoin->marked)
              {
               joinFile = OpenFileIfNeeded(theEnv,joinFile,fileName,fileID,imageID,&fileCount,
                                        joinArrayVersion,headerFP,
                                       "struct joinNode",JoinPrefix(),FALSE,NULL);
               if (joinFile == NULL)
                 {
#if FUZZY_DEFTEMPLATES   
                  CloseDefruleFiles(theEnv,moduleFile,defruleFile,joinFile,patternFvFile,maxIndices);
#else
                  CloseDefruleFiles(theEnv,moduleFile,defruleFile,joinFile,maxIndices);
#endif
                  return(0);
                 }

               JoinToCode(theEnv,joinFile,theJoin,imageID,maxIndices);
               joinArrayCount++;
               joinFile = CloseFileIfNeeded(theEnv,joinFile,&joinArrayCount,&joinArrayVersion,
                                            maxIndices,NULL,NULL);
              }
           }

         /*==========================================*/
         /* Move on to the next disjunct or defrule. */
         /*==========================================*/

         if (theDefrule->disjunct != NULL) theDefrule = theDefrule->disjunct;
         else theDefrule = (struct defrule *) EnvGetNextDefrule(theEnv,theDefrule);
        }

      moduleCount++;
      moduleArrayCount++;
     }

#if FUZZY_DEFTEMPLATES  
   CloseDefruleFiles(theEnv,moduleFile,defruleFile,joinFile,patternFvFile,maxIndices);
#else
   CloseDefruleFiles(theEnv,moduleFile,defruleFile,joinFile,maxIndices);
#endif

   return(1);
  }

/********************************************************/
/* CloseDefruleFiles: Closes all of the C files created */
/*   for defrule. Called when an error occurs or when   */
/*   the defrules have all been written to the files.   */
/********************************************************/
static void CloseDefruleFiles(
#if ! FUZZY_DEFTEMPLATES
  void *theEnv,
  FILE *moduleFile,
  FILE *defruleFile,
  FILE *joinFile,
  int maxIndices)
#else
  void *theEnv,
  FILE *moduleFile,
  FILE *defruleFile,
  FILE *joinFile,
  FILE *patternFvFile,
  int maxIndices)
#endif
  {
   int count = maxIndices;
   int arrayVersion = 0;

   if (joinFile != NULL)
     {
      count = maxIndices;
      CloseFileIfNeeded(theEnv,joinFile,&count,&arrayVersion,maxIndices,NULL,NULL);
     }

   if (defruleFile != NULL)
     {
      count = maxIndices;
      CloseFileIfNeeded(theEnv,defruleFile,&count,&arrayVersion,maxIndices,NULL,NULL);
     }

   if (moduleFile != NULL)
     {
      count = maxIndices;
      CloseFileIfNeeded(theEnv,moduleFile,&count,&arrayVersion,maxIndices,NULL,NULL);
     }

#if FUZZY_DEFTEMPLATES 
   if (patternFvFile != NULL)
     {
      count = maxIndices;
      patternFvFile = CloseFileIfNeeded(theEnv,patternFvFile,&count,&arrayVersion,maxIndices,NULL,NULL);
     }
#endif
  }

/*********************************************************/
/* DefruleModuleToCode: Writes the C code representation */
/*   of a single defrule module to the specified file.   */
/*********************************************************/
#if IBM_TBC
#pragma argsused
#endif
static void DefruleModuleToCode(
  void *theEnv,
  FILE *theFile,
  struct defmodule *theModule,
  int imageID,
  int maxIndices,
  int moduleCount)
  {
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(moduleCount)
#endif

   fprintf(theFile,"{");

   ConstructModuleToCode(theEnv,theFile,theModule,imageID,maxIndices,
                                  DefruleData(theEnv)->DefruleModuleIndex,ConstructPrefix(DefruleData(theEnv)->DefruleCodeItem));

   fprintf(theFile,",NULL}");
  }

/**********************************************************/
/* DefruleToCode: Writes the C code representation of a   */
/*   single defrule data structure to the specified file. */
/**********************************************************/
static void DefruleToCode(
#if ! FUZZY_DEFTEMPLATES
  void *theEnv,
  FILE *theFile,
  struct defrule *theDefrule,
  int imageID,
  int maxIndices,
  int moduleCount)
#else
  void *theEnv,
  FILE *theFile,
  struct defrule *theDefrule,
  int imageID,
  int maxIndices,
  int moduleCount,
  int patternFvArrayCount,
  int patternFvArrayVersion)
#endif
  {
   /*==================*/
   /* Construct Header */
   /*==================*/

   fprintf(theFile,"{");

   ConstructHeaderToCode(theEnv,theFile,&theDefrule->header,imageID,maxIndices,
                                  moduleCount,ModulePrefix(DefruleData(theEnv)->DefruleCodeItem),
                                  ConstructPrefix(DefruleData(theEnv)->DefruleCodeItem));

   /*==========================*/
   /* Flags and Integer Values */
   /*==========================*/

   fprintf(theFile,",%d,%d,%d,%d,%d,%d,%d,%d,",
                   theDefrule->salience,theDefrule->localVarCnt,
                   theDefrule->complexity,theDefrule->afterBreakpoint,
                   theDefrule->watchActivation,theDefrule->watchFiring,
                   theDefrule->autoFocus,theDefrule->executing);

   /*==================*/
   /* Dynamic Salience */
   /*==================*/

      ExpressionToCode(theEnv,theFile,theDefrule->dynamicSalience);
      fprintf(theFile,",");

   /*=============*/
   /* RHS Actions */
   /*=============*/

   ExpressionToCode(theEnv,theFile,theDefrule->actions);
   fprintf(theFile,",");

   /*=========================*/
   /* Logical Dependency Join */
   /*=========================*/

   if (theDefrule->logicalJoin != NULL)
     {
      fprintf(theFile,"&%s%d_%ld[%ld],",JoinPrefix(),
                     imageID,(theDefrule->logicalJoin->bsaveID / maxIndices) + 1,
                             theDefrule->logicalJoin->bsaveID % maxIndices);
     }
   else
     { fprintf(theFile,"NULL,"); }

   /*===========*/
   /* Last Join */
   /*===========*/

   if (theDefrule->lastJoin != NULL)
     {
      fprintf(theFile,"&%s%d_%ld[%ld],",JoinPrefix(),
                     imageID,(theDefrule->lastJoin->bsaveID / maxIndices) + 1,
                             theDefrule->lastJoin->bsaveID % maxIndices);
     }
   else
     { fprintf(theFile,"NULL,"); }
     
#if FUZZY_DEFTEMPLATES 
   /*===============*/
   /* Next Disjunct */
   /*===============*/

   if (theDefrule->disjunct != NULL)
     {
      fprintf(theFile,"&%s%d_%ld[%ld],",ConstructPrefix(DefruleData(theEnv)->DefruleCodeItem),
                     imageID,(theDefrule->disjunct->header.bsaveID / maxIndices) + 1,
                             theDefrule->disjunct->header.bsaveID % maxIndices);
     }
   else
     { fprintf(theFile,"NULL,"); }

   /*====================================*/
   /* Static & Dynamic Certainty Factors */
   /*====================================*/

#if CERTAINTY_FACTORS  
   fprintf(theFile,"%s,", FloatToString(theEnv,theDefrule->CF));

   ExpressionToCode(theEnv,theFile,theDefrule->dynamicCF);
#endif

   /*====================================*/
   /* Fuzzy Value Stuff                  */
   /*                                    */
   /* Note: min_of_maxmins set to -1 by  */
   /*       default -- not meaninful till*/
   /*       rule fired                   */
   /*====================================*/

   fprintf(theFile,",-1.0,%d,%d,",
                    theDefrule->lhsRuleType,
                    theDefrule->numberOfFuzzySlots);

   if (theDefrule->numberOfFuzzySlots == 0)
      fprintf(theFile,"NULL}");
   else
      fprintf(theFile,"&%s%d_%d[%d]}", PatternFvPrefix(), imageID,
                                      patternFvArrayVersion, patternFvArrayCount);

#else  /* FUZZY_DEFTEMPLATES */

   /*===============*/
   /* Next Disjunct */
   /*===============*/

   if (theDefrule->disjunct != NULL)
     {
      fprintf(theFile,"&%s%d_%ld[%ld]}",ConstructPrefix(DefruleData(theEnv)->DefruleCodeItem),
                     imageID,(theDefrule->disjunct->header.bsaveID / maxIndices) + 1,
                             theDefrule->disjunct->header.bsaveID % maxIndices);
     }
   else
     { fprintf(theFile,"NULL}"); }

#endif  /* FUZZY_DEFTEMPLATES */
  }

/***************************************************/
/* JoinToCode: Writes the C code representation of */
/*   a single join node to the specified file.     */
/***************************************************/
static void JoinToCode(
  void *theEnv,
  FILE *theFile,
  struct joinNode *theJoin,
  int imageID,
  int maxIndices)
  {
   struct patternParser *theParser;

   /*===========================*/
   /* Mark the join as visited. */
   /*===========================*/

   theJoin->marked = 0;

   /*===========================*/
   /* Flags and Integer Values. */
   /*===========================*/

   fprintf(theFile,"{%d,%d,%d,%d,0,0,%d,%d,0,",
                   theJoin->firstJoin,theJoin->logicalJoin,
                   theJoin->joinFromTheRight,theJoin->patternIsNegated,
                   theJoin->rhsType,theJoin->depth);

   /*==============*/
   /* Beta Memory. */
   /*==============*/

   fprintf(theFile,"NULL,");

   /*====================*/
   /* Network Expression */
   /*====================*/

   PrintHashedExpressionReference(theEnv,theFile,theJoin->networkTest,imageID,maxIndices);
   fprintf(theFile,",");

   /*============================*/
   /* Right Side Entry Structure */
   /*============================*/

   if (theJoin->rightSideEntryStructure == NULL)
     { fprintf(theFile,"NULL,"); }
   else if (theJoin->joinFromTheRight == FALSE)
     {
      theParser = GetPatternParser(theEnv,(int) theJoin->rhsType);
      if (theParser->codeReferenceFunction == NULL) fprintf(theFile,"NULL,");
      else
        {
         fprintf(theFile,"VS ");
         (*theParser->codeReferenceFunction)(theEnv,theJoin->rightSideEntryStructure,
                                             theFile,imageID,maxIndices);
         fprintf(theFile,",");
        }
     }
   else
     {
      fprintf(theFile,"&%s%d_%ld[%ld],",JoinPrefix(),
              imageID,(((struct joinNode *) theJoin->rightSideEntryStructure)->bsaveID / maxIndices) + 1,
                      ((struct joinNode *) theJoin->rightSideEntryStructure)->bsaveID % maxIndices);
     }

   /*=================*/
   /* Next Join Level */
   /*=================*/

   if (theJoin->nextLevel == NULL)
     { fprintf(theFile,"NULL,"); }
   else
     {
      fprintf(theFile,"&%s%d_%ld[%ld],",JoinPrefix(),
                    imageID,(theJoin->nextLevel->bsaveID / maxIndices) + 1,
                            theJoin->nextLevel->bsaveID % maxIndices);
     }

   /*=================*/
   /* Last Join Level */
   /*=================*/

   if (theJoin->lastLevel == NULL)
     { fprintf(theFile,"NULL,"); }
   else
     {
      fprintf(theFile,"&%s%d_%ld[%ld],",JoinPrefix(),
                    imageID,(theJoin->lastLevel->bsaveID / maxIndices) + 1,
                            theJoin->lastLevel->bsaveID % maxIndices);
     }

   /*==================*/
   /* Right Drive Node */
   /*==================*/

   if (theJoin->rightDriveNode == NULL)
     { fprintf(theFile,"NULL,"); }
   else
     {
      fprintf(theFile,"&%s%d_%ld[%ld],",JoinPrefix(),
                    imageID,(theJoin->rightDriveNode->bsaveID / maxIndices) + 1,
                            theJoin->rightDriveNode->bsaveID % maxIndices);
     }

   /*==================*/
   /* Right Match Node */
   /*==================*/

   if (theJoin->rightMatchNode == NULL)
     { fprintf(theFile,"NULL,"); }
   else
     {
      fprintf(theFile,"&%s%d_%ld[%ld],",JoinPrefix(),
                    imageID,(theJoin->rightMatchNode->bsaveID / maxIndices) + 1,
                            theJoin->rightMatchNode->bsaveID % maxIndices);
     }

   /*==================*/
   /* Rule to Activate */
   /*==================*/

   if (theJoin->ruleToActivate == NULL)
     { fprintf(theFile,"NULL}"); }
   else
     {
      fprintf(theFile,"&%s%d_%ld[%ld]}",ConstructPrefix(DefruleData(theEnv)->DefruleCodeItem),imageID,
                                    (theJoin->ruleToActivate->header.bsaveID / maxIndices) + 1,
                                    theJoin->ruleToActivate->header.bsaveID % maxIndices);
     }
  }

/*************************************************************/
/* DefruleCModuleReference: Writes the C code representation */
/*   of a reference to a defrule module data structure.      */
/*************************************************************/
globle void DefruleCModuleReference(
  void *theEnv,
  FILE *theFile,
  int count,
  int imageID,
  int maxIndices)
  {
   fprintf(theFile,"MIHS &%s%d_%d[%d]",ModulePrefix(DefruleData(theEnv)->DefruleCodeItem),
                      imageID,
                      (count / maxIndices) + 1,
                      (count % maxIndices));
  }

#endif /* DEFRULE_CONSTRUCT && (! RUN_TIME) && CONSTRUCT_COMPILER */


