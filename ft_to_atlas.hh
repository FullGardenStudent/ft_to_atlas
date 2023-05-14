#ifndef FT_TO_ATLAS_H_
#define FT_TO_ATLAS_H_

#include "ft2build.h"
#include FT_FREETYPE_H

typedef unsigned int U32;

#include <iostream>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

/*
** A Character struct that hold a character info
*/
struct Character {
  uint32_t char_width, char_height, bearing_x, bearing_y;
  long advance;
  float char_u, char_v, uv_width, uv_height;
};

/*
** TODO: perform error checking if the size of the font is not big enough
** Function: creates a font atlas starting from the given offset.
** @param width : Width of the texture atlas to generate
** @param height : height of the texture atlas to generate
** @param font_paths : A vector of paths to .ttf font files.
** @param output_atlas : whether to output the texture-atlas or not
** @param offset : a vertical offset to tell from wherer to start including
*fonts in the atlas. Its zero by default.
*/
uint8_t *create_font_atlas(U32 width, U32 height,
                           const std::vector<std::string_view> font_paths,
			   const std::vector<uint32_t> font_sizes,
                           bool output_atlas, U32 offset = 0);

/*
** Function: returns the character info of the requested character c w.r.t to
*the font_index as per the font_paths provided while creating the font atlas
** @param c : the character to which you want to retrieve the font info of
** @param font_index : which font the character info should be retrieved from.
*This index is expected to be in order with font_paths vector used while
*creating the font atlas
*/

Character get_char_info(char c, U32 font_index);

// load icons and append them above the font glyphs
  bool load_icons(std::string_view path);

  // returns the length of the given text for that particular font 
  U32 get_text_length(std::string_view text, U32 font_index,float scale);

  // returns the height of the longest character for the given text input. 
  U32 get_text_max_height(std::string_view text, U32 font_index, float scale);
#ifdef __cplusplus
}
#endif

#endif // FT_TO_ATLAS_H_
