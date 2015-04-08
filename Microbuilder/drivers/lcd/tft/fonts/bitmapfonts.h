#ifndef __BITMAPFONTS_H__
#define __BITMAPFONTS_H__

#include "projectconfig.h"

/**************************************************************************/
/*! 
    @brief Describes a single character's display information
*/
/**************************************************************************/
typedef struct
{
  const uint8_t widthBits;              // width, in bits (or pixels), of the character
  const uint16_t offset;                // offset of the character's bitmap, in bytes, into the the FONT_INFO's data array
} FONT_CHAR_INFO;	

/**************************************************************************/
/*! 
    @brief Describes a single font
*/
/**************************************************************************/
typedef struct
{
  const uint8_t           height;       // height of the font's characters
  const uint8_t           startChar;    // the first character in the font (e.g. in charInfo and data)
  const uint8_t           endChar;      // the last character in the font (e.g. in charInfo and data)
  const FONT_CHAR_INFO*	  charInfo;     // pointer to array of char information
  const uint8_t*          data;         // pointer to generated array of character visual representation
} FONT_INFO;

#endif
