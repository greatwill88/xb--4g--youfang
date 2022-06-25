#ifndef COM_DTYPES_H
#define COM_DTYPES_H

/**
  @file com_dtypes.h
  @brief This header file contains general data types that are of use to all
  modules.  */

#include <stdint.h>

/*===========================================================================

                            Data Declarations

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/* For NT apps we want to use the Win32 definitions and/or those
 supplied by the Win32 compiler for things like NULL, MAX, MIN
 abs, labs, etc.
*/
#ifdef T_WINNT
   #error code not present
#endif

/* ------------------------------------------------------------------------
** Constants
** ------------------------------------------------------------------------ */

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

/** @addtogroup utils_services
@{ */

/** @name Macros for Common Data Types
@{ */
#define TRUE   1   /**< Boolean TRUE value. */
#define FALSE  0   /**< Boolean FALSE value. */

#define  ON   1    /**< ON value. */
#define  OFF  0    /**< OFF value. */

#ifndef NULL
  #define NULL  0  /**< NULL value. */
#endif
/** @} */ /* end_name_group Macros for Common Data Types */

/* -----------------------------------------------------------------------
** Standard Types
** ----------------------------------------------------------------------- */

/** @} */ /* end_addtogroup utils_services */

/* The following definitions are the same across platforms.  This first
 group are the sanctioned types.
*/
#ifndef _ARM_ASM_
#ifndef _BOOLEAN_DEFINED

/** @addtogroup utils_services
@{ */
/** Boolean value type. */
typedef  unsigned char      boolean;
#define _BOOLEAN_DEFINED
#endif

#ifndef _UINT32_DEFINED
/** Unsigned 32-bit value.*/
typedef  unsigned long int  uint32;
#define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
/** Unsigned 16-bit value.*/
typedef  unsigned short     uint16;
#define _UINT16_DEFINED
#endif

#ifndef _UINT8_DEFINED
/** Unsigned 8-bit value. */
typedef  unsigned char      uint8;
#define _UINT8_DEFINED
#endif

#ifndef _INT32_DEFINED
/** Signed 32-bit value.*/
typedef  signed long int    int32;
#define _INT32_DEFINED
#endif

#ifndef _S32_DEFINED
/** Signed short 32-bit value.*/
typedef  signed int         s32;
#define _S32_DEFINED
#endif

#ifndef _INT16_DEFINED
/** Signed 16-bit value.*/
typedef  signed short       int16;
#define _INT16_DEFINED
#endif

#ifndef _INT8_DEFINED
/** Signed 8-bit value.*/
typedef  signed char        int8;
#define _INT8_DEFINED
#endif

/** @cond
*/
/* This group are the deprecated types.  Their use should be
** discontinued and new code should use the types above
*/
#ifndef _BYTE_DEFINED
/** DEPRECATED: Unsigned 8  bit value type.*/
typedef  unsigned char      byte;
#define  _BYTE_DEFINED
#endif

/** DEPRECATED: Unsinged 16 bit value type.*/
typedef  unsigned short     word;

/** DEPRECATED: Unsigned 32 bit value type.*/
typedef  unsigned long      dword;

/** DEPRECATED: Unsigned 8  bit value type.*/
typedef  unsigned char      uint1;

/** DEPRECATED: Unsigned 16 bit value type.*/
typedef  unsigned short     uint2;

/** DEPRECATED: Unsigned 32 bit value type.*/
typedef  unsigned long      uint4;

/** DEPRECATED: Signed 8  bit value type. */
typedef  signed char        int1;

/** DEPRECATED: Signed 16 bit value type.*/
typedef  signed short       int2;

/** DEPRECATED: Signed 32 bit value type. */
typedef  long int           int4;

/** DEPRECATED: Signed 32 bit value.*/
typedef  signed long        sint31;

/** DEPRECATED: Signed 16 bit value. */
typedef  signed short       sint15;

/** DEPRECATED: Signed 8  bit value.*/
typedef  signed char        sint7;

typedef  unsigned int  size_t;

typedef uint16 UWord16 ;
typedef uint32 UWord32 ;
typedef int32  Word32 ;
typedef int16  Word16 ;
typedef uint8  UWord8 ;
typedef int8   Word8 ;
typedef int32  Vect32 ;
/** @endcond */

#if (! defined T_WINNT) && (! defined __GNUC__)
  /* Non WinNT Targets */
  #ifndef _INT64_DEFINED
    /** Signed 64-bit value.*/
    typedef long long     int64;
    #define _INT64_DEFINED
  #endif
  #ifndef _UINT64_DEFINED
    /** Unsigned 64-bit value.*/
    typedef  unsigned long long  uint64;
    #define _UINT64_DEFINED
  #endif
#else /* T_WINNT || TARGET_OS_SOLARIS || __GNUC__ */
  /* WINNT or SOLARIS based targets */
  #if (defined __GNUC__)
    #ifndef _INT64_DEFINED
      typedef long long           int64;
      #define _INT64_DEFINED
    #endif
    #ifndef _UINT64_DEFINED
      typedef unsigned long long  uint64;
      #define _UINT64_DEFINED
    #endif
  #else
    typedef  __int64              int64;       /* Signed 64-bit value */
    #ifndef _UINT64_DEFINED
      typedef  unsigned __int64   uint64;      /* Unsigned 64-bit value */
      #define _UINT64_DEFINED
    #endif
  #endif
#endif /* T_WINNT */

#ifndef _UWORD64_DEFINED
#define _UWORD64_DEFINED
typedef uint64 UWord64 ;
#endif

#endif /* _ARM_ASM_ */

#ifdef __cplusplus
}
#endif

/** @} */ /* end_addtogroup utils_services */
#endif  /* COM_DTYPES_H */

