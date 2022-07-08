/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997, Daniel Roux & EPSITEC SA
 * Copyright (C) 2017-2019, Mathieu Schroeter
 * http://epsitec.ch; http://www.blupi.org; http://github.com/blupi-games
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://gnu.org/licenses
 */

#include <stdio.h>
#include <stdlib.h>

#include <SDL_ttf.h>

#include "blupi.h"
#include "def.h"
#include "event.h"
#include "misc.h"
#include "pixmap.h"
#include "text.h"

class Font
{
  TTF_Font * font;
  SDL_Color  color;
  SDL_bool   outline;

public:
  Font (
    const char * name, int size, SDL_Color color, SDL_bool bold,
    SDL_bool outline, SDL_bool rtl = SDL_FALSE)
  {
    this->font    = TTF_OpenFont (name, size);
    this->color   = color;
    this->outline = outline;

    TTF_SetFontHinting (this->font, TTF_HINTING_NORMAL);
    if (bold)
      TTF_SetFontStyle (this->font, TTF_STYLE_BOLD);

    if (rtl)
    {
      //TTF_SetFontDirection(TTF_DIRECTION_RTL);
      TTF_SetDirection(5);
    }
  }

  ~Font () { TTF_CloseFont (this->font); }

  TTF_Font * GetFont () { return this->font; }

  void Draw (CPixmap * pPixmap, Point pos, const char * pText, Sint32 slope)
  {
    int           w0, h0;
    SDL_Rect      r0;
    SDL_Texture * tex0;

    const auto isRTL = IsRightReading ();
    const auto angle = isRTL ? -2.5 : 2.5;

    if (this->outline)
    {
      TTF_SetFontOutline (this->font, 1);
      SDL_Surface * text =
        TTF_RenderUTF8_Solid (this->font, pText, {0x00, 0x00, 0x00, 0});
      tex0 = SDL_CreateTextureFromSurface (g_renderer, text);
      SDL_FreeSurface (text);

      TTF_SizeUTF8 (this->font, pText, &w0, &h0);
      r0.x = pos.x;
      r0.y = pos.y;
      r0.w = w0;
      r0.h = h0;

      if (isRTL)
        r0.x -= w0;
    }

    TTF_SetFontOutline (this->font, 0);
    SDL_Surface * text =
      TTF_RenderUTF8_Blended (this->font, pText, this->color);
    SDL_Texture * tex = SDL_CreateTextureFromSurface (g_renderer, text);
    SDL_FreeSurface (text);

    TTF_SetFontOutline (this->font, 0);
    this->color.a = 64;
    SDL_Surface * text2 =
      TTF_RenderUTF8_Blended (this->font, pText, this->color);
    SDL_Texture * tex2 = SDL_CreateTextureFromSurface (g_renderer, text2);
    SDL_FreeSurface (text2);
    this->color.a = 0;

    int w, h;
    TTF_SizeUTF8 (this->font, pText, &w, &h);
    SDL_Rect r;
    r.x = pos.x + (isRTL ? -1 : 1);
    r.y = pos.y + 1;
    r.w = w;
    r.h = h;

    if (isRTL)
      r.x -= w;

    int           res;
    SDL_Texture * target;

    target = SDL_GetRenderTarget (g_renderer);

    if (this->outline)
    {
      /* outline */
      res = pPixmap->Blit (-1, tex0, r0, slope ? angle : 0, SDL_FLIP_NONE);
      SDL_DestroyTexture (tex0);
    }

    res = pPixmap->Blit (-1, tex, r, slope ? angle : 0, SDL_FLIP_NONE);
    SDL_DestroyTexture (tex);

    res = pPixmap->Blit (-1, tex2, r, slope ? angle : 0, SDL_FLIP_NONE);
    SDL_DestroyTexture (tex2);

    SDL_SetRenderTarget (g_renderer, target);
  }
};

class Fonts
{
private:
  Font * latinLittle;
  Font * latinRed;
  Font * latinSlim;
  Font * latinWhite;

