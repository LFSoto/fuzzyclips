   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.24  05/17/06            */
   /*                                                     */
   /*                 RULE PARSING MODULE                 */
   /*******************************************************/

/*************************************************************/
/* Purpose:  Parses a defrule construct.                     */
/*                                                           */
/* Principal Programmer(s):                                  */
/*      Gary D. Riley                                        */
/*                                                           */
/* Contributing Programmer(s):                               */
/*      Bob Orchard (NRCC - Nat'l Research Council of Canada)*/
/*                  (Fuzzy reasoning extensions)             */
/*                  (certainty factors for facts and rules)  */
/*                  (extensions to run command)              */
/*                                                           */
/* Revision History:                                         */
/*                                                           */
/*      6.24: Removed DYNAMIC_SALIENCE, INCREMENTAL_RESET,   */
/*            and LOGICAL_DEPENDENCIES compilation flags.    */
/*                                                           */
/*************************************************************/

#define _RULEPSR_SOURCE_

#include "setup.h"

#if DEFRULE_CONSTRUCT

#include <stdio.h>
#define _STDIO_INCLUDED_
#include <string.h>

#include "analysis.h"
#include "constant.h"
#include "constrct.h"
#include "cstrcpsr.h"
#include "cstrnchk.h"
#include "cstrnops.h"
#include "engine.h"
#include "envrnmnt.h"
#include "exprnpsr.h"
#include "incrrset.h"
#include "memalloc.h"
#include "pattern.h"
#include "prccode.h"
#include "prcdrpsr.h"
#include "router.h"
#include "rulebld.h"
#include "rulebsc.h"
#include "rulecstr.h"
#include "ruledef.h"
#include "ruledlt.h"
#include "rulelhs.h"
#include "scanner.h"
#include "symbol.h"
#include "watch.h"

#include "lgcldpnd.h"

#if DEFTEMPLATE_CONSTRUCT
#include "tmpltfun.h"
#endif

#if BLOAD || BLOAD_ONLY || BLOAD_AND_BSAVE
#include "bload.h"
#endif

#include "rulepsr.h"

/***************************************/
/* LOCAL INTERNAL FUNCTION DEFINITIONS */
/***************************************/

#if (! RUN_TIME) && (! BLOAD_ONLY)
   static struct expr            *ParseRuleRHS(void *,char *);
   static int                     ReplaceRHSVariable(void *,struct expr *,void *);
   static struct defrule         *ProcessRuleLHS(void *,struct lhsParseNode *,struct expr *,SYMBOL_HN *,short *);
#if FUZZY_DEFTEMPLATES  
   static struct defrule         *CreateNewDisjunct(void *,SYMBOL_HN *,int,struct expr *,
                                                    int,unsigned,struct joinNode *,int);
#else
   static struct defrule         *CreateNewDisjunct(void *,SYMBOL_HN *,int,struct expr *,
                                                    int,unsigned,struct joinNode *);
#endif
   static int                     RuleComplexity(void *,struct lhsParseNode *);
   static int                     ExpressionComplexity(void *,struct expr *);
   static int                     LogicalAnalysis(void *,struct lhsParseNode *);
   static void                    AddToDefruleList(struct defrule *);
#endif

