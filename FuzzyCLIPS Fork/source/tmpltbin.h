   /*******************************************************/
   /*      "C" Language Integrated Production System      */
   /*                                                     */
   /*             CLIPS Version 6.20  01/31/02            */
   /*                                                     */
   /*         DEFTEMPLATE BSAVE/BLOAD HEADER FILE         */
   /*******************************************************/

/*************************************************************/
/* Purpose:                                                  */
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
/*                                                           */
/*************************************************************/

#if (! RUN_TIME)
#ifndef _H_tmpltbin

#define _H_tmpltbin

struct bsaveTemplateSlot
  {
   unsigned long slotName;
   unsigned int multislot : 1;
   unsigned int noDefault : 1;
   unsigned int defaultPresent : 1;
   unsigned int defaultDynamic : 1;
   long constraints;
   long defaultList;
   long next;
  };

struct bsaveDeftemplate;
struct bsaveDeftemplateModule;

#include "cstrcbin.h"

struct bsaveDeftemplate
  {
   struct bsaveConstructHeader header;
   long slotList;
   unsigned int implied : 1;
   unsigned int numberOfSlots : 15;
   long patternNetwork;
#if FUZZY_DEFTEMPLATES
   unsigned int hasFuzzySlots : 1;
   long fuzzyTemplateList;
#endif
  };
  
#if FUZZY_DEFTEMPLATES

struct bsaveLvPlusUniverse
  {
    double from;
    double to;
    long   unitsName;
    long   ptPtr;
  };

struct bsaveFuzzyPrimaryTerm
  {
    long fuzzyValue;
    long next;
  };

#endif

#include "modulbin.h"

struct bsaveDeftemplateModule
  {
   struct bsaveDefmoduleItemHeader header;
  };

#define TMPLTBIN_DATA 61

struct deftemplateBinaryData
  { 
   struct deftemplate *DeftemplateArray;
   long NumberOfDeftemplates;
   long NumberOfTemplateSlots;
   long NumberOfTemplateModules;
   struct templateSlot *SlotArray;
   struct deftemplateModule *ModuleArray;
#if FUZZY_DEFTEMPLATES
   struct primary_term *PrimaryTermArray;
   struct fuzzyLv *LvPlusUniverseArray;
   long NumberOfFuzzyTemplates;
   long NumberOfFuzzyPrimaryTerms;
#endif
  };
  
#define DeftemplateBinaryData(theEnv) ((struct deftemplateBinaryData *) GetEnvironmentData(theEnv,TMPLTBIN_DATA))

#define DeftemplatePointer(i) ((struct deftemplate *) (&DeftemplateBinaryData(theEnv)->DeftemplateArray[i]))

#ifndef _H_tmpltdef
#include "tmpltdef.h"
#endif

#ifdef LOCALE
#undef LOCALE
#endif

#ifdef _TMPLTBIN_SOURCE_
#define LOCALE
#else
#define LOCALE extern
#endif

   LOCALE void                           DeftemplateBinarySetup(void *);
   LOCALE void                          *BloadDeftemplateModuleReference(void *,int);

#endif
#endif



