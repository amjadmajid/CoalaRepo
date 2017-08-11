/*
 * stdarg.h
 *
 * Provides facilities for stepping through a list of function arguments of
 * an unknown number and type.
 *
 * NOTE: Gcc should provide stdarg.h, and I believe their version will work
 *       with crtdll. If necessary I think you can replace this with the GCC
 *       stdarg.h.
 *
 * Note that the type used in va_arg is supposed to match the actual type
 * *after default promotions*. Thus, va_arg (..., short) is not valid.
 *
 * This file is part of the Mingw32 package.
 *
 * Contributors:
 *  Created by Colin Peters <colin@bird.fu.is.saga-u.ac.jp>
 *
 *  THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  This source code is offered for use in the public domain. You may
 *  use, modify or distribute it freely.
 *
 *  This code is distributed in the hope that it will be useful but
 *  WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 *  DISCLAMED. This includes but is not limited to warranties of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * $Revision: 1.5 $
 * $Author: cgf $
 * $Date: 2000/02/05 04:04:57 $
 *
 */

#ifndef _STDARG_H_
#define _STDARG_H_

/* All the headers include this file. */
#pragma GCC system_header
#pragma GCC system_header
/* Do not include _mingw.h since _mingw.h includes this file.
#include <_mingw.h>
*/

/**
 * Define version masks
 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa383745(v=vs.85).aspx
 * Values and names are guessed based on comments in the documentation.
 */
#define OSVERSION_MASK            0xFFFF0000
#define SPVERSION_MASK            0x0000FF00
#define SUBVERSION_MASK           0x000000FF

/**
 * Macros to extract values from NTDDI version.
 * Derived from comments on MSDN or social.microsoft.com
 */
#define OSVER(ver) ((ver) & OSVERSION_MASK)
#define SPVER(ver) (((ver) & SPVERSION_MASK) >> 8)
#define SUBVER(ver) ((ver) & SUBVERSION_MASK)
#define WINNTVER(ver) ((ver) / 0x00010000)

/**
 * Macros to create the minimal NTDDI version from _WIN32_WINNT value.
 */
#define NTDDI_VERSION_FROM_WIN32_WINNT(ver) _NTDDI_VERSION_FROM_WIN32_WINNT(ver)
#define _NTDDI_VERSION_FROM_WIN32_WINNT(ver) ver##0000

/**
 * Version constants defining _WIN32_WINNT versions.
 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa383745(v=vs.85).aspx
 */
#define _WIN32_WINNT_NT4          0x0400
#define _WIN32_WINNT_NT4E     0x0401
#define _WIN32_WINNT_WIN95    0x0400
#define _WIN32_WINNT_WIN98    0x0410
#define _WIN32_WINNT_WINME    0x0490
#define _WIN32_WINNT_WIN2K        0x0500
#define _WIN32_WINNT_WINXP        0x0501
#define _WIN32_WINNT_WS03         0x0502
#define _WIN32_WINNT_WIN6         0x0600
#define _WIN32_WINNT_VISTA        0x0600
#define _WIN32_WINNT_WS08         0x0600
#define _WIN32_WINNT_LONGORN      0x0600
#define _WIN32_WINNT_WIN7         0x0601

/**
 * Version constants defining _WIN32_IE versions.
 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa383745(v=vs.85).aspx
 */
#define _WIN32_IE_IE50            0x0500
#define _WIN32_IE_IE501           0x0501
#define _WIN32_IE_IE55            0x0550
#define _WIN32_IE_IE60            0x0600
#define _WIN32_IE_IE60SP1         0x0601
#define _WIN32_IE_IE60SP2         0x0603
#define _WIN32_IE_IE70            0x0700
#define _WIN32_IE_IE80            0x0800

/**
 * Version constants defining NTDDI_VERSION.
 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa383745(v=vs.85).aspx
 */
#define __NTDDI_WIN5               0x05000000
#define __NTDDI_WIN51              0x05010000
#define __NTDDI_WIN52              0x05020000
#define __NTDDI_WIN6               0x06000000
#define __NTDDI_WIN61              0x06010000
#define __NTDDI_SP0                0x00000000
#define __NTDDI_SP1                0x00000100
#define __NTDDI_SP2                0x00000200
#define __NTDDI_SP3                0x00000300
#define __NTDDI_SP4                0x00000400

