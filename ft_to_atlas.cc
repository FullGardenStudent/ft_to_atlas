// Author : FullGardenStudent
// Description:
//////////////////////////////////////////////////////////////////////

#include "../ft_to_atlas.hh"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

struct ft_to_atlas {
  FT_Library library;
  std::vector<FT_Face> faces;
  std::string_view icons_path;
  U32 t_width, t_height, atlas_size;
  std::vector<uint8_t> font_atlas;
  std::vector<std::unordered_map<char, Character>> char_infos;
  // std::unordered_map<char, Character> regular_char_info;
} cxt;

bool load_icons(std::string_view path){
  //stbi_write_png("test_image.png",w,h,4,cxt.font_atlas.data(),w*4);
  cxt.icons_path = path;
  return true;
}

bool create_atlas(U32 texture_width, U32 texture_height, bool output_texture,
                  U32 offset);
FT_Error init_freetype(const std::vector<std::string_view> font_paths,const std::vector<uint32_t> font_sizes);

Character get_char_info(char c, U32 font_index) {
  if (font_index >= cxt.char_infos.size()) {
    std::cerr << "ERROR: faulty font index requested!" << std::endl;
  }
  return cxt.char_infos[font_index][c];
}

U32 get_text_length(std::string_view text, U32 font_index, float scale){
  U32 length = 0;
  if(font_index > cxt.char_infos.size()){
    std::cout << "invalid font_index" << std::endl;
    exit(1);
  }

  for(U32 i=0;i<text.size();i++){
    length += (cxt.char_infos[font_index][text[i]].advance >> 6 ) * scale;
  }
  //std::cout << "length : " << length << std::endl;
  return length;
}

U32 get_text_max_height(std::string_view text, U32 font_index, float scale){
  U32 height = 0;
  if(font_index > cxt.char_infos.size()){
    std::cerr << "invalid font_index" << std::endl;
    return -1;
  }

  for(U32 i=0;i<text.size();i++){
    (height < (cxt.char_infos[font_index][text[i]].char_height ) * scale)?
      height = (cxt.char_infos[font_index][text[i]].char_height ) * scale:
      height = height;
  }
  std::cout << "height : " << height << std::endl;
  return height;
}

uint8_t *create_font_atlas(U32 width, U32 height,
                           const std::vector<std::string_view> font_paths,
			   const std::vector<uint32_t> font_sizes,
                           bool output_atlas, U32 offset) {

  init_freetype(font_paths, font_sizes);
  create_atlas(width, height, output_atlas, offset);
  return cxt.font_atlas.data();
}

FT_Error init_freetype(const std::vector<std::string_view> font_paths,const std::vector<uint32_t> font_sizes) {

  FT_Error error;

  // init freetype
  error = FT_Init_FreeType(&cxt.library);
  if (error) {
      std::cout << "Freetype initialization error!" << std::endl;
    return error;
  }
  cxt.faces.resize(font_paths.size());
  // FT_Property_Set(editorCxt.library, "bsdf", "spread", &status);

  for (int i = 0; i < font_paths.size(); i++) {
    error = FT_New_Face(cxt.library, font_paths[i].data(), 0, &cxt.faces[i]);
    if (error == FT_Err_Unknown_File_Format) {
      std::cerr << ("freetype file format not recognized") << std::endl;
      return error;
    } else if (error) {
        std::cerr << ("freetype file format not recognized") << std::endl;  
      return error;
    }
  }

  for(U32 i=0;i<cxt.faces.size();i++){
    error = FT_Set_Pixel_Sizes(cxt.faces[i],0,font_sizes[i]);
    if (error) {
        std::cout << "Freetype2 glyph rendering error : " << error << std::endl;
    }
  }

  // for (auto const &f : cxt.faces) {
  //   error = FT_Set_Pixel_Sizes(f, 0, 14);
  // }

  return error;
}