/****************************************************/
/* ParseDefrule: Coordinates all actions necessary  */
/*   for the parsing and creation of a defrule into */
/*   the current environment.                       */
/****************************************************/
globle int ParseDefrule(
  void *theEnv,
  char *readSource)
  {
#if (MAC_MCW || IBM_MCW) && (RUN_TIME || BLOAD_ONLY)
#pragma unused(theEnv,readSource)
#endif

#if (! RUN_TIME) && (! BLOAD_ONLY)
   SYMBOL_HN *ruleName;
   struct lhsParseNode *theLHS;
   struct expr *actions;
   struct token theToken;
   struct defrule *topDisjunct, *tempPtr;
   struct defruleModule *theModuleItem;
   short error;

   /*================================================*/
   /* Flush the buffer which stores the pretty print */
   /* representation for a rule.  Add the already    */
   /* parsed keyword defrule to this buffer.         */
   /*================================================*/

   SetPPBufferStatus(theEnv,ON);
   FlushPPBuffer(theEnv);
   SavePPBuffer(theEnv,"(defrule ");

   /*=========================================================*/
   /* Rules cannot be loaded when a binary load is in effect. */
   /*=========================================================*/

#if BLOAD || BLOAD_ONLY || BLOAD_AND_BSAVE
   if ((Bloaded(theEnv) == TRUE) && (! ConstructData(theEnv)->CheckSyntaxMode))
     {
      CannotLoadWithBloadMessage(theEnv,"defrule");
      return(TRUE);
     }
#endif

   /*================================================*/
   /* Parse the name and comment fields of the rule, */
   /* deleting the rule if it already exists.        */
   /*================================================*/

#if DEBUGGING_FUNCTIONS
   DefruleData(theEnv)->DeletedRuleDebugFlags = 0;
#endif

   ruleName = GetConstructNameAndComment(theEnv,readSource,&theToken,"defrule",
                                         EnvFindDefrule,EnvUndefrule,"*",FALSE,
                                         TRUE,TRUE);

   if (ruleName == NULL) return(TRUE);

   /*============================*/
   /* Parse the LHS of the rule. */
   /*============================*/

   theLHS = ParseRuleLHS(theEnv,readSource,&theToken,ValueToString(ruleName));
   if (theLHS == NULL)
     {
      ReturnPackedExpression(theEnv,PatternData(theEnv)->SalienceExpression);
      PatternData(theEnv)->SalienceExpression = NULL;
#if CERTAINTY_FACTORS  
      ReturnPackedExpression(theEnv,PatternData(theEnv)->CFExpression);
      PatternData(theEnv)->CFExpression = NULL;
#endif
      return(TRUE);
     }

   /*============================*/
   /* Parse the RHS of the rule. */
   /*============================*/

   ClearParsedBindNames(theEnv);
   ExpressionData(theEnv)->ReturnContext = TRUE;
   actions = ParseRuleRHS(theEnv,readSource);

   if (actions == NULL)
     {
      ReturnPackedExpression(theEnv,PatternData(theEnv)->SalienceExpression);
      PatternData(theEnv)->SalienceExpression = NULL;
#if CERTAINTY_FACTORS  
      ReturnPackedExpression(theEnv,PatternData(theEnv)->CFExpression);
      PatternData(theEnv)->CFExpression = NULL;
#endif
      ReturnLHSParseNodes(theEnv,theLHS);
      return(TRUE);
     }

   /*=======================*/
   /* Process the rule LHS. */
   /*=======================*/

   topDisjunct = ProcessRuleLHS(theEnv,theLHS,actions,ruleName,&error);

   ReturnExpression(theEnv,actions);
   ClearParsedBindNames(theEnv);
   ReturnLHSParseNodes(theEnv,theLHS);

   if (error)
     {
      ReturnPackedExpression(theEnv,PatternData(theEnv)->SalienceExpression);
      PatternData(theEnv)->SalienceExpression = NULL;
#if CERTAINTY_FACTORS 
      ReturnPackedExpression(theEnv,PatternData(theEnv)->CFExpression);
      PatternData(theEnv)->CFExpression = NULL;
#endif
      return(TRUE);
     }

   /*==============================================*/
   /* If we're only checking syntax, don't add the */
   /* successfully parsed defrule to the KB.       */
   /*==============================================*/

   if (ConstructData(theEnv)->CheckSyntaxMode)
     {
      ReturnPackedExpression(theEnv,PatternData(theEnv)->SalienceExpression);
      PatternData(theEnv)->SalienceExpression = NULL;
#if CERTAINTY_FACTORS  
      ReturnPackedExpression(theEnv,PatternData(theEnv)->CFExpression);
      PatternData(theEnv)->CFExpression = NULL;
#endif
      return(FALSE);
     }

   PatternData(theEnv)->SalienceExpression = NULL;
#if CERTAINTY_FACTORS 
   PatternData(theEnv)->CFExpression = NULL;
#endif

   /*======================================*/
   /* Save the nice printout of the rules. */
   /*======================================*/

   SavePPBuffer(theEnv,"\n");
   if (EnvGetConserveMemory(theEnv) == TRUE)
     { topDisjunct->header.ppForm = NULL; }
   else
     { topDisjunct->header.ppForm = CopyPPBuffer(theEnv); }

   /*=======================================*/
   /* Store a pointer to the rule's module. */
   /*=======================================*/

   theModuleItem = (struct defruleModule *)
                   GetModuleItem(theEnv,NULL,FindModuleItem(theEnv,"defrule")->moduleIndex);

   for (tempPtr = topDisjunct; tempPtr != NULL; tempPtr = tempPtr->disjunct)
     { tempPtr->header.whichModule = (struct defmoduleItemHeader *) theModuleItem; }

   /*===============================================*/
   /* Rule completely parsed. Add to list of rules. */
   /*===============================================*/

   AddToDefruleList(topDisjunct);

   /*========================================================================*/
   /* If a rule is redefined, then we want to restore its breakpoint status. */
   /*========================================================================*/

#if DEBUGGING_FUNCTIONS
   if (BitwiseTest(DefruleData(theEnv)->DeletedRuleDebugFlags,0))
     { EnvSetBreak(theEnv,topDisjunct); }
   if (BitwiseTest(DefruleData(theEnv)->DeletedRuleDebugFlags,1) || EnvGetWatchItem(theEnv,"activations"))
     { EnvSetDefruleWatchActivations(theEnv,ON,(void *) topDisjunct); }
   if (BitwiseTest(DefruleData(theEnv)->DeletedRuleDebugFlags,2) || EnvGetWatchItem(theEnv,"rules"))
     { EnvSetDefruleWatchFirings(theEnv,ON,(void *) topDisjunct); }
#endif

   /*================================*/
   /* Perform the incremental reset. */
   /*================================*/

   IncrementalReset(theEnv,topDisjunct);

   /*=============================================*/
   /* Return FALSE to indicate no errors occured. */
   /*=============================================*/

#endif
   return(FALSE);
  }

