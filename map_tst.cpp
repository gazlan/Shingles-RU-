/* ******************************************************************** **
** @@ Ternary Search Tree
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

/* ******************************************************************** **
** uses pre-compiled headers
** ******************************************************************** */

#include "stdafx.h"

#include "map_tst.h"

/* ******************************************************************** **
** @@ internal defines
** ******************************************************************** */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ TST::TST()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : Constructor
** ******************************************************************** */

TST::TST()
{ 
   _pRoot  = NULL;
   _pData  = NULL;

   _iNodes = 0;

   _bInserted = false;

   #ifdef _DEBUG
   _dwID = 0;
   _dwParentID = 0;
   #endif   
}

/* ******************************************************************** **
** @@ TST::~TST()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : Destructor
** ******************************************************************** */

TST::~TST()
{
   Cleanup(_pRoot);
}

/* ******************************************************************** **
** @@ TST::Insert()
** @  Copyrt :
** @  Author :
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

bool TST::Insert(char* pStr)
{
   _pRoot = Insert(_pRoot,pStr);

   return _bInserted;
}

/* ******************************************************************** **
** @@ TST::Insert()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

TST_Node* TST::Insert(TST_Node* pNode,char* pStr)
{
   _bInserted = false;

   if (!pNode)
   {
      pNode = new TST_Node;

      if (!pNode)
      {
         ASSERT(0);
         return NULL;
      }

      memset(pNode,0,sizeof(TST_Node));

      #ifdef _DEBUG
      pNode->_dwID       = _dwID++;
      pNode->_dwParentID = _dwParentID;
      #endif   

      pNode->_cSplitter = *pStr;
      ++_iNodes;
   }

   #ifdef _DEBUG
   _dwParentID = pNode->_dwID; 
   #endif   

   if (*pStr < pNode->_cSplitter)
   {
      pNode->_pLo = Insert(pNode->_pLo,pStr);
   }
   else if (*pStr == pNode->_cSplitter)
   {
      if (!*pStr)
      {
         if (!pNode->_pEq) // No Dups !
         {
            pNode->_pEq = (TST_Node*)_pData;
            _bInserted  = true;
         }
      }
      else
      {
         pNode->_pEq = Insert(pNode->_pEq,++pStr);
      }
   }
   else
   {
      pNode->_pHi = Insert(pNode->_pHi,pStr);
   }

   return pNode;
}

/* ******************************************************************** **
** @@ TST::Cleanup()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void TST::Cleanup(TST_Node* pNode)
{
   if (!pNode)
   {
      return;
   }

   Cleanup(pNode->_pLo);

   if (pNode->_cSplitter)
   {
      Cleanup(pNode->_pEq);
   }

   Cleanup(pNode->_pHi);

   delete pNode;
   pNode = NULL;
}

/* ******************************************************************** **
** @@ TST::Search()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

void* TST::Search(char* pStr)
{
   TST_Node*   pNode = _pRoot;

   while (pNode)
   {
      if (*pStr < pNode->_cSplitter)
      {
         pNode = pNode->_pLo;
      }
      else if (*pStr == pNode->_cSplitter)
      {
         if (!*pStr++)
         {
           return pNode->_pEq;
         }

         pNode = pNode->_pEq;
      }
      else
      {
         pNode = pNode->_pHi;
      }
   }

   return NULL;
}

/* ******************************************************************** **
** @@ TST::Search()
** @  Copyrt :
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  : 
** ******************************************************************** */

bool TST::Search(TST_Node* pNode,char* pStr)
{
   if (!pNode)
   {
      ASSERT(0);
      return false;
   }

   if (*pStr < pNode->_cSplitter)
   {
      return Search(pNode->_pLo,pStr);
   }
   else if (*pStr > pNode->_cSplitter)
   {
      return Search(pNode->_pHi,pStr);
   }
   else
   {
      if (!*pStr)
      {
         return true;
      }

      return Search(pNode->_pEq,++pStr);
   }
}

/* ******************************************************************** **
** End of File
** ******************************************************************** */
