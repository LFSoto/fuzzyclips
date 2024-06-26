   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.22  06/15/04            */
   /*                                                     */
   /*           FACT RETE PRINT FUNCTIONS MODULE          */
   /*******************************************************/

/*************************************************************/
/* Purpose: Print routines for the fact rete primitives.     */
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
/*************************************************************/

#define _FACTPRT_SOURCE_

#include <stdio.h>
#define _STDIO_INCLUDED_

#include "setup.h"

#if DEFTEMPLATE_CONSTRUCT && DEFRULE_CONSTRUCT

#include "envrnmnt.h"
#include "symbol.h"
#include "router.h"
#include "factgen.h"

#include "factprt.h"

/***************************************/
/* PrintFactJNCompVars1: Print routine */
/*   for the FactJNCompVars1 function. */
/***************************************/
#if IBM_TBC && (! DEVELOPER)
#pragma argsused
#endif
globle void PrintFactJNCompVars1(
  void *theEnv,
  char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factCompVarsJN1Call *hack;

   hack = (struct factCompVarsJN1Call *) ValueToBitMap(theValue);
   EnvPrintRouter(theEnv,logicalName,"(fact-jn-cmp-vars1 ");
   if (hack->pass) EnvPrintRouter(theEnv,logicalName,"p ");
   else EnvPrintRouter(theEnv,logicalName,"n ");
   PrintLongInteger(theEnv,logicalName,(long) hack->slot1);
   EnvPrintRouter(theEnv,logicalName," ");
   PrintLongInteger(theEnv,logicalName,(long) hack->pattern2);
   EnvPrintRouter(theEnv,logicalName," ");
   PrintLongInteger(theEnv,logicalName,(long) hack->slot2);
   EnvPrintRouter(theEnv,logicalName,")");
#else
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/***************************************/
/* PrintFactJNCompVars2: Print routine */
/*   for the FactJNCompVars2 function. */
/***************************************/
#if IBM_TBC && (! DEVELOPER)
#pragma argsused
#endif
globle void PrintFactJNCompVars2(
  void *theEnv,
  char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factCompVarsJN2Call *hack;

   hack = (struct factCompVarsJN2Call *) ValueToBitMap(theValue);
   EnvPrintRouter(theEnv,logicalName,"(fact-jn-cmp-vars2 ");
   if (hack->pass) EnvPrintRouter(theEnv,logicalName,"p ");
   else EnvPrintRouter(theEnv,logicalName,"n ");

   EnvPrintRouter(theEnv,logicalName,"s");
   PrintLongInteger(theEnv,logicalName,(long) hack->slot1);
   EnvPrintRouter(theEnv,logicalName," ");

   if (hack->fromBeginning1) EnvPrintRouter(theEnv,logicalName,"b ");
   else EnvPrintRouter(theEnv,logicalName,"e ");

   EnvPrintRouter(theEnv,logicalName,"f");
   PrintLongInteger(theEnv,logicalName,(long) hack->offset1);
   EnvPrintRouter(theEnv,logicalName," ");

   EnvPrintRouter(theEnv,logicalName,"p");
   PrintLongInteger(theEnv,logicalName,(long) hack->pattern2);
   EnvPrintRouter(theEnv,logicalName," ");

   EnvPrintRouter(theEnv,logicalName,"s");
   PrintLongInteger(theEnv,logicalName,(long) hack->slot2);
   EnvPrintRouter(theEnv,logicalName," ");

   if (hack->fromBeginning2) EnvPrintRouter(theEnv,logicalName,"b ");
   else EnvPrintRouter(theEnv,logicalName,"e ");

   EnvPrintRouter(theEnv,logicalName,"f");
   PrintLongInteger(theEnv,logicalName,(long) hack->offset2);
   EnvPrintRouter(theEnv,logicalName,")");
#else
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/***************************************/
/* PrintFactPNCompVars1: Print routine */
/*   for the FactPNCompVars1 function. */
/***************************************/
#if IBM_TBC && (! DEVELOPER)
#pragma argsused
#endif
globle void PrintFactPNCompVars1(
  void *theEnv,
  char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factCompVarsPN1Call *hack;

   hack = (struct factCompVarsPN1Call *) ValueToBitMap(theValue);
   EnvPrintRouter(theEnv,logicalName,"(fact-pn-cmp-vars ");
   if (hack->pass) EnvPrintRouter(theEnv,logicalName,"p ");
   else EnvPrintRouter(theEnv,logicalName,"n ");
   PrintLongInteger(theEnv,logicalName,(long) hack->field1);
   EnvPrintRouter(theEnv,logicalName," ");
   PrintLongInteger(theEnv,logicalName,(long) hack->field2);
   EnvPrintRouter(theEnv,logicalName,")");
#else
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/**************************************/
/* PrintFactSlotLength: Print routine */
/*   for the FactSlotLength function. */
/**************************************/
#if IBM_TBC && (! DEVELOPER)
#pragma argsused
#endif
globle void PrintFactSlotLength(
  void *theEnv,
  char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factCheckLengthPNCall *hack;

   hack = (struct factCheckLengthPNCall *) ValueToBitMap(theValue);

   EnvPrintRouter(theEnv,logicalName,"(slot-length ");
   PrintLongInteger(theEnv,logicalName,(long) hack->whichSlot);
   EnvPrintRouter(theEnv,logicalName," ");
   if (hack->exactly) EnvPrintRouter(theEnv,logicalName,"= ");
   else EnvPrintRouter(theEnv,logicalName,">= ");
   PrintLongInteger(theEnv,logicalName,(long) hack->minLength);
   EnvPrintRouter(theEnv,logicalName,")");
#else
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/*************************************/
/* PrintFactJNGetVar1: Print routine */
/*   for the FactJNGetvar1 function. */
/*************************************/
#if IBM_TBC && (! DEVELOPER)
#pragma argsused
#endif
globle void PrintFactJNGetVar1(
  void *theEnv,
  char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factGetVarJN1Call *hack;

   hack = (struct factGetVarJN1Call *) ValueToBitMap(theValue);
   EnvPrintRouter(theEnv,logicalName,"(fact-jn-getvar-1 ");
   if (hack->factAddress) EnvPrintRouter(theEnv,logicalName,"t ");
   else EnvPrintRouter(theEnv,logicalName,"f ");
   if (hack->allFields) EnvPrintRouter(theEnv,logicalName,"t ");
   else EnvPrintRouter(theEnv,logicalName,"f ");

   PrintLongInteger(theEnv,logicalName,(long) hack->whichPattern);
   EnvPrintRouter(theEnv,logicalName," ");
   PrintLongInteger(theEnv,logicalName,(long) hack->whichField);
   EnvPrintRouter(theEnv,logicalName," ");
   PrintLongInteger(theEnv,logicalName,(long) hack->whichSlot);
   EnvPrintRouter(theEnv,logicalName,")");
#else
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/*************************************/
/* PrintFactJNGetVar2: Print routine */
/*   for the FactJNGetvar2 function. */
/*************************************/
#if IBM_TBC && (! DEVELOPER)
#pragma argsused
#endif
globle void PrintFactJNGetVar2(
  void *theEnv,
  char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factGetVarJN2Call *hack;

   hack = (struct factGetVarJN2Call *) ValueToBitMap(theValue);
   EnvPrintRouter(theEnv,logicalName,"(fact-jn-getvar-2 ");

   PrintLongInteger(theEnv,logicalName,(long) hack->whichPattern);
   EnvPrintRouter(theEnv,logicalName," ");
   PrintLongInteger(theEnv,logicalName,(long) hack->whichSlot);
   EnvPrintRouter(theEnv,logicalName,")");
#else
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/*************************************/
/* PrintFactJNGetVar3: Print routine */
/*   for the FactJNGetVar3 function. */
/*************************************/
#if IBM_TBC && (! DEVELOPER)
#pragma argsused
#endif
globle void PrintFactJNGetVar3(
  void *theEnv,
  char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factGetVarJN3Call *hack;

   hack = (struct factGetVarJN3Call *) ValueToBitMap(theValue);
   EnvPrintRouter(theEnv,logicalName,"(fact-jn-getvar-3 ");
   if (hack->fromBeginning) EnvPrintRouter(theEnv,logicalName,"t ");
   else EnvPrintRouter(theEnv,logicalName,"f ");
   if (hack->fromEnd) EnvPrintRouter(theEnv,logicalName,"t ");
   else EnvPrintRouter(theEnv,logicalName,"f ");

   PrintLongInteger(theEnv,logicalName,(long) hack->beginOffset);
   EnvPrintRouter(theEnv,logicalName," ");
   PrintLongInteger(theEnv,logicalName,(long) hack->endOffset);
   EnvPrintRouter(theEnv,logicalName," ");
   PrintLongInteger(theEnv,logicalName,(long) hack->whichSlot);
   EnvPrintRouter(theEnv,logicalName,")");
#else
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/*************************************/
/* PrintFactPNGetVar1: Print routine */
/*   for the FactPNGetvar1 function. */
/*************************************/
#if IBM_TBC && (! DEVELOPER)
#pragma argsused
#endif
globle void PrintFactPNGetVar1(
  void *theEnv,
  char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factGetVarPN1Call *hack;

   hack = (struct factGetVarPN1Call *) ValueToBitMap(theValue);
   EnvPrintRouter(theEnv,logicalName,"(fact-pn-getvar-1 ");
   if (hack->factAddress) EnvPrintRouter(theEnv,logicalName,"t ");
   else EnvPrintRouter(theEnv,logicalName,"f ");
   if (hack->allFields) EnvPrintRouter(theEnv,logicalName,"t F");
   else EnvPrintRouter(theEnv,logicalName,"f F");

   PrintLongInteger(theEnv,logicalName,(long) hack->whichField);
   EnvPrintRouter(theEnv,logicalName," S");
   PrintLongInteger(theEnv,logicalName,(long) hack->whichSlot);
   EnvPrintRouter(theEnv,logicalName,")");
#else
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/*************************************/
/* PrintFactPNGetVar2: Print routine */
/*   for the FactPNGetvar2 function. */
/*************************************/
#if IBM_TBC && (! DEVELOPER)
#pragma argsused
#endif
globle void PrintFactPNGetVar2(
  void *theEnv,
  char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factGetVarPN2Call *hack;

   hack = (struct factGetVarPN2Call *) ValueToBitMap(theValue);;
   EnvPrintRouter(theEnv,logicalName,"(fact-pn-getvar-2 S");
   PrintLongInteger(theEnv,logicalName,(long) hack->whichSlot);
   EnvPrintRouter(theEnv,logicalName,")");
#else
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/*************************************/
/* PrintFactPNGetVar3: Print routine */
/*   for the FactPNGetvar3 function. */
/*************************************/
#if IBM_TBC && (! DEVELOPER)
#pragma argsused
#endif
globle void PrintFactPNGetVar3(
  void *theEnv,
  char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factGetVarPN3Call *hack;

   hack = (struct factGetVarPN3Call *) ValueToBitMap(theValue);
   EnvPrintRouter(theEnv,logicalName,"(fact-pn-getvar-3 ");

   if (hack->fromBeginning) EnvPrintRouter(theEnv,logicalName,"t ");
   else EnvPrintRouter(theEnv,logicalName,"f ");
   if (hack->fromEnd) EnvPrintRouter(theEnv,logicalName,"t B");
   else EnvPrintRouter(theEnv,logicalName,"f B");

   PrintLongInteger(theEnv,logicalName,(long) hack->beginOffset);
   EnvPrintRouter(theEnv,logicalName," E");
   PrintLongInteger(theEnv,logicalName,(long) hack->endOffset);
   EnvPrintRouter(theEnv,logicalName," S");
   PrintLongInteger(theEnv,logicalName,(long) hack->whichSlot);
   EnvPrintRouter(theEnv,logicalName,")");
#else
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/***************************************/
/* PrintFactPNConstant1: Print routine */
/*   for the FactPNConstant1 function. */
/***************************************/
#if IBM_TBC && (! DEVELOPER)
#pragma argsused
#endif
globle void PrintFactPNConstant1(
  void *theEnv,
  char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factConstantPN1Call *hack;

   hack = (struct factConstantPN1Call *) ValueToBitMap(theValue);

   EnvPrintRouter(theEnv,logicalName,"(fact-pn-constant1 ");

   PrintLongInteger(theEnv,logicalName,(long) hack->whichSlot);

   if (hack->testForEquality) EnvPrintRouter(theEnv,logicalName," = ");
   else EnvPrintRouter(theEnv,logicalName," != ");

   PrintAtom(theEnv,logicalName,GetFirstArgument()->type,GetFirstArgument()->value);
   EnvPrintRouter(theEnv,logicalName,")");
#else
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

/***************************************/
/* PrintFactPNConstant2: Print routine */
/*   for the FactPNConstant2 function. */
/***************************************/
#if IBM_TBC && (! DEVELOPER)
#pragma argsused
#endif
globle void PrintFactPNConstant2(
  void *theEnv,
  char *logicalName,
  void *theValue)
  {
#if DEVELOPER
   struct factConstantPN2Call *hack;

   hack = (struct factConstantPN2Call *) ValueToBitMap(theValue);

   EnvPrintRouter(theEnv,logicalName,"(fact-pn-constant2 ");

   PrintLongInteger(theEnv,logicalName,(long) hack->whichSlot);

   EnvPrintRouter(theEnv,logicalName," ");

   PrintLongInteger(theEnv,logicalName,(long) hack->offset);

   if (hack->testForEquality) EnvPrintRouter(theEnv,logicalName," = ");
   else EnvPrintRouter(theEnv,logicalName," != ");

   PrintAtom(theEnv,logicalName,GetFirstArgument()->type,GetFirstArgument()->value);
   EnvPrintRouter(theEnv,logicalName,")");
#else
#if MAC_MCW || IBM_MCW || MAC_XCD
#pragma unused(theEnv)
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

#if FUZZY_DEFTEMPLATES

/***********************************************************************/
/* PrintPNFUZZY_VALUE: Print routine for the PN_FUZZY_VALUE function.  */
/***********************************************************************/
#if IBM_TBC && (! DEVELOPER)
#pragma argsused
#endif
globle VOID PrintPNFUZZY_VALUE(
  void *theEnv,
  char *logicalName,
  void *theValue)
  {
#if DEVELOPER

   EnvPrintRouter(theEnv,logicalName,"(fact-pn-fuzzy_value ");

   PrintAtom(logicalName,GetFirstArgument()->type,GetFirstArgument()->value);
   EnvPrintRouter(theEnv,logicalName,")");
#else
#if MAC_MPW
#pragma unused(logicalName)
#pragma unused(theValue)
#endif
#endif
  }

#endif

#endif /* DEFTEMPLATE_CONSTRUCT && DEFRULE_CONSTRUCT */