#if (! RUN_TIME) && (! BLOAD_ONLY)

#if FUZZY_DEFTEMPLATES  

/**************************************************************/
/* findFuzzyValueInNetworktest:                               */
/*                                                            */
/* Given a ptr to a networkTest of a pattern (ptr to expr),   */
/* try to find a fuzzy value test (SCALL_PN_FUZZY_VALUE) and  */
/* extract ptr to the fuzzy value associated with the test -- */
/* fuzzy value found as the only arg of SCALL_PN_FUZZY_VALUE -*/
/* return null if none found                                  */
/**************************************************************/

static FUZZY_VALUE_HN *findFuzzyValueInNetworktest(
  struct expr *netTestPtr)
{
  FUZZY_VALUE_HN *temp;

  if (netTestPtr == NULL)
     return( NULL );

  if (netTestPtr->type == SCALL_PN_FUZZY_VALUE)
     return( (FUZZY_VALUE_HN *)netTestPtr->argList->value );

  temp = findFuzzyValueInNetworktest(netTestPtr->argList);

  if (temp != NULL)
    return( temp );

  return( findFuzzyValueInNetworktest(netTestPtr->nextArg) );

}

#endif

/**************************************************************/
/* ProcessRuleLHS: Processes each of the disjuncts of a rule. */
/**************************************************************/
static struct defrule *ProcessRuleLHS(
  void *theEnv,
  struct lhsParseNode *theLHS,
  struct expr *actions,
  SYMBOL_HN *ruleName,
  short *error)
  {
   struct lhsParseNode *tempNode = NULL;
   struct defrule *topDisjunct = NULL, *currentDisjunct, *lastDisjunct = NULL;
   struct expr *newActions, *packPtr;
   int logicalJoin;
   int localVarCnt;
   int complexity;
   struct joinNode *lastJoin;

   /*================================================*/
   /* Initially set the parsing error flag to FALSE. */
   /*================================================*/

   *error = FALSE;

   /*===========================================================*/
   /* The top level of the construct representing the LHS of a  */
   /* rule is assumed to be an OR.  If the implied OR is at the */
   /* top level of the pattern construct, then remove it.       */
   /*===========================================================*/

   if (theLHS->type == OR_CE) theLHS = theLHS->right;

   /*=========================================*/
   /* Loop through each disjunct of the rule. */
   /*=========================================*/

   localVarCnt = CountParsedBindNames(theEnv);
   for (;
        theLHS != NULL;
        theLHS = theLHS->bottom)
     {
#if FUZZY_DEFTEMPLATES  
       unsigned int LHSRuleType; /* LHSRuleType is set to FUZZY_LHS or CRISP_LHS */
       unsigned int numFuzzySlots;
       int patternNum;
#endif

      /*===================================*/
      /* Analyze the LHS of this disjunct. */
      /*===================================*/

      if (theLHS->type == AND_CE) tempNode = theLHS->right;
      else if (theLHS->type == PATTERN_CE) tempNode = theLHS;

      if (VariableAnalysis(theEnv,tempNode))
        {
         *error = TRUE;
         ReturnDefrule(theEnv,topDisjunct);
         return(NULL);
        }

      /*=========================================*/
      /* Perform entity dependent post analysis. */
      /*=========================================*/

      if (PostPatternAnalysis(theEnv,tempNode))
        {
         *error = TRUE;
         ReturnDefrule(theEnv,topDisjunct);
         return(NULL);
        }
        
#if FUZZY_DEFTEMPLATES  
      /* calculate the number of fuzzy value slots in patterns with
         the rule disjunct and also determine LHS type of the
         rule -- if number of fuzzy slots in non-NOT patterns >0
         then FUZZY LHS [if pattern is (not (xxx (fuzzySlot ...) ...))
         then they don't count when considering the LHS type]
      */
      if (theLHS->type == AND_CE) tempNode = theLHS->right;
      else if (theLHS->type == PATTERN_CE) tempNode = theLHS;

      {
         int numFuzzySlotsInNonNotPatterns = 0;
         numFuzzySlots = FuzzySlotAnalysis(theEnv,tempNode, &numFuzzySlotsInNonNotPatterns);
         if (numFuzzySlotsInNonNotPatterns > 0)
           LHSRuleType = FUZZY_LHS;
         else
           LHSRuleType = CRISP_LHS;
      }

#endif

      /*========================================================*/
      /* Print out developer information if it's being watched. */
      /*========================================================*/

#if DEVELOPER && DEBUGGING_FUNCTIONS
/*
      if (EnvGetWatchItem(theEnv,"rule-analysis"))
        {
         struct lhsParseNode *traceNode;
         char buffer[20];

         EnvPrintRouter(theEnv,WDISPLAY,"\n");
         for (traceNode = tempNode; traceNode != NULL; traceNode = traceNode->bottom)
           {
            if (traceNode->userCE)
              {
               sprintf(buffer,"CE %2d: ",traceNode->whichCE);
               EnvPrintRouter(theEnv,WDISPLAY,buffer);
               PrintExpression(theEnv,WDISPLAY,traceNode->networkTest);
               EnvPrintRouter(theEnv,WDISPLAY,"\n");
              }
           }
        }
*/
#endif

      /*========================================*/
      /* Check to see that logical CEs are used */
      /* appropriately in the LHS of the rule.  */
      /*========================================*/

      if ((logicalJoin = LogicalAnalysis(theEnv,tempNode)) < 0)
        {
         *error = TRUE;
         ReturnDefrule(theEnv,topDisjunct);
         return(NULL);
        }

      /*======================================================*/
      /* Check to see if there are any RHS constraint errors. */
      /*======================================================*/

      if (CheckRHSForConstraintErrors(theEnv,actions,tempNode))
        {
         *error = TRUE;
         ReturnDefrule(theEnv,topDisjunct);
         return(NULL);
        }

      /*=================================================*/
      /* Replace variable references in the RHS with the */
      /* appropriate variable retrieval functions.       */
      /*=================================================*/

      newActions = CopyExpression(theEnv,actions);
      if (ReplaceProcVars(theEnv,"RHS of defrule",newActions,NULL,NULL,
                          ReplaceRHSVariable,(void *) tempNode))
        {
         *error = TRUE;
         ReturnDefrule(theEnv,topDisjunct);
         ReturnExpression(theEnv,newActions);
         return(NULL);
        }

      /*==================================*/
      /* We're finished for this disjunct */
      /* if we're only checking syntax.   */
      /*==================================*/

      if (ConstructData(theEnv)->CheckSyntaxMode)
        {
         ReturnExpression(theEnv,newActions);
         continue;
        }

      /*=================================*/
      /* Install the disjunct's actions. */
      /*=================================*/

      ExpressionInstall(theEnv,newActions);
      packPtr = PackExpression(theEnv,newActions);
      ReturnExpression(theEnv,newActions);

      /*===============================================================*/
      /* Create the pattern and join data structures for the new rule. */
      /*===============================================================*/

      lastJoin = ConstructJoins(theEnv,logicalJoin,tempNode);

      /*===================================================================*/
      /* Determine the rule's complexity for use with conflict resolution. */
      /*===================================================================*/

      complexity = RuleComplexity(theEnv,tempNode);

      /*=====================================================*/
      /* Create the defrule data structure for this disjunct */
      /* and put it in the list of disjuncts for this rule.  */
      /*=====================================================*/

#if FUZZY_DEFTEMPLATES  
      /* if not a FUZZY LHS then no need to allocate space to store ptrs to
         fuzzy slots of the patterns on the LHS since there won't be any
         -- numFuzzySlots will be 0.
      */

      currentDisjunct = CreateNewDisjunct(theEnv,ruleName,localVarCnt,packPtr,complexity,
                                          logicalJoin,lastJoin,numFuzzySlots);
      /* set the type of LHS of Rule disjunct and also
         save fuzzy slot locator info for any fuzzy patterns
      */

      currentDisjunct->lhsRuleType = LHSRuleType;

      if (numFuzzySlots > 0)
        {
         struct lhsParseNode *lhsPNPtr, *lhsSlotPtr;
         int i;

         /* get ptr to a pattern CE */
          if (theLHS->type == AND_CE) lhsPNPtr = theLHS->right;
          else if (theLHS->type == PATTERN_CE) lhsPNPtr = theLHS;
          else lhsPNPtr = NULL;

          patternNum = 0; /* indexed from 0 */
          i = 0;
          while (lhsPNPtr != NULL)
           {
             if (lhsPNPtr->type == PATTERN_CE)
               {
                 lhsSlotPtr = lhsPNPtr->right;
                 while (lhsSlotPtr != NULL)
                     {
                       /*==========================================================*/
                       /* If fuzzy template slot then save ptr to fuzzy value      */
                       /*                                                          */
                       /* NOTE: when the pattern was added to the pattern net, the */
                       /* pattern node for the template name was removed (see      */
                       /* PlaceFactPattern) and the pattern node of the FUZZY_VALUE*/
                       /* was changed a networkTest link in the SF_VARIABLE or     */
                       /* SF_WILDCARD node as an SCALL_PN_FUZZY_VALUE expression.  */
                       /* We need to search for that fuzzy value to put it in the  */
                       /* rule (pattern_fv_arrayPtr points to an array of          */
                       /* structures that holds the pattern number, slot number and*/
                       /* fuzzy value HN ptrs connected to the patterns of LHS).   */
                       /*==========================================================*/

                       if (lhsSlotPtr->type == SF_WILDCARD ||
                           lhsSlotPtr->type == SF_VARIABLE)
                         {
                           FUZZY_VALUE_HN *fv_ptr;
                           struct fzSlotLocator * fzSL_ptr;

                           fv_ptr = findFuzzyValueInNetworktest(lhsSlotPtr->networkTest);

                           if (fv_ptr != NULL)
                             {
                               fzSL_ptr = currentDisjunct->pattern_fv_arrayPtr + i;
                               fzSL_ptr->patternNum = patternNum;
                               fzSL_ptr->slotNum = (lhsSlotPtr->slotNumber)-1;
                               fzSL_ptr->fvhnPtr = fv_ptr;
                               i++;
}
                         }

                       /*=====================================================*/
                       /* Move on to the next slot in the pattern.            */
                       /*=====================================================*/
                       lhsSlotPtr = lhsSlotPtr->right;
                     }
               }

             /*=====================================================*/
             /* Move on to the next pattern in the LHS of the rule. */
             /*=====================================================*/
             lhsPNPtr = lhsPNPtr->bottom;
             patternNum++;
           }

           /* i should == numFuzzySlots OR something internal is screwed up --
               OR this algorithm has a problem.
           */
           if (i != numFuzzySlots)
            {
               EnvPrintRouter(theEnv,WERROR,"Internal ERROR *** Fuzzy structures -- routine ProcessRuleLHS\n");
               genexit( 2 );
            }
        }
#else
      currentDisjunct = CreateNewDisjunct(theEnv,ruleName,localVarCnt,packPtr,complexity,
                                          (unsigned) logicalJoin,lastJoin);
#endif

      /*============================================================*/
      /* Place the disjunct in the list of disjuncts for this rule. */
      /* If the disjunct is the first disjunct, then increment the  */
      /* reference counts for the dynamic salience (the expression  */
      /* for the dynamic salience is only stored with the first     */
      /* disjuncts and the other disjuncts refer back to the first  */
      /* disjunct for their dynamic salience value.                 */
      /*============================================================*/

      if (topDisjunct == NULL)
        {
         topDisjunct = currentDisjunct;
         ExpressionInstall(theEnv,topDisjunct->dynamicSalience);
#if CERTAINTY_FACTORS  
         ExpressionInstall(theEnv,topDisjunct->dynamicCF);
#endif
        }
      else lastDisjunct->disjunct = currentDisjunct;

      /*===========================================*/
      /* Move on to the next disjunct of the rule. */
      /*===========================================*/

      lastDisjunct = currentDisjunct;
     }

   return(topDisjunct);
  }

