///////////////////////////////////////////////////////////////////////////////
// Name:        pdffont.h
// Purpose:     
// Author:      Ulrich Telle
// Modified by:
// Created:     2005-08-04
// Copyright:   (c) Ulrich Telle
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/// \file pdffont.h Interface of the wxPdfFont class

#ifndef _PDFFONT_H_
#define _PDFFONT_H_

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "pdffont.h"
#endif

#include "wx/strconv.h"
#include "wx/xml/xml.h"

#include "wx/pdfdocdef.h"

/// Class representing a font description. (For internal use only)
class WXDLLIMPEXP_PDFDOC wxPdfFontDescription
{
public:
  /// Default constructor
  wxPdfFontDescription();

  /// Default destructor
  ~wxPdfFontDescription();

  /// Set ascender
  void SetAscent(int ascent) { m_ascent = ascent; };

  /// Get ascender
  int  GetAscent() const { return m_ascent; };
  
  /// Set descender
  void SetDescent(int descent) { m_descent = descent;};

  /// Get descender
  int  GetDescent() const { return m_descent;};
  
  /// Set CapHeight
  void SetCapHeight(int capHeight) { m_capHeight = capHeight; };

  /// Get CapHeight
  int  GetCapHeight() const { return m_capHeight; };
  
  /// Set font flags
  void SetFlags(int flags) { m_flags = flags; };

  /// Get font flags
  int  GetFlags() const { return m_flags; };
  
  /// Set font bounding box
  void SetFontBBox(const wxString& fontBBox) { m_fontBBox = fontBBox; };

  /// Get font bounding box
  wxString GetFontBBox() const { return m_fontBBox; };
  
  /// Set italic angle
  void SetItalicAngle(int italicAngle) { m_italicAngle = italicAngle; };

  /// Get italic angle
  int  GetItalicAngle() const { return m_italicAngle; };
  
  /// Set StemV
  void SetStemV(int stemV) { m_stemV = stemV; };

  /// Get StemV
  int  GetStemV() const { return m_stemV; };

  /// Set missing character width
  void SetMissingWidth(int missingWidth) { m_missingWidth = missingWidth; };

  /// Get missing character width
  int  GetMissingWidth() const { return m_missingWidth; };

private:
  int      m_ascent;        ///< Ascender
  int      m_descent;       ///< Descender
  int      m_capHeight;     ///< CapHeight
  int      m_flags;         ///< Font flags
  wxString m_fontBBox;      ///< Font bounding box
  int      m_italicAngle;   ///< Angle for italics
  int      m_stemV;         ///< StemV
  int      m_missingWidth;  ///< Missing character width
};

WX_DECLARE_HASH_MAP(long, short, wxIntegerHash, wxIntegerEqual, CharWidthMap);

/// Base class for all fonts and for representing the core fonts. (For internal use only)
class WXDLLIMPEXP_PDFDOC wxPdfFont
{
public:
  /// Font constructor
  wxPdfFont(int index, const wxString& name = wxEmptyString, short* cwArray = NULL);

  /// Default destructor
  virtual ~wxPdfFont();

  /// Set font index
  void SetIndex(int index) { m_index = index; }

  /// Get font index
  int  GetIndex() { return m_index; }

  /// Set font object index
  void SetObjIndex(int n) { m_n = n; }

  /// Get font object index
  int  GetObjIndex() { return m_n; }

  /// Set font file index
  void SetFileIndex(int fn) { m_fn = fn; }

  /// Get font file index
  int  GetFileIndex() { return m_fn; }

  /// Set encoding differences index
  void SetDiffIndex(int ndiff) { m_ndiff = ndiff; }

  /// Get encoding differences index
  int  GetDiffIndex() { return m_ndiff; }

  /// Set font type
  void SetType(const wxString& type) { m_type = type; }

  /// Get font type
  wxString GetType() { return m_type; }

  /// Set font name
  void SetName(const wxString& name) { m_name = name; }

  /// Get font name
  wxString GetName() { return m_name; }

  /// Set underline position
  void SetUnderlinePosition(int up) { m_up = up; }

  /// Get underline position
  int  GetUnderlinePosition() { return m_up; }

  /// Set underline thickness
  void SetUnderlineThickness(int ut) { m_ut = ut; }

  /// Get underline thickness
  int  GetUnderlineThickness() { return m_ut; }

  /// Set encoding
  void SetEncoding(const wxString& enc) { m_enc = enc; }

  /// Get encoding
  wxString GetEncoding() { return m_enc; }

  /// Check whether the font has differences to WinAnsi encoding
  bool HasDiffs() { return m_diffs.Length() > 0; }

  /// Set encoding differences
  void SetDiffs(const wxString& diffs) { m_diffs = diffs; }

  /// Get encoding differences
  wxString GetDiffs() { return m_diffs; }

  /// Check whether the font has an associated font file
  bool HasFile() { return m_file.Length() > 0; }

  /// Get the name of the associated font file
  wxString GetFontFile() { return m_file; }

  /// Get the name of the Character-to-Glyph mapping file
  wxString GetCtgFile() { return m_ctg; }

  /// Get font file size 1
  int  GetSize1() { return m_size1; }

  /// Check whether the file has a size 2
  bool HasSize2() { return m_size2 > 0; }

  /// Get font file size 2 (Type 1 only)
  int  GetSize2() { return m_size2; }

  /// Get the font's CMap (Type 0 only)
  wxString GetCMap() { return m_cmap; }

  /// Get font ordering
  wxString GetOrdering() { return m_ordering; }