#define NTDDI_WIN2K               __NTDDI_WIN5 + __NTDDI_SP0
#define NTDDI_WIN2KSP1            __NTDDI_WIN5 + __NTDDI_SP1
#define NTDDI_WIN2KSP2            __NTDDI_WIN5 + __NTDDI_SP2
#define NTDDI_WIN2KSP3            __NTDDI_WIN5 + __NTDDI_SP3
#define NTDDI_WIN2KSP4            __NTDDI_WIN5 + __NTDDI_SP4

#define NTDDI_WINXP               __NTDDI_WIN51 + __NTDDI_SP0
#define NTDDI_WINXPSP1            __NTDDI_WIN51 + __NTDDI_SP1
#define NTDDI_WINXPSP2            __NTDDI_WIN51 + __NTDDI_SP2
#define NTDDI_WINXPSP3            __NTDDI_WIN51 + __NTDDI_SP3

#define NTDDI_WS03                __NTDDI_WIN52 + __NTDDI_SP0
#define NTDDI_WS03SP1             __NTDDI_WIN52 + __NTDDI_SP1
#define NTDDI_WS03SP2             __NTDDI_WIN52 + __NTDDI_SP2

#define NTDDI_VISTA               __NTDDI_WIN6 + __NTDDI_SP0
#define NTDDI_VISTASP1            __NTDDI_WIN6 + __NTDDI_SP1
#define NTDDI_VISTASP2        __NTDDI_WIN6 + __NTDDI_SP2

#define NTDDI_LONGHORN            NTDDI_VISTA

#define NTDDI_WIN6                NTDDI_VISTA
#define NTDDI_WIN6SP1             NTDDI_VISTASP1
#define NTDDI_WIN6SP2         NTDDI_VISTASP2

#define NTDDI_WS08                __NTDDI_WIN6 + __NTDDI_SP1

#define NTDDI_WIN7                __NTDDI_WIN61 + __NTDDI_SP0

/**
 * Assign defaults
 */
#ifdef NTDDI_VERSION
#  ifdef _WIN32_WINNT
#    if _WIN32_WINNT != OSDIR(NTDDI_VERSION)
#      error The _WIN32_WINNT value does not match NTDDI_VERSION
#    endif
#  else
#    define _WIN32_WINNT WINNTVER(NTDDI_VERSION)
#    ifndef WINVER
#      define WINVER _WIN32_WINNT
#    endif
#  endif
#endif

#ifndef _WIN32_WINNT
#  ifdef WINVER
#    define _WIN32_WINNT WINVER
#  else
#    ifdef _WARN_DEFAULTS
#      warning _WIN32_WINNT is defaulting to _WIN32_WINNT_WIN2K
#    endif
#    define _WIN32_WINNT _WIN32_WINNT_WIN2K
#  endif
#endif

#ifndef WINVER
#  define WINVER _WIN32_WINNT
#endif

#ifndef NTDDI_VERSION
#  ifdef _WARN_DEFAULTS
#    warning NTDDI_VERSION is defaulting to _WIN32_WINNT version SPK0
#  endif
#  define NTDDI_VERSION NTDDI_VERSION_FROM_WIN32_WINNT(_WIN32_WINNT)
#endif


#define __MINGW_VERSION             4.0
#define __MINGW_MAJOR_VERSION       4
#define __MINGW_MINOR_VERSION       0
#define __MINGW_PATCHLEVEL          0

/* The following four macros are deprecated and will be removed
 * in the release greater than 4.1.
 */
#define __MINGW32_VERSION           3.20
#define __MINGW32_MAJOR_VERSION     3
#define __MINGW32_MINOR_VERSION     20
#define __MINGW32_PATCHLEVEL        0

#ifndef __GNUC__
#error ERROR: You must use a GNU Compiler.
#endif