/************************************************************************/
/* CreateNewDisjunct: Creates and initializes a defrule data structure. */
/************************************************************************/
static struct defrule *CreateNewDisjunct(
#if ! FUZZY_DEFTEMPLATES
  void *theEnv,
  SYMBOL_HN *ruleName,
  int localVarCnt,
  struct expr *theActions,
  int complexity,
  unsigned logicalJoin,
  struct joinNode *lastJoin)
#else
  void *theEnv,
  SYMBOL_HN *ruleName,
  int localVarCnt,
  struct expr *theActions,
  int complexity,
  unsigned logicalJoin,
  struct joinNode *lastJoin,
  int numFuzzySlots)
#endif
  {
   struct joinNode *tempJoin;
   struct defrule *newDisjunct;

   /*===================================================*/
   /* Create and initialize the defrule data structure. */
   /*===================================================*/

   newDisjunct = get_struct(theEnv,defrule);
#if FUZZY_DEFTEMPLATES  
   if (numFuzzySlots <= 0)
      newDisjunct->pattern_fv_arrayPtr = NULL;
   else
      newDisjunct->pattern_fv_arrayPtr =
              (struct fzSlotLocator *)gm1(theEnv,sizeof(struct fzSlotLocator) * numFuzzySlots);

   newDisjunct->numberOfFuzzySlots = numFuzzySlots;
#endif

   newDisjunct->header.ppForm = NULL;
   newDisjunct->header.next = NULL;
   newDisjunct->header.usrData = NULL;
   newDisjunct->logicalJoin = NULL;
   newDisjunct->disjunct = NULL;
   newDisjunct->header.name = ruleName;
   IncrementSymbolCount(newDisjunct->header.name);
   newDisjunct->actions = theActions;
   newDisjunct->salience = PatternData(theEnv)->GlobalSalience;
   newDisjunct->afterBreakpoint = 0;
   newDisjunct->watchActivation = 0;
   newDisjunct->watchFiring = 0;
   newDisjunct->executing = 0;
   newDisjunct->complexity = complexity;
   newDisjunct->autoFocus = PatternData(theEnv)->GlobalAutoFocus;
   newDisjunct->dynamicSalience = PatternData(theEnv)->SalienceExpression;
#if CERTAINTY_FACTORS  
   newDisjunct->CF = PatternData(theEnv)->GlobalCF;
   newDisjunct->dynamicCF = PatternData(theEnv)->CFExpression;
#endif
   newDisjunct->localVarCnt = localVarCnt;

   /*=====================================*/
   /* Add a pointer to the rule's module. */
   /*=====================================*/

   newDisjunct->header.whichModule =
      (struct defmoduleItemHeader *)
      GetModuleItem(theEnv,NULL,FindModuleItem(theEnv,"defrule")->moduleIndex);

   /*============================================================*/
   /* Attach the rule's last join to the defrule data structure. */
   /*============================================================*/

   lastJoin->ruleToActivate = newDisjunct;
   newDisjunct->lastJoin = lastJoin;

   /*=================================================*/
   /* Determine the rule's logical join if it exists. */
   /*=================================================*/

   tempJoin = lastJoin;
   while (tempJoin != NULL)
     {
      if (tempJoin->depth == logicalJoin)
        {
         newDisjunct->logicalJoin = tempJoin;
         tempJoin->logicalJoin = TRUE;
        }
      tempJoin = tempJoin->lastLevel;
     }

   /*==================================================*/
   /* Return the newly created defrule data structure. */
   /*==================================================*/

   return(newDisjunct);
  }

