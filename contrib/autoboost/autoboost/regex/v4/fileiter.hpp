/*
 *
 * Copyright (c) 1998-2002
 * John Maddock
 *
 * Use, modification and distribution are subject to the
 * Boost Software License, Version 1.0. (See accompanying file
 * LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         fileiter.hpp
  *   VERSION      see <autoboost/version.hpp>
  *   DESCRIPTION: Declares various platform independent file and
  *                directory iterators, plus binary file input in
  *                the form of class map_file.
  */

#ifndef AUTOBOOST_RE_FILEITER_HPP_INCLUDED
#define AUTOBOOST_RE_FILEITER_HPP_INCLUDED

#ifndef AUTOBOOST_REGEX_CONFIG_HPP
#include <autoboost/regex/config.hpp>
#endif
#include <autoboost/assert.hpp>

#ifndef AUTOBOOST_REGEX_NO_FILEITER

#if (defined(__CYGWIN__) || defined(__CYGWIN32__)) && !defined(AUTOBOOST_REGEX_NO_W32)
#error "Sorry, can't mix <windows.h> with STL code and gcc compiler: if you ran configure, try again with configure --disable-ms-windows"
#define AUTOBOOST_REGEX_FI_WIN32_MAP
#define AUTOBOOST_REGEX_FI_POSIX_DIR
#elif (defined(__WIN32__) || defined(_WIN32) || defined(WIN32)) && !defined(AUTOBOOST_REGEX_NO_W32)
#define AUTOBOOST_REGEX_FI_WIN32_MAP
#define AUTOBOOST_REGEX_FI_WIN32_DIR
#else
#define AUTOBOOST_REGEX_FI_POSIX_MAP
#define AUTOBOOST_REGEX_FI_POSIX_DIR
#endif

#if defined(AUTOBOOST_REGEX_FI_WIN32_MAP)||defined(AUTOBOOST_REGEX_FI_WIN32_DIR)
#include <windows.h>
#endif

#if defined(AUTOBOOST_REGEX_FI_WIN32_DIR)

#include <cstddef>

namespace autoboost{
   namespace AUTOBOOST_REGEX_DETAIL_NS{

#ifndef AUTOBOOST_NO_ANSI_APIS
typedef WIN32_FIND_DATAA _fi_find_data;
#else
typedef WIN32_FIND_DATAW _fi_find_data;
#endif
typedef HANDLE _fi_find_handle;

   } // namespace AUTOBOOST_REGEX_DETAIL_NS

} // namespace autoboost

#define _fi_invalid_handle INVALID_HANDLE_VALUE
#define _fi_dir FILE_ATTRIBUTE_DIRECTORY

#elif defined(AUTOBOOST_REGEX_FI_POSIX_DIR)

#include <cstddef>
#include <cstdio>
#include <cctype>
#include <iterator>
#include <list>
#include <cassert>
#include <dirent.h>

#if defined(__SUNPRO_CC)
using std::list;
#endif

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

namespace autoboost{
   namespace AUTOBOOST_REGEX_DETAIL_NS{

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_PREFIX
#endif

struct _fi_find_data
{
   unsigned dwFileAttributes;
   char cFileName[MAX_PATH];
};

struct _fi_priv_data;

typedef _fi_priv_data* _fi_find_handle;
#define _fi_invalid_handle 0
#define _fi_dir 1

_fi_find_handle _fi_FindFirstFile(const char* lpFileName, _fi_find_data* lpFindFileData);
bool _fi_FindNextFile(_fi_find_handle hFindFile,   _fi_find_data* lpFindFileData);
bool _fi_FindClose(_fi_find_handle hFindFile);

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_SUFFIX
#endif

   } // namespace AUTOBOOST_REGEX_DETAIL_NS
} // namespace autoboost

#ifdef FindFirstFile
 #undef FindFirstFile
#endif
#ifdef FindNextFile
 #undef FindNextFile
#endif
#ifdef FindClose
 #undef FindClose
#endif

#define FindFirstFileA _fi_FindFirstFile
#define FindNextFileA _fi_FindNextFile
#define FindClose _fi_FindClose

#endif

namespace autoboost{
   namespace AUTOBOOST_REGEX_DETAIL_NS{

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_PREFIX
#endif

#ifdef AUTOBOOST_REGEX_FI_WIN32_MAP // win32 mapfile

class AUTOBOOST_REGEX_DECL mapfile
{
   HANDLE hfile;
   HANDLE hmap;
   const char* _first;
   const char* _last;
public:

   typedef const char* iterator;

   mapfile(){ hfile = hmap = 0; _first = _last = 0; }
   mapfile(const char* file){ hfile = hmap = 0; _first = _last = 0; open(file); }
   ~mapfile(){ close(); }
   void open(const char* file);
   void close();
   const char* begin(){ return _first; }
   const char* end(){ return _last; }
   size_t size(){ return _last - _first; }
   bool valid(){ return (hfile != 0) && (hfile != INVALID_HANDLE_VALUE); }
};


#else

class AUTOBOOST_REGEX_DECL mapfile_iterator;

class AUTOBOOST_REGEX_DECL mapfile
{
   typedef char* pointer;
   std::FILE* hfile;
   long int _size;
   pointer* _first;
   pointer* _last;
   mutable std::list<pointer*> condemed;
   enum sizes
   {
      buf_size = 4096
   };
   void lock(pointer* node)const;
   void unlock(pointer* node)const;
public:

   typedef mapfile_iterator iterator;

   mapfile(){ hfile = 0; _size = 0; _first = _last = 0; }
   mapfile(const char* file){ hfile = 0; _size = 0; _first = _last = 0; open(file); }
   ~mapfile(){ close(); }
   void open(const char* file);
   void close();
   iterator begin()const;
   iterator end()const;
   unsigned long size()const{ return _size; }
   bool valid()const{ return hfile != 0; }
   friend class mapfile_iterator;
};

class AUTOBOOST_REGEX_DECL mapfile_iterator
#if !defined(AUTOBOOST_NO_STD_ITERATOR) || defined(AUTOBOOST_MSVC_STD_ITERATOR)
: public std::iterator<std::random_access_iterator_tag, char>
#endif
{
   typedef mapfile::pointer internal_pointer;
   internal_pointer* node;
   const mapfile* file;
   unsigned long offset;
   long position()const
   {
      return file ? ((node - file->_first) * mapfile::buf_size + offset) : 0;
   }
   void position(long pos)
   {
      if(file)
      {
         node = file->_first + (pos / mapfile::buf_size);
         offset = pos % mapfile::buf_size;
      }
   }
public:
   typedef std::ptrdiff_t                  difference_type;
   typedef char                            value_type;
   typedef const char*                     pointer;
   typedef const char&                     reference;
   typedef std::random_access_iterator_tag iterator_category;

   mapfile_iterator() { node = 0; file = 0; offset = 0; }
   mapfile_iterator(const mapfile* f, long arg_position)
   {
      file = f;
      node = f->_first + arg_position / mapfile::buf_size;
      offset = arg_position % mapfile::buf_size;
      if(file)
         file->lock(node);
   }
   mapfile_iterator(const mapfile_iterator& i)
   {
      file = i.file;
      node = i.node;
      offset = i.offset;
      if(file)
         file->lock(node);
   }
   ~mapfile_iterator()
   {
      if(file && node)
         file->unlock(node);
   }
   mapfile_iterator& operator = (const mapfile_iterator& i);
   char operator* ()const
   {
      AUTOBOOST_ASSERT(node >= file->_first);
      AUTOBOOST_ASSERT(node < file->_last);
      return file ? *(*node + sizeof(int) + offset) : char(0);
   }
   char operator[] (long off)const
   {
      mapfile_iterator tmp(*this);
      tmp += off;
      return *tmp;
   }
   mapfile_iterator& operator++ ();
   mapfile_iterator operator++ (int);
   mapfile_iterator& operator-- ();
   mapfile_iterator operator-- (int);

   mapfile_iterator& operator += (long off)
   {
      position(position() + off);
      return *this;
   }
   mapfile_iterator& operator -= (long off)
   {
      position(position() - off);
      return *this;
   }

   friend inline bool operator==(const mapfile_iterator& i, const mapfile_iterator& j)
   {
      return (i.file == j.file) && (i.node == j.node) && (i.offset == j.offset);
   }

   friend inline bool operator!=(const mapfile_iterator& i, const mapfile_iterator& j)
   {
      return !(i == j);
   }

   friend inline bool operator<(const mapfile_iterator& i, const mapfile_iterator& j)
   {
      return i.position() < j.position();
   }
   friend inline bool operator>(const mapfile_iterator& i, const mapfile_iterator& j)
   {
      return i.position() > j.position();
   }
   friend inline bool operator<=(const mapfile_iterator& i, const mapfile_iterator& j)
   {
      return i.position() <= j.position();
   }
   friend inline bool operator>=(const mapfile_iterator& i, const mapfile_iterator& j)
   {
      return i.position() >= j.position();
   }

   friend mapfile_iterator operator + (const mapfile_iterator& i, long off);
   friend mapfile_iterator operator + (long off, const mapfile_iterator& i)
   {
      mapfile_iterator tmp(i);
      return tmp += off;
   }
   friend mapfile_iterator operator - (const mapfile_iterator& i, long off);
   friend inline long operator - (const mapfile_iterator& i, const mapfile_iterator& j)
   {
      return i.position() - j.position();
   }
};

#endif

// _fi_sep determines the directory separator, either '\\' or '/'
AUTOBOOST_REGEX_DECL extern const char* _fi_sep;

struct file_iterator_ref
{
   _fi_find_handle hf;
   _fi_find_data _data;
   long count;
};


class AUTOBOOST_REGEX_DECL file_iterator
{
   char* _root;
   char* _path;
   char* ptr;
   file_iterator_ref* ref;

public:
   typedef std::ptrdiff_t            difference_type;
   typedef const char*               value_type;
   typedef const char**              pointer;
   typedef const char*&              reference;
   typedef std::input_iterator_tag   iterator_category;