#if (__GNUC__ < 3 || !defined(__GNUC_MINOR__) || (__GNUC__ == 3 && __GNUC_MINOR__ < 4) || (__GNUC__ == 3 && __GNUC_MINOR__ == 4 && __GNUC_PATCHLEVEL__ < 5))
#error ERROR: You must use a GNU Compiler version >= 3.4.5.
#endif

/* These are defined by the user (or the compiler)
 * to specify how identifiers are imported from a DLL.
 *
 * __MINGW_IMPORT                  The attribute definition to specify imported
 *                                 variables/functions.
 * _CRTIMP                         As above.  For MS compatibility.
 * __MINGW_VERSION                 Runtime version.
 * __MINGW_MAJOR_VERSION           Runtime major version.
 * __MINGW_MINOR_VERSION           Runtime minor version.
 * __MINGW_BUILD_DATE              Runtime build date.
 *
 * Macros to enable MinGW features which deviate from standard MSVC
 * compatible behaviour; these may be specified directly in user code,
 * activated implicitly, (e.g. by specifying _POSIX_C_SOURCE or such),
 * or by inclusion in __MINGW_FEATURES__:
 *
 * __USE_MINGW_ANSI_STDIO          Select a more ANSI C99 compatible
 *                                 implementation of printf() and friends.
 *
 * Other macros:
 *
 * __int64                         define to be long long.  Using a typedef
 *                                 doesn't work for "unsigned __int64"
 *
 *
 * Manifest definitions for flags to control globbing of the command line
 * during application start up, (before main() is called).  The first pair,
 * when assigned as bit flags within _CRT_glob, select the globbing algorithm
 * to be used; (the MINGW algorithm overrides MSCVRT, if both are specified).
 * Prior to mingwrt-3.21, only the MSVCRT option was supported; this choice
 * may produce different results, depending on which particular version of
 * MSVCRT.DLL is in use; (in recent versions, it seems to have become
 * definitively broken, when globbing within double quotes).
 */
#define __CRT_GLOB_USE_MSVCRT__     0x0001

/* From mingwrt-3.21 onward, this should be the preferred choice; it will
 * produce consistent results, regardless of the MSVCRT.DLL version in use.
 */
#define __CRT_GLOB_USE_MINGW__      0x0002

/* When the __CRT_GLOB_USE_MINGW__ flag is set, within _CRT_glob, the
 * following additional options are also available, (but are not enabled
 * by default):
 *
 *    __CRT_GLOB_USE_SINGLE_QUOTE__ allows use of single (apostrophe)
 *                      quoting characters, analogously to
 *                      POSIX usage, as an alternative to
 *                      double quotes, for collection of
 *                      arguments separated by white space
 *                      into a single logical argument.
 *
 *    __CRT_GLOB_BRACKET_GROUPS__   enable interpretation of bracketed
 *                      character groups as POSIX compatible
 *                      globbing patterns, matching any one
 *                      character which is either included
 *                      in, or excluded from the group.
 *
 *    __CRT_GLOB_CASE_SENSITIVE__   enable case sensitive matching for
 *                      globbing patterns; this is default
 *                      behaviour for POSIX, but because of
 *                      the case insensitive nature of the
 *                      MS-Windows file system, it is more
 *                      appropriate to use case insensitive
 *                      globbing as the MinGW default.
 *
 */
#define __CRT_GLOB_USE_SINGLE_QUOTE__   0x0010
#define __CRT_GLOB_BRACKET_GROUPS__ 0x0020
#define __CRT_GLOB_CASE_SENSITIVE__ 0x0040

/* The MinGW globbing algorithm uses the ASCII DEL control code as a marker
 * for globbing characters which were embedded within quoted arguments; (the
 * quotes are stripped away BEFORE the argument is globbed; the globbing code
 * treats the marked character as immutable, and strips out the DEL markers,
 * before storing the resultant argument).  The DEL code is mapped to this
 * function here; DO NOT change it, without rebuilding the runtime.
 */
#define __CRT_GLOB_ESCAPE_CHAR__    (char)(127)


/* Manifest definitions identifying the flag bits, controlling activation
 * of MinGW features, as specified by the user in __MINGW_FEATURES__.
 */