  Font * hebrewLittle;
  Font * hebrewRed;
  Font * hebrewSlim;
  Font * hebrewWhite;

private:
  Font * GetFont (Sint32 font)
  {
    if (IsRightReading ())
      switch (font)
      {
      case FONTLITTLE:
        return this->hebrewLittle;
      case FONTRED:
        return this->hebrewRed;
      case FONTSLIM:
        return this->hebrewSlim;
      case FONTWHITE:
        return this->hebrewWhite;
      }

    switch (font)
    {
    case FONTLITTLE:
      return this->latinLittle;
    case FONTRED:
      return this->latinRed;
    case FONTSLIM:
      return this->latinSlim;
    case FONTWHITE:
      return this->latinWhite;
    }

    return nullptr;
  }

public:
  Fonts ()
  {
    this->latinLittle = new Font (
      "/home/schroeterm/devel/blupi/planetblupi-dev/ChakraPetch-Regular.ttf",
      12, {0xFF, 0xFF, 0x00, 0}, SDL_FALSE, SDL_TRUE);
    this->latinRed = new Font (
      "/home/schroeterm/devel/blupi/planetblupi-dev/NovaSlim-Regular.ttf", 13,
      {0xFF, 0x00, 0x00, 0}, SDL_TRUE, SDL_TRUE);
    this->latinSlim = new Font (
      "/home/schroeterm/devel/blupi/planetblupi-dev/NovaSlim-Regular.ttf", 12,
      {0xB4, 0x17, 0x12, 0}, SDL_FALSE, SDL_FALSE);
    this->latinWhite = new Font (
      "/home/schroeterm/devel/blupi/planetblupi-dev/NovaSlim-Regular.ttf", 13,
      {0xFF, 0xFF, 0xFF, 0}, SDL_TRUE, SDL_TRUE);

    this->hebrewLittle = new Font (
      "/home/schroeterm/devel/blupi/planetblupi-dev/IBMPlexSansHebrew-Regular.ttf",
      12, {0xFF, 0xFF, 0x00, 0}, SDL_FALSE, SDL_TRUE, SDL_TRUE);
    this->hebrewRed = new Font (
      "/home/schroeterm/devel/blupi/planetblupi-dev/IBMPlexSansHebrew-Regular.ttf", 13,
      {0xFF, 0x00, 0x00, 0}, SDL_TRUE, SDL_TRUE, SDL_TRUE);
    this->hebrewSlim = new Font (
      "/home/schroeterm/devel/blupi/planetblupi-dev/IBMPlexSansHebrew-Regular.ttf", 12,
      {0xB4, 0x17, 0x12, 0}, SDL_FALSE, SDL_FALSE, SDL_TRUE);
    this->hebrewWhite = new Font (
      "/home/schroeterm/devel/blupi/planetblupi-dev/IBMPlexSansHebrew-Regular.ttf", 13,
      {0xFF, 0xFF, 0xFF, 0}, SDL_TRUE, SDL_TRUE, SDL_TRUE);
  }

  ~Fonts ()
  {
    delete this->latinLittle;
    delete this->latinRed;
    delete this->latinSlim;
    delete this->latinWhite;

    delete this->hebrewLittle;
    delete this->hebrewRed;
    delete this->hebrewSlim;
    delete this->hebrewWhite;
  }

  Sint32 GetTextWidth (const char * pText, Sint32 font)
  {
    int w = 0, h = 0;
    TTF_SizeUTF8 (this->GetFont (font)->GetFont (), pText, &w, &h);
    return w;
  }

  void Draw (
    CPixmap * pPixmap, Sint32 font, Point pos, const char * pText, Sint32 slope)
  {
    this->GetFont (font)->Draw (pPixmap, pos, pText, slope);
  }
};

Fonts *
FontsInit ()
{
  static Fonts fonts;
  return &fonts;
}

/**
 * \brief Draw a text in a pixmap to a specific position.
 *
 * \param[in] pPixmap - The pixmap where it must be drawn.
 * \param[in] pos - The coordinates for the text.
 * \param[in] pText - The text.
 * \param[in] font - The font style (little or normal).
 * \param[in] slope - Text slope.
 */