/****************************************************************/
/* ReplaceExpressionVariables: Replaces all symbolic references */
/*   to variables (local and global) found in an expression on  */
/*   the RHS of a rule with expressions containing function     */
/*   calls to retrieve the variable's value. Makes the final    */
/*   modifications necessary for handling the modify and        */
/*   duplicate commands.                                        */
/****************************************************************/
static int ReplaceRHSVariable(
  void *theEnv,
  struct expr *list,
  void *VtheLHS)
  {
   struct lhsParseNode *theVariable;

   /*=======================================*/
   /* Handle modify and duplicate commands. */
   /*=======================================*/

#if DEFTEMPLATE_CONSTRUCT
   if (list->type == FCALL)
     {
      if (list->value == (void *) FindFunction(theEnv,"modify"))
        {
         if (UpdateModifyDuplicate(theEnv,list,"modify",VtheLHS) == FALSE)
           return(-1);
        }
      else if (list->value == (void *) FindFunction(theEnv,"duplicate"))
        {
         if (UpdateModifyDuplicate(theEnv,list,"duplicate",VtheLHS) == FALSE)
           return(-1);
        }

      return(0);
     }

#endif

   if ((list->type != SF_VARIABLE) && (list->type != MF_VARIABLE))
     { return(FALSE); }

   /*===============================================================*/
   /* Check to see if the variable is bound on the LHS of the rule. */
   /*===============================================================*/

   theVariable = FindVariable((SYMBOL_HN *) list->value,(struct lhsParseNode *) VtheLHS);
   if (theVariable == NULL) return(FALSE);

   /*================================================*/
   /* Replace the variable reference with a function */
   /* call to retrieve the variable.                 */
   /*================================================*/

   if (theVariable->patternType != NULL)
     { (*theVariable->patternType->replaceGetJNValueFunction)(theEnv,list,theVariable); }
   else
     { return(FALSE); }

   /*=================================================================*/
   /* Return TRUE to indicate the variable was successfully replaced. */
   /*=================================================================*/

   return(TRUE);
  }