FT_GlyphSlot load_font(FT_Face &face, char c) {

  FT_Error error = FT_Load_Char(face, c, FT_LOAD_RENDER);

  if (error) {
    std::cout << "Freetype2 character loading error : " << error << std::endl;
  }

  error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

  if (error) {
    std::cout << "Freetype2 glyph rendering error : " << error << std::endl;
  }

  return face->glyph;
}

bool create_atlas(U32 texture_width, U32 texture_height, bool output_texture,
                  U32 offset) {
  cxt.font_atlas.resize(texture_width * texture_height * 4);
  std::memset(cxt.font_atlas.data(), 0, texture_width * texture_height * 4);

  if(!cxt.icons_path.empty()){
    int w,h,cmp;
    const uint8_t* icons_img = stbi_load(cxt.icons_path.data(),&w,&h,&cmp,4);
    //cxt.font_atlas.resize(w * h * 4);
    std::memcpy(cxt.font_atlas.data(),icons_img,w*h*4);
  }
  
  U32 woffset = 0;
  U32 hoffset = offset, maxHeight = 0;

  cxt.char_infos.resize(cxt.faces.size());
  for (U32 j = 0; j < cxt.faces.size(); j++) {
    // for (auto c : str) {
    for (U32 i = 32; i < 127; i++) {
      auto glyphSlot = load_font(cxt.faces[j], i);
      if (glyphSlot->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY ||
          glyphSlot->bitmap.num_grays != 256) {
        std::cerr << "Unsupported pixel format!!" << std::endl;
      }

      // std::memset(textureAtlas.data(), 255, 512 * 512 * 4);
      auto width = glyphSlot->bitmap.width;
      auto height = glyphSlot->bitmap.rows;
      auto bufferSize = width * height * 4;

      // spaces have a zero buffer size
      // if (bufferSize == 0) {
      //   std::cerr << "Font buffer size is zero!!" << std::endl;
      //   continue;
      // }

      if (height > maxHeight) {
        maxHeight = height;
      }
      uint8_t *src = glyphSlot->bitmap.buffer;
      uint8_t *startOfLine = src;
      int dst = 0;

      if (woffset + width > texture_width) {
        hoffset = hoffset + maxHeight;
        woffset = 0;
      }

      cxt.char_infos[j][i] = {width,
                                         height,
                                         (U32)glyphSlot->bitmap_left,
                                         (U32)glyphSlot->bitmap_top,
                                         glyphSlot->advance.x,
                                         woffset / (float)texture_width,
                                         hoffset / (float)texture_height,
                                         width / (float)texture_width,
                                         height / (float)texture_height};

      for (U32 y = 0; y < height; ++y) {
        src = startOfLine;
        for (U32 x = 0; x < width; ++x) {
          auto value = *src;
          src++;

          cxt.font_atlas[dst++ + texture_width * hoffset * 4 + woffset * 4 +
                         y * (texture_width * 4 - width * 4)] = 0xff;
          cxt.font_atlas[dst++ + texture_width * hoffset * 4 + woffset * 4 +
                         y * (texture_width * 4 - width * 4)] = 0xff;
          cxt.font_atlas[dst++ + texture_width * hoffset * 4 + woffset * 4 +
                         y * (texture_width * 4 - width * 4)] = 0xff;
          cxt.font_atlas[dst++ + texture_width * hoffset * 4 + woffset * 4 +
                         y * (texture_width * 4 - width * 4)] = value;
        }
        startOfLine += glyphSlot->bitmap.pitch;
      }
      woffset = woffset + width;
      if (woffset > texture_width) {
        hoffset = hoffset + maxHeight;
        woffset = 0;
      }
    }
  }

  // Output texture atlas as a png
  if (output_texture) {
    U32 channels = 4;
    stbi_write_png("texture_atlas.png", texture_width, texture_height,
                   channels, cxt.font_atlas.data(), texture_width * channels);
  }

  // return texture_atlas;
  return true;
}