#define __MINGW_ANSI_STDIO__        0x0000000000000001ULL
/*
 * The following three are not yet formally supported; they are
 * included here, to document anticipated future usage.
 */
#define __MINGW_LC_EXTENSIONS__     0x0000000000000050ULL
#define __MINGW_LC_MESSAGES__       0x0000000000000010ULL
#define __MINGW_LC_ENVVARS__        0x0000000000000040ULL

/* Try to avoid problems with outdated checks for GCC __attribute__ support.  */
#undef __attribute__

#ifndef __MINGW_IMPORT
 /* Note the extern. This is needed to work around GCC's
  * limitations in handling dllimport attribute.
  */
# define __MINGW_IMPORT  extern __attribute__ ((__dllimport__))
#endif
#ifndef _CRTIMP
# ifdef __USE_CRTIMP
#  define _CRTIMP  __attribute__ ((dllimport))
# else
#  define _CRTIMP
# endif
#endif

#ifndef __int64
# define __int64 long long
#endif
#ifndef __int32
# define __int32 long
#endif
#ifndef __int16
# define __int16 short
#endif
#ifndef __int8
# define __int8 char
#endif
#ifndef __small
# define __small char
#endif
#ifndef __hyper
# define __hyper long long
#endif

#define __MINGW_GNUC_PREREQ(major, minor) \
 (__GNUC__ > (major) || (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))

#ifdef __cplusplus
# define __CRT_INLINE inline
#else
# if __GNUC_STDC_INLINE__
#  define __CRT_INLINE extern inline __attribute__((__gnu_inline__))
# else
#  define __CRT_INLINE extern __inline__
# endif
#endif

#define _CRTALIAS __CRT_INLINE __attribute__ ((__always_inline__))

#ifdef __cplusplus
# define   BEGIN_C_DECLS    extern "C" {
# define __UNUSED_PARAM(x)
# define   END_C_DECLS      }
#else
# define   BEGIN_C_DECLS
# define __UNUSED_PARAM(x)  x __attribute__ ((__unused__))
# define   END_C_DECLS
#endif

#define __MINGW_ATTRIB_NORETURN __attribute__ ((__noreturn__))
#define __MINGW_ATTRIB_CONST __attribute__ ((__const__))
#define __MINGW_ATTRIB_MALLOC __attribute__ ((__malloc__))
#define __MINGW_ATTRIB_PURE __attribute__ ((__pure__))
#define __MINGW_ATTRIB_NONNULL(arg) __attribute__ ((__nonnull__ (arg)))
#define __MINGW_ATTRIB_DEPRECATED __attribute__ ((__deprecated__))
#define __MINGW_NOTHROW __attribute__ ((__nothrow__))


/* Activation of MinGW specific extended features:
 *
 * TODO: Mark (almost) all CRT functions as __MINGW_NOTHROW.  This will
 * allow GCC to optimize away some EH unwind code, at least in DW2 case.
 */
#ifndef __USE_MINGW_ANSI_STDIO
/*
 * If user didn't specify it explicitly...
 */
# if   defined __STRICT_ANSI__  ||  defined _ISOC99_SOURCE \
   ||  defined _POSIX_SOURCE    ||  defined _POSIX_C_SOURCE \
   ||  defined _XOPEN_SOURCE    ||  defined _XOPEN_SOURCE_EXTENDED \
   ||  defined _GNU_SOURCE      ||  defined _BSD_SOURCE \
   ||  defined _SVID_SOURCE
   /*
    * but where any of these source code qualifiers are specified,
    * then assume ANSI I/O standards are preferred over Microsoft's...
    */
#  define __USE_MINGW_ANSI_STDIO    1
# else
   /*
    * otherwise use whatever __MINGW_FEATURES__ specifies...
    */
#  define __USE_MINGW_ANSI_STDIO    (__MINGW_FEATURES__ & __MINGW_ANSI_STDIO__)
# endif
#endif