/*******************************************************/
/* ParseRuleRHS: Coordinates all the actions necessary */
/*   for parsing the RHS of a rule.                    */
/*******************************************************/
static struct expr *ParseRuleRHS(
  void *theEnv,
  char *readSource)
  {
   struct expr *actions;
   struct token theToken;

   /*=========================================================*/
   /* Process the actions on the right hand side of the rule. */
   /*=========================================================*/

   SavePPBuffer(theEnv,"\n   ");
   SetIndentDepth(theEnv,3);

   actions = GroupActions(theEnv,readSource,&theToken,TRUE,NULL,FALSE);

   if (actions == NULL) return(NULL);

   /*=============================*/
   /* Reformat the closing token. */
   /*=============================*/

   PPBackup(theEnv);
   PPBackup(theEnv);
   SavePPBuffer(theEnv,theToken.printForm);

   /*======================================================*/
   /* Check for the closing right parenthesis of the rule. */
   /*======================================================*/

   if (theToken.type != RPAREN)
     {
      SyntaxErrorMessage(theEnv,"defrule");
      ReturnExpression(theEnv,actions);
      return(NULL);
     }

   /*========================*/
   /* Return the rule's RHS. */
   /*========================*/

   return(actions);
  }

/************************************************************/
/* RuleComplexity: Returns the complexity of a rule for use */
/*   by the LEX and MEA conflict resolution strategies.     */
/************************************************************/
static int RuleComplexity(
  void *theEnv,
  struct lhsParseNode *theLHS)
  {
   struct lhsParseNode *thePattern, *tempPattern;
   int complexity = 0;

   while (theLHS != NULL)
     {
      complexity += 1; /* Add 1 for each pattern. */
      complexity += ExpressionComplexity(theEnv,theLHS->networkTest);
      thePattern = theLHS->right;
      while (thePattern != NULL)
        {
         if (thePattern->multifieldSlot)
           {
            tempPattern = thePattern->bottom;
            while (tempPattern != NULL)
              {
               complexity += ExpressionComplexity(theEnv,tempPattern->networkTest);
               tempPattern = tempPattern->right;
              }
           }
         else
           { complexity += ExpressionComplexity(theEnv,thePattern->networkTest); }
         thePattern = thePattern->right;
        }
      theLHS = theLHS->bottom;
     }

   return(complexity);
  }