   file_iterator();
   file_iterator(const char* wild);
   ~file_iterator();
   file_iterator(const file_iterator&);
   file_iterator& operator=(const file_iterator&);
   const char* root()const { return _root; }
   const char* path()const { return _path; }
   const char* name()const { return ptr; }
   _fi_find_data* data() { return &(ref->_data); }
   void next();
   file_iterator& operator++() { next(); return *this; }
   file_iterator operator++(int);
   const char* operator*() { return path(); }

   friend inline bool operator == (const file_iterator& f1, const file_iterator& f2)
   {
      return ((f1.ref->hf == _fi_invalid_handle) && (f2.ref->hf == _fi_invalid_handle));
   }

   friend inline bool operator != (const file_iterator& f1, const file_iterator& f2)
   {
      return !(f1 == f2);
   }

};

// dwa 9/13/00 - suppress unused parameter warning
inline bool operator < (const file_iterator&, const file_iterator&)
{
   return false;
}


class AUTOBOOST_REGEX_DECL directory_iterator
{
   char* _root;
   char* _path;
   char* ptr;
   file_iterator_ref* ref;

public:
   typedef std::ptrdiff_t            difference_type;
   typedef const char*               value_type;
   typedef const char**              pointer;
   typedef const char*&              reference;
   typedef std::input_iterator_tag   iterator_category;

   directory_iterator();
   directory_iterator(const char* wild);
   ~directory_iterator();
   directory_iterator(const directory_iterator& other);
   directory_iterator& operator=(const directory_iterator& other);

   const char* root()const { return _root; }
   const char* path()const { return _path; }
   const char* name()const { return ptr; }
   _fi_find_data* data() { return &(ref->_data); }
   void next();
   directory_iterator& operator++() { next(); return *this; }
   directory_iterator operator++(int);
   const char* operator*() { return path(); }

   static const char* separator() { return _fi_sep; }

   friend inline bool operator == (const directory_iterator& f1, const directory_iterator& f2)
   {
      return ((f1.ref->hf == _fi_invalid_handle) && (f2.ref->hf == _fi_invalid_handle));
   }


   friend inline bool operator != (const directory_iterator& f1, const directory_iterator& f2)
   {
      return !(f1 == f2);
   }

   };

inline bool operator < (const directory_iterator&, const directory_iterator&)
{
   return false;
}

#ifdef AUTOBOOST_HAS_ABI_HEADERS
#  include AUTOBOOST_ABI_SUFFIX
#endif


} // namespace AUTOBOOST_REGEX_DETAIL_NS
using autoboost::AUTOBOOST_REGEX_DETAIL_NS::directory_iterator;
using autoboost::AUTOBOOST_REGEX_DETAIL_NS::file_iterator;
using autoboost::AUTOBOOST_REGEX_DETAIL_NS::mapfile;
} // namespace autoboost

#endif     // AUTOBOOST_REGEX_NO_FILEITER
#endif     // AUTOBOOST_RE_FILEITER_HPP


