/*
 * We need to set a default MSVCRT_VERSION which describes the MSVCRT.DLL on
 * the users system.  We are defaulting to XP but we recommend the user define
 * this in his config.h or Makefile file based on the minimum supported version
 * of OS for his program.
 * ME = 600
 * XP = 710
 * VISTA = 800
 * WIN7 = 900
 * WIN8 = 1010
 */
#ifndef MSVCRT_VERSION
#define MSVCRT_VERSION 710
#endif

#ifdef _USE_32BIT_TIME_T
#if MSVCRT_VERSION < 800
#warning Your MSVCRT_VERSION does not support the use of _USE_32BIT_TIME_T.
#warning You should define MSVCRT_VERSION based on your MSVCRT.DLL version.
#warning ME = 600, XP = 710, VISTA = 800, WIN7 = 900 and WIN8 = 1010.
#endif /* MSVCRT_VERSION < 800 */
#endif /* _USE_32BIT_TIME_T */

struct threadlocalinfostruct;
struct threadmbinfostruct;
typedef struct threadlocalinfostruct *pthreadlocinfo;
typedef struct threadmbcinfostruct *pthreadmbcinfo;

typedef struct localeinfo_struct {
  pthreadlocinfo locinfo;
  pthreadmbcinfo mbcinfo;
} _locale_tstruct, *_locale_t;

/* The __AW() definition will be used for mapping UNICODE versus ASCII versions
 * to the non represented names.  This is accomplished by macro expansion of
 * the symbol passed and concantenating either A or W to the symbol.
 */
#define __AW__(AW, AW_) AW ## AW_
#if ( \
 (!defined(__TEST_SQL_NOUNICODEMAP) && defined(UNICODE)) || \
 (!defined(__TEST_SQL_NOUNICODEMAP) && defined(_UNICODE)) || \
 defined(FORCE_UNICODE) || \
 (defined(__TEST_SQL_NOUNICODEMAP) && !defined(SQL_NOUNICODEMAP) && \
   (defined(UNICODE) || defined(_UNICODE))) \
 )
# define __AW(AW) __AW__(AW, W)
# define __STR(AW) __AW__(L, AW)
#else
# define __AW(AW) __AW__(AW, A)
# define __STR(AW) __AW__(, AW)
#endif

/*
 * Don't do any of this stuff for the resource compiler.
 */
#ifndef RC_INVOKED

/*
 * I was told that Win NT likes this.
 */
#ifndef _VA_LIST_DEFINED
#define _VA_LIST_DEFINED
#endif

#ifndef _VA_LIST
#define _VA_LIST
#if defined __GNUC__ && __GNUC__ >= 3
typedef __builtin_va_list va_list;
#else
typedef char* va_list;
#endif
#endif

/*
 * Amount of space required in an argument list (ie. the stack) for an
 * argument of type t.
 */
#define __va_argsiz(t)  \
    (((sizeof(t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))


/*
 * Start variable argument list processing by setting AP to point to the
 * argument after pN.
 */
#ifdef  __GNUC__
/*
 * In GNU the stack is not necessarily arranged very neatly in order to
 * pack shorts and such into a smaller argument list. Fortunately a
 * neatly arranged version is available through the use of __builtin_next_arg.
 */
#define va_start(ap, pN)    \
    ((ap) = ((va_list) __builtin_next_arg(pN)))
#else
/*
 * For a simple minded compiler this should work (it works in GNU too for
 * vararg lists that don't follow shorts and such).
 */
#define va_start(ap, pN)    \
    ((ap) = ((va_list) (&pN) + __va_argsiz(pN)))
#endif


/*
 * End processing of variable argument list. In this case we do nothing.
 */
#define va_end(ap)  ((void)0)


/*
 * Increment ap to the next argument in the list while returing a
 * pointer to what ap pointed to first, which is of type t.
 *
 * We cast to void* and then to t* because this avoids a warning about
 * increasing the alignment requirement.
 */

#define va_arg(ap, t)                   \
     (((ap) = (ap) + __va_argsiz(t)),       \
      *((t*) (void*) ((ap) - __va_argsiz(t))))

#endif /* Not RC_INVOKED */

#endif /* not _STDARG_H_ */