void
DrawText (
  CPixmap * pPixmap, Point pos, const char * pText, Sint32 font, Sint32 slope)
{
  Sint32       rank;
  bool         isLatin    = false;
  int          numberSize = 0;
  const char * it         = nullptr;
  int          skip       = 0;
  Sint32       start      = pos.y;
  Sint32       rel        = 0;

  auto useD7  = strchr (pText, 0xD7) != nullptr;
  auto length = strlen (pText);

  //if (length >= 1 && !useD7 && IsRightReading ())
  //  pos.x -= GetTextWidth (pText, font);

  FontsInit ()->Draw (pPixmap, font, pos, pText, slope);

#if 0
  TTF_Font * _font;
  SDL_bool outline;
  SDL_Color black;
  switch (font) {
    case FONTLITTLE:
      outline = SDL_TRUE;
      black = { 0xFF, 0xFF, 0x00, 0 };
      //_font = TTF_OpenFont("/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Regular.ttf", 10);
      _font = TTF_OpenFont("/home/schroeterm/devel/blupi/planetblupi-dev/ChakraPetch-Regular.ttf", 12);
      //_font = TTF_OpenFont("/home/schroeterm/devel/blupi/planetblupi-dev/Tomorrow-Regular.ttf", 11);
      //_font = TTF_OpenFont("/home/schroeterm/devel/blupi/planetblupi-dev/NovaOval-Regular.ttf", 11);
      break;
    case FONTRED:
      outline = SDL_TRUE;
      black = { 0xFF, 0x00, 0x00, 0 };
      _font = TTF_OpenFont("/home/schroeterm/devel/blupi/planetblupi-dev/NovaSlim-Regular.ttf", 13);
      TTF_SetFontStyle(_font, TTF_STYLE_BOLD);
      //_font = TTF_OpenFont("/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf", 12);
      break;
    case FONTSLIM:
      outline = SDL_FALSE;
      black = { 0xB4, 0x17, 0x12, 0 };
      _font = TTF_OpenFont("/home/schroeterm/devel/blupi/planetblupi-dev/NovaSlim-Regular.ttf", 12);
      //TTF_SetFontStyle(_font, TTF_STYLE_BOLD);
      //_font = TTF_OpenFont("/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf", 12);
      break;
    case FONTWHITE:
      outline = SDL_TRUE;
      black = { 0xFF, 0xFF, 0xFF, 0 };
      //_font = TTF_OpenFont("/usr/share/fonts/truetype/roboto/unhinted/RobotoTTF/Roboto-Bold.ttf", 12);
      //_font = TTF_OpenFont("/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf", 13);
      _font = TTF_OpenFont("/home/schroeterm/devel/blupi/planetblupi-dev/NovaSlim-Regular.ttf", 13);
      TTF_SetFontStyle(_font, TTF_STYLE_BOLD);
      //TTF_SetFontSizeDPI(_font, 14, 64,64);
      //TTF_SetFontKerning(_font, 0);
      break;
  }

  TTF_SetFontHinting(_font, TTF_HINTING_NORMAL);

  int w0, h0;
  SDL_Rect r0;
  SDL_Texture * tex0;

  if (outline)
  {
    TTF_SetFontOutline(_font, 1);
    SDL_Surface * text = TTF_RenderUTF8_Solid(_font, pText, {0x00, 0x00, 0x00, 0});
    tex0 = SDL_CreateTextureFromSurface(g_renderer, text);
    SDL_FreeSurface(text);


    TTF_SizeUTF8(_font, pText, &w0, &h0);
    r0.x = pos.x;
    r0.y = pos.y;
    r0.w = w0;
    r0.h = h0;
  }

  TTF_SetFontOutline(_font, 0);
  SDL_Surface * text = TTF_RenderUTF8_Blended(_font, pText, black);
  SDL_Texture * tex = SDL_CreateTextureFromSurface(g_renderer, text);
  SDL_FreeSurface(text);

  TTF_SetFontOutline(_font, 0);
  black.a = 64;
  SDL_Surface * text2 = TTF_RenderUTF8_Blended(_font, pText, black);
  SDL_Texture * tex2 = SDL_CreateTextureFromSurface(g_renderer, text2);
  SDL_FreeSurface(text2);

  int w, h;
  TTF_SizeUTF8(_font, pText, &w, &h);
  SDL_Rect r;
  r.x = pos.x + 1;
  r.y = pos.y + 1;
  r.w = w;
  r.h = h;

  TTF_CloseFont (_font);

  int res;
  SDL_Texture * target;

  if (outline) {
    /* outline */
    target = SDL_GetRenderTarget (g_renderer);
    SDL_SetRenderTarget (g_renderer, target);
    //res = SDL_RenderCopy (g_renderer, tex0, nullptr, &r0);
    SDL_Point pt = {0, 0};
    if (slope)
      res = SDL_RenderCopyEx(g_renderer, tex0, nullptr, &r0, 2.5, &pt, SDL_FLIP_NONE);
    else
      res = SDL_RenderCopy (g_renderer, tex0, nullptr, &r0);
    //SDL_SetRenderTarget (g_renderer, target);
    SDL_DestroyTexture(tex0);
  }

  SDL_Point pt = {0, 0};

  target = SDL_GetRenderTarget (g_renderer);
  SDL_SetRenderTarget (g_renderer, target);
  //res = SDL_RenderCopy (g_renderer, tex, nullptr, &r);
  if (slope)
    res = SDL_RenderCopyEx(g_renderer, tex, nullptr, &r, 2.5, &pt, SDL_FLIP_NONE);
  else
    res = SDL_RenderCopy (g_renderer, tex, nullptr, &r);
  //SDL_SetRenderTarget (g_renderer, target);
  SDL_DestroyTexture(tex);

  target = SDL_GetRenderTarget (g_renderer);
  SDL_SetRenderTarget (g_renderer, target);
  //res = SDL_RenderCopy (g_renderer, tex, nullptr, &r);
  if (slope)
    res = SDL_RenderCopyEx(g_renderer, tex2, nullptr, &r, 2.5, &pt, SDL_FLIP_NONE);
  else
    res = SDL_RenderCopy (g_renderer, tex2, nullptr, &r);
  //SDL_SetRenderTarget (g_renderer, target);
  SDL_DestroyTexture(tex2);
#endif // 0

#if 0
  while (*pText != '\0' || skip)
  {
    if (isRightReading && numberSize == 0)
    {
      const auto test = [] (const char * text) -> bool {
        return *text > ' ' && *text <= '~';
      };
      it      = pText;
      isLatin = test (pText);
      if (isLatin)
      {
        while (test (pText))
          ++pText;

        numberSize = pText - it;
        skip       = numberSize - 1;
      }
    }

    if (numberSize)
    {
      pText = it + numberSize - 1;
      numberSize--;
    }

    rank = GetOffset (pText);

    if (isRightReading)
    {
      if (rank == '(')
        rank = ')';
      else if (rank == ')')
        rank = '(';
    }

    auto inc = rank > 127;
    auto lg  = GetCharWidth (pText, font);

    if (isRightReading)
      pos.x += -lg;

    rel += lg;
    if (slope)
      pos.y = start + rel / slope;

    if (font != FONTLITTLE)
    {
      rank += 256 * font;
      pPixmap->DrawIcon (-1, CHTEXT, rank, pos);
    }
    else
      pPixmap->DrawIcon (-1, CHLITTLE, rank, pos);

    if (!isRightReading)
      pos.x += lg;

    if (!numberSize && skip > 0)
    {
      pText += skip;
      skip = 0;
    }

    if (inc)
      pText++;
    pText++;
  }
#endif // 0
}

