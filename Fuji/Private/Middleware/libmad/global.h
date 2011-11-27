/*
 * libmad - MPEG audio decoder library
 * Copyright (C) 2000-2004 Underbit Technologies, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: global.h,v 1.11 2004/01/23 09:41:32 rob Exp $
 */

# ifndef LIBMAD_GLOBAL_H
# define LIBMAD_GLOBAL_H

/* conditional debugging */

# if defined(DEBUG) && defined(NDEBUG)
#  error "cannot define both DEBUG and NDEBUG"
# endif

# if defined(DEBUG)
#  include <stdio.h>
# endif

/* conditional features */

# if defined(OPT_SPEED) && defined(OPT_ACCURACY)
#  error "cannot optimize for both speed and accuracy"
# endif

# if defined(OPT_SPEED) && !defined(OPT_SSO)
#  define OPT_SSO
# endif

# if defined(HAVE_UNISTD_H) && defined(HAVE_WAITPID) &&  \
    defined(HAVE_FCNTL) && defined(HAVE_PIPE) && defined(HAVE_FORK)
#  define USE_ASYNC
# endif

# if !defined(HAVE_ASSERT_H)
#  if defined(NDEBUG)
#   define assert(x)	/* nothing */
#  else
#   define assert(x)	do { if (!(x)) abort(); } while (0)
#  endif
# endif

// select the appropriate decoder
# if defined(_XBOX)
#   if _XBOX_VER < 200
#error
#     define FPM_INTEL
#   elif _XBOX_VER >= 200
#     define FPM_PPC
#   else
#     error XBox version undefined...
#   endif
# elif defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA644)
#   define FPM_64BIT
# elif defined(__i386__) || defined(_M_IX86)
#error
#   define FPM_INTEL
# elif defined(__arm__) || defined(_ANDROID) || defined(_SYMBIAN)
#   define FPM_ARM
# elif defined(__ppc) || defined(__powerpc__) || defined(__PowerPC__) || defined(__PPC__) || defined(__ppc__) || defined(_M_MPPC) || defined(_GC) || defined(__wii__) || defined(_WII)
#   define FPM_PPC
# elif defined(__ppc64__) || defined(__PPU__)
#   define FPM_PPC
# elif defined(_MIPS_ARCH) || defined(_mips) || defined(__mips) || defined(__mips__) || defined(__MIPSEL__) || defined(_MIPSEL) || defined(__MIPSEL) || defined(_M_MRX000) \
	|| defined(PSP) || defined(__psp__) || defined(__PSP__) || defined(_PSP) || defined(_EE_) || defined(_EE) || defined(_R5900) || defined(__R5900)
#   define FPM_MIPS
# elif defined(_DC) || defined(__SH4__) || defined(__SH4_SINGLE_ONLY__)
#   define FPM_DEFAULT
# elif defined(TARGET_OS_IPHONE)
#   include <TargetConditionals.h>
#   if TARGET_IPHONE_SIMULATOR == 1
#error
#     define FPM_INTEL
#   else
#     define FPM_ARM
#   endif
# else
# pragma message("Unknown architecture!")
#   define FPM_DEFAULT
# endif

# endif