/********************************************************************/
/* ExpressionComplexity: Determines the complexity of a expression. */
/********************************************************************/
static int ExpressionComplexity(
   void *theEnv,
  struct expr *exprPtr)
  {
   int complexity = 0;

   while (exprPtr != NULL)
     {
      if (exprPtr->type == FCALL)
        {
         /*=========================================*/
         /* Logical combinations do not add to the  */
         /* complexity, but their arguments do.     */
         /*=========================================*/

         if ((exprPtr->value == ExpressionData(theEnv)->PTR_AND) ||
                  (exprPtr->value == ExpressionData(theEnv)->PTR_NOT) ||
                  (exprPtr->value == ExpressionData(theEnv)->PTR_OR))
           { complexity += ExpressionComplexity(theEnv,exprPtr->argList); }

         /*=========================================*/
         /* else other function calls increase the  */
         /* complexity, but their arguments do not. */
         /*=========================================*/

         else
           { complexity++; }
        }
      else if ((EvaluationData(theEnv)->PrimitivesArray[exprPtr->type] != NULL) ?
               EvaluationData(theEnv)->PrimitivesArray[exprPtr->type]->addsToRuleComplexity : FALSE)
        { complexity++; }

      exprPtr = exprPtr->nextArg;
     }

   return(complexity);
  }

/********************************************/
/* LogicalAnalysis: Analyzes the use of the */
/*   logical CE within the LHS of a rule.   */
/********************************************/
static int LogicalAnalysis(
  void *theEnv,
  struct lhsParseNode *patternList)
  {
   int firstLogical, logicalsFound = FALSE, logicalJoin = 0;
   int gap = FALSE;

   firstLogical = patternList->logical;

   /*===================================================*/
   /* Loop through each pattern in the LHS of the rule. */
   /*===================================================*/

   for (;
        patternList != NULL;
        patternList = patternList->bottom)
     {
      /*=======================================*/
      /* Skip anything that isn't a pattern CE */
      /* or is embedded within a not/and CE.   */
      /*=======================================*/

      if ((patternList->type != PATTERN_CE) || (patternList->endNandDepth != 1))
        { continue; }

      /*=====================================================*/
      /* If the pattern CE is not contained within a logical */
      /* CE, then set the gap flag to TRUE indicating that   */
      /* any subsequent pattern CE found within a logical CE */
      /* represents a gap between logical CEs which is an    */
      /* error.                                              */
      /*=====================================================*/

      if (patternList->logical == FALSE)
        {
         gap = TRUE;
         continue;
        }

      /*=================================================*/
      /* If a logical CE is encountered and the first CE */
      /* of the rule isn't a logical CE, then indicate   */
      /* that the first CE must be a logical CE.         */
      /*=================================================*/

      if (! firstLogical)
        {
         PrintErrorID(theEnv,"RULEPSR",1,TRUE);
         EnvPrintRouter(theEnv,WERROR,"Logical CEs must be placed first in a rule\n");
         return(-1);
        }

      /*===================================================*/
      /* If a break within the logical CEs was found and a */
      /* new logical CE is encountered, then indicate that */
      /* there can't be any gaps between logical CEs.      */
      /*===================================================*/

      if (gap)
        {
         PrintErrorID(theEnv,"RULEPSR",2,TRUE);
         EnvPrintRouter(theEnv,WERROR,"Gaps may not exist between logical CEs\n");
         return(-1);
        }

      /*===========================================*/
      /* Increment the count of logical CEs found. */
      /*===========================================*/

      logicalJoin++;
      logicalsFound = TRUE;
     }

   /*============================================*/
   /* If logical CEs were found, then return the */
   /* join number where the logical information  */
   /* will be stored in the join network.        */
   /*============================================*/

   if (logicalsFound) return(logicalJoin);

   /*=============================*/
   /* Return zero indicating that */
   /* no logical CE was found.    */
   /*=============================*/

   return(0);
  }

