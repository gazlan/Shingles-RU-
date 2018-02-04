/* ******************************************************************** **
** @@ Memory Mapped File
** @  Copyrt : 
** @  Author :
** @  Modify :
** @  Update :
** @  Dscr   :
** ******************************************************************** */

#ifndef _MMF_HPP_
#define _MMF_HPP_

#if _MSC_VER > 1000
#pragma once
#endif

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

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
** @@                   Classes
** ******************************************************************** */

/* ******************************************************************** **
** @@ class MMF
** @  Copyrt : 
** @  Author :
** @  Modify :
** @  Update : 
** @  Dscr   :
** ******************************************************************** */

class MMF
{
   private:

      HANDLE   _hFile;
      HANDLE   _hMap;
      DWORD    _dwSizeHi;
      DWORD    _dwSizeLo;
      void*    _pView;
      BOOL     _bReadOnly;
      BOOL     _bOpen;

   public:

                MMF();
      virtual  ~MMF();
   
      BOOL     OpenReadOnly(const char* const pszFileName);
      BOOL     OpenReadWrite(const char* const pszFileName);
      BOOL     IsOpen();
      BOOL     Close();
      BOOL     Flush();
      BYTE*    Buffer();
      DWORD    Size();
};

#endif

/* ******************************************************************** **
** @@                   The End
** ******************************************************************** */