// Affiche un pavé de texte.
// Une ligne vide est affichée avec un demi interligne !
// Si part != -1, n'affiche que les lignes qui commencent
// par "n|", avec n=part.

void
DrawTextRect (
  CPixmap * pPixmap, Point pos, char * pText, Sint32 pente, Sint32 font,
  Sint32 part)
{
  char   text[100];
  char * pDest;
  Sint32 itl;

  if (font == FONTLITTLE)
    itl = DIMLITTLEY - 2;
  else
    itl = DIMTEXTY;

  while (*pText != 0)
  {
    pDest = text;
    while (*pText != 0 && *pText != '\r' && *pText != '\n')
      *pDest++ = *pText++;
    *pDest = 0;
    if (*pText == '\r')
      pText++; // saute '\r'
    if (*pText == '\n')
      pText++; // saute '\n'

    pDest = text;
    if (text[0] != 0 && text[1] == '|') // commence par "n|" ?
    {
      if (part != -1 && part != text[0] - '0')
        continue;
      pDest += 2; // saute "n|"
    }
    else
    {
      if (part != -1)
        continue;
    }

    DrawText (pPixmap, pos, pDest, font, pente);

    if (pDest[0] == 0) // ligne vide ?
    {
      pos.y += itl / 2; // descend de 1/2 ligne
    }
    else
    {
      pos.y += itl; // passe à la ligne suivante
    }
  }
}