/*****************************************************************/
/* FindVariable: Searches for the last occurence of a variable   */
/*  in the LHS of a rule that is visible from the RHS of a rule. */
/*  The last occurence of the variable on the LHS side of the    */
/*  rule will have the strictest constraints (because it will    */
/*  have been intersected with all of the other constraints for  */
/*  the variable on the LHS of the rule). The strictest          */
/*  constraints are useful for performing type checking on the   */
/*  RHS of the rule.                                             */
/*****************************************************************/
globle struct lhsParseNode *FindVariable(
  SYMBOL_HN *name,
  struct lhsParseNode *theLHS)
  {
   struct lhsParseNode *theFields, *tmpFields = NULL;
   struct lhsParseNode *theReturnValue = NULL;

   /*==============================================*/
   /* Loop through each CE in the LHS of the rule. */
   /*==============================================*/

   for (;
        theLHS != NULL;
        theLHS = theLHS->bottom)
     {
      /*==========================================*/
      /* Don't bother searching for the variable  */
      /* in anything other than a pattern CE that */
      /* is not contained within a not CE.        */
      /*==========================================*/

      if ((theLHS->type != PATTERN_CE) ||
          (theLHS->negated == TRUE) ||
          (theLHS->beginNandDepth > 1))
        { continue; }

      /*=====================================*/
      /* Check the pattern address variable. */
      /*=====================================*/

      if (theLHS->value == (void *) name)
        { theReturnValue = theLHS; }

      /*============================================*/
      /* Check for the variable inside the pattern. */
      /*============================================*/

      theFields = theLHS->right;
      while (theFields != NULL)
        {
         /*=================================================*/
         /* Go one level deeper to check a multifield slot. */
         /*=================================================*/

         if (theFields->multifieldSlot)
           {
            tmpFields = theFields;
            theFields = theFields->bottom;
           }

         /*=================================*/
         /* See if the field being examined */
         /* is the variable being sought.   */
         /*=================================*/

         if (theFields == NULL)
           { /* Do Nothing */ }
         else if (((theFields->type == SF_VARIABLE) ||
                   (theFields->type == MF_VARIABLE)) &&
             (theFields->value == (void *) name))
           { theReturnValue = theFields; }

         /*============================*/
         /* Move on to the next field. */
         /*============================*/

         if (theFields == NULL)
           {
            theFields = tmpFields;
            tmpFields = NULL;
           }
         else if ((theFields->right == NULL) && (tmpFields != NULL))
           {
            theFields = tmpFields;
            tmpFields = NULL;
           }
         theFields = theFields->right;
        }
     }

   /*=========================================================*/
   /* Return a pointer to the LHS location where the variable */
   /* was found (or a NULL pointer if it wasn't).             */
   /*=========================================================*/

   return(theReturnValue);
  }

/**********************************************************/
/* AddToDefruleList: Adds a defrule to the list of rules. */
/**********************************************************/
static void AddToDefruleList(
  struct defrule *rulePtr)
  {
   struct defrule *tempRule;
   struct defruleModule *theModuleItem;

   theModuleItem = (struct defruleModule *) rulePtr->header.whichModule;

   if (theModuleItem->header.lastItem == NULL)
     { theModuleItem->header.firstItem = (struct constructHeader *) rulePtr; }
   else
     {
      tempRule = (struct defrule *) theModuleItem->header.lastItem;
      while (tempRule != NULL)
        {
         tempRule->header.next = (struct constructHeader *) rulePtr;
         tempRule = tempRule->disjunct;
        }
     }

   theModuleItem->header.lastItem = (struct constructHeader *) rulePtr;
  }


#endif /* (! RUN_TIME) && (! BLOAD_ONLY) */

#endif /* DEFRULE_CONSTRUCT */