  /// Get font supplement /Type 0 only)
  wxString GetSupplement() { return m_supplement; }

  /// Get the character width array as string
  virtual wxString GetWidthsAsString();
  
  /// Get the width of a string
  virtual double GetStringWidth(const wxString& s);

  /// Set the font description
  virtual void SetDesc(const wxPdfFontDescription& desc) { m_desc = desc; }

  /// Get the font description
  virtual const wxPdfFontDescription& GetDesc() const { return m_desc; }

  /// Load the font metrics XML file
  virtual bool LoadFontMetrics(wxXmlNode* root) { return false; };

#if wxUSE_UNICODE
  /// Get the associated encoding converter
  virtual wxMBConv* GetEncodingConv() { return GetWinEncodingConv(); }
#endif

  /// Get the default WinAnsi encoding converter
  static wxMBConv* GetWinEncodingConv();

protected:
  int                  m_index; ///< Index number of this font
  int                  m_n;     ///< Font object index
  int                  m_fn;    ///< Font file index
  int                  m_ndiff; ///< Index of encoding differences object

  wxString             m_type;  ///< Font type
  wxString             m_name;  ///< Font name
  
  int                  m_up;    ///< Underline position
  int                  m_ut;    ///< Underline thickness

  CharWidthMap*        m_cw;    ///< Array with character widths

  wxPdfFontDescription m_desc;  ///< Font description

  wxString             m_enc;   ///< Encoding
  wxString             m_diffs; ///< Encoding differences

  wxString             m_file;  ///< Filename of font program
  wxString             m_ctg;   ///< Filename of char to glyph mapping
  int                  m_size1; ///< TrueType file size or Type1 file size 1
  int                  m_size2; ///< Type1 file size 2

  wxString             m_cmap;        ///< CMap of a CID font
  wxString             m_ordering;    ///< Ordering of a CID font 
  wxString             m_supplement;  ///< Supplement of a CID font

  static wxMBConv*     ms_winEncoding;  ///< WinAnsi converter
};

/// Class representing TrueType fonts. (For internal use only)
class WXDLLIMPEXP_PDFDOC wxPdfFontTrueType : public wxPdfFont
{
public:
  ///< Constructor
  wxPdfFontTrueType(int index);

  /// Default destructor
  virtual ~wxPdfFontTrueType();

  /// Get the width of a string
  virtual double GetStringWidth(const wxString& s);

  /// Load the font metrics XML file
  virtual bool LoadFontMetrics(wxXmlNode* root);

#if wxUSE_UNICODE
  /// Get the associated encoding converter
  virtual wxMBConv* GetEncodingConv() { return m_conv; }
#endif

protected:
  wxMBConv* m_conv;   ///< Assocated encoding converter
};

#if wxUSE_UNICODE

/// Class representing Unicode TrueType fonts. (For internal use only)
class WXDLLIMPEXP_PDFDOC wxPdfFontTrueTypeUnicode : public wxPdfFont
{
public:
  ///< Constructor
  wxPdfFontTrueTypeUnicode(int index);

  /// Default destructor
  virtual ~wxPdfFontTrueTypeUnicode();

  /// Get the character width array as string
  virtual wxString GetWidthsAsString();

  /// Get the width of a string
  virtual double GetStringWidth(const wxString& s);

  /// Load the font metrics XML file
  virtual bool LoadFontMetrics(wxXmlNode* root);

  /// Get the associated encoding converter
  virtual wxMBConv* GetEncodingConv() { return m_conv; }

protected:
  wxMBConv* m_conv;   ///< Assocated encoding converter
};

#endif // wxUSE_UNICODE

/// Class representing Type 1 fonts. (For internal use only)
class WXDLLIMPEXP_PDFDOC wxPdfFontType1 : public wxPdfFont
{
public:
  ///< Constructor
  wxPdfFontType1(int index);

  /// Default destructor
  virtual ~wxPdfFontType1();

  /// Get the width of a string
  virtual double GetStringWidth(const wxString& s);

  /// Load the font metrics XML file
  virtual bool LoadFontMetrics(wxXmlNode* root);

#if wxUSE_UNICODE
  /// Get the associated encoding converter
  virtual wxMBConv* GetEncodingConv() { return m_conv; }
#endif

protected:
  wxMBConv* m_conv;   ///< Assocated encoding converter
};

#if wxUSE_UNICODE

/// Class representing Type 0 fonts. (For internal use only)
class WXDLLIMPEXP_PDFDOC wxPdfFontType0 : public wxPdfFont
{
public:
  ///< Constructor
  wxPdfFontType0(int index);

  /// Default destructor
  virtual ~wxPdfFontType0();

  /// Get the character width array as string
  virtual wxString GetWidthsAsString();

  /// Get the width of a string
  virtual double GetStringWidth(const wxString& s);

  /// Load the font metrics XML file
  virtual bool LoadFontMetrics(wxXmlNode* root);

  /// Get the associated encoding converter
  virtual wxMBConv* GetEncodingConv() { return m_conv; }

protected:
  bool HasHalfWidthRange() { return m_hwRange; }
  wxChar HalfWidthRangeFirst() { return m_hwFirst; }
  wxChar HalfWidthRangeLast() { return m_hwLast; }

  bool      m_hwRange;  ///< Flag whether the font has a half width range
  wxChar    m_hwFirst;  ///< CID of the first half width character
  wxChar    m_hwLast;   ///< CID of the last half width character

  wxMBConv* m_conv;     ///< Assocated encoding converter
};

#endif // wxUSE_UNICODE

#endif