// Affiche un texte centré pouvant éventuellement
// contenir plusieurs lignes séparées par des '\n'.

void
DrawTextCenter (CPixmap * pPixmap, Point pos, const char * pText, Sint32 font)
{
  char   text[100];
  char * pDest;
  Sint32 itl;
  Point  start;
  auto   isRightReading = IsRightReading ();

  if (font == FONTLITTLE)
    itl = DIMLITTLEY - 2;
  else
    itl = DIMTEXTY;

  while (*pText != 0)
  {
    pDest = text;
    while (*pText != 0 && *pText != '\r' && *pText != '\n')
      *pDest++ = *pText++;
    *pDest = 0;
    if (*pText == '\r')
      pText++; // saute '\r'
    if (*pText == '\n')
      pText++; // saute '\n'

    pDest = text;
    start.x =
      pos.x +
      (isRightReading ? GetTextWidth (pDest) : -GetTextWidth (pDest)) / 2;
    start.y = pos.y;
    DrawText (pPixmap, start, pDest, font);

    if (pDest[0] == 0) // ligne vide ?
    {
      pos.y += itl / 2; // descend de 1/2 ligne
    }
    else
    {
      pos.y += itl; // passe à la ligne suivante
    }
  }
}

// Retourne la hauteur d'un texte.

Sint32
GetTextHeight (char * pText, Sint32 font, Sint32 part)
{
  char   text[100];
  char * pDest;
  Sint32 itl;
  Sint32 h = 0;

  if (font == FONTLITTLE)
    itl = DIMLITTLEY - 2;
  else
    itl = DIMTEXTY;

  while (*pText != 0)
  {
    pDest = text;
    while (*pText != 0 && *pText != '\r' && *pText != '\n')
      *pDest++ = *pText++;
    *pDest = 0;
    if (*pText == '\r')
      pText++; // saute '\r'
    if (*pText == '\n')
      pText++; // saute '\n'

    pDest = text;
    if (text[0] != 0 && text[1] == '|') // commence par "n|" ?
    {
      if (part != -1 && part != text[0] - '0')
        continue;
      pDest += 2; // saute "n|"
    }
    else
    {
      if (part != -1)
        continue;
    }

    if (pDest[0] == 0) // ligne vide ?
    {
      h += itl / 2; // descend de 1/2 ligne
    }
    else
    {
      h += itl; // passe à la ligne suivante
    }
  }

  return h;
}

// Retourne la longueur d'un texte.

Sint32
GetTextWidth (const char * pText, Sint32 font)
{
#if 0
  while (*pText != 0)
  {
    auto rank = GetOffset (pText);
    auto inc  = rank > 127;

    width += GetCharWidth (pText, font);

    if (inc)
      pText++;
    pText++;
  }
#endif // 0

  return FontsInit ()->GetTextWidth (pText, font);
}

// Retourne la longueur d'un grand chiffre.

void
GetBignumInfo (Sint32 num, Sint32 & start, Sint32 & lg)
{
  static Sint32 table[11] = {0, 53, 87, 133, 164, 217, 253, 297, 340, 382, 426};

  start = table[num];
  lg    = table[num + 1] - table[num];
}

// Affiche un grand nombre.

void
DrawBignum (CPixmap * pPixmap, Point pos, Sint32 num)
{
  char   string[10];
  Sint32 i = 0;
  Sint32 start, lg;
  Rect   rect;

  snprintf (string, sizeof (string), "%d", num);

  rect.top    = 0;
  rect.bottom = 52;
  while (string[i] != 0)
  {
    GetBignumInfo (string[i] - '0', start, lg);

    rect.left  = start;
    rect.right = start + lg;
    pPixmap->DrawPart (-1, CHBIGNUM, pos, rect);
    pos.x += lg + 4;

    i++;
  }
}

// Retourne la longueur d'un grand nombre.

Sint32
GetBignumWidth (Sint32 num)
{
  char   string[10];
  Sint32 i = 0;
  Sint32 start, lg;
  Sint32 width = -4;

  snprintf (string, sizeof (string), "%d", num);

  while (string[i] != 0)
  {
    GetBignumInfo (string[i] - '0', start, lg);
    width += lg + 4;
    i++;
  }

  return width;
}
