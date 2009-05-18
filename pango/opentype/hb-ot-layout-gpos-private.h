/*
 * Copyright (C) 2007,2008,2009  Red Hat, Inc.
 *
 *  This is part of HarfBuzz, an OpenType Layout engine library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Red Hat Author(s): Behdad Esfahbod
 */

#ifndef HB_OT_LAYOUT_GPOS_PRIVATE_H
#define HB_OT_LAYOUT_GPOS_PRIVATE_H

#include "hb-ot-layout-gsubgpos-private.h"

/* XXX */
#include "harfbuzz-impl.h"


/* Shared Tables: ValueRecord, Anchor Table, and MarkArray */

typedef SHORT Value;
typedef Value ValueRecord[];

#if 0
struct ValueRecord {
  /* TODO */

  private:
  SHORT		xPlacement;		/* Horizontal adjustment for
					 * placement--in design units */
  SHORT		yPlacement;		/* Vertical adjustment for
					 * placement--in design units */
  SHORT		xAdvance;		/* Horizontal adjustment for
					 * advance--in design units (only used
					 * for horizontal writing) */
  SHORT		yAdvance;		/* Vertical adjustment for advance--in
					 * design units (only used for vertical
					 * writing) */
  Offset	xPlaDevice;		/* Offset to Device table for
					 * horizontal placement--measured from
					 * beginning of PosTable (may be NULL) */
  Offset	yPlaDevice;		/* Offset to Device table for vertical
					 * placement--measured from beginning
					 * of PosTable (may be NULL) */
  Offset	xAdvDevice;		/* Offset to Device table for
					 * horizontal advance--measured from
					 * beginning of PosTable (may be NULL) */
  Offset	yAdvDevice;		/* Offset to Device table for vertical
					 * advance--measured from beginning of
					 * PosTable (may be NULL) */
};
ASSERT_SIZE (ValueRecord, 16);

struct ValueFormat {
  /* TODO */

  private:
  0x0001	xPlacement;		/* Includes horizontal adjustment
					 * for placement */
  0x0002	yPlacement;		/* Includes vertical adjustment for
					 * placement */
  0x0004	xAdvance;		/* Includes horizontal adjustment
					 * for advance */
  0x0008	yAdvance;		/* Includes vertical adjustment for
					 * advance */
  0x0010	xPlaDevice;		/* Includes horizontal Device table
					 * for placement */
  0x0020	yPlaDevice;		/* Includes vertical Device table
					 * for placement */
  0x0040	xAdvDevice;		/* Includes horizontal Device table
					 * for advance */
  0x0080	yAdvDevice;		/* Includes vertical Device table
					 * for advance */
  0xF000	reserved;		/* For future use */
};
ASSERT_SIZE (ValueFormat, 18);
#endif


struct AnchorFormat1 {

  friend struct Anchor;

  private:
  inline void get_anchor (hb_ot_layout_t *layout, hb_codepoint_t glyph_id,
			  hb_position_t *x, hb_position_t *y) const {
      *x = layout->gpos_info.x_scale * xCoordinate / 0x10000;
      *y = layout->gpos_info.y_scale * yCoordinate / 0x10000;
  }

  private:
  USHORT	format;			/* Format identifier--format = 1 */
  SHORT		xCoordinate;		/* Horizontal value--in design units */
  SHORT		yCoordinate;		/* Vertical value--in design units */
};
ASSERT_SIZE (AnchorFormat1, 6);

struct AnchorFormat2 {

  friend struct Anchor;

  private:
  inline void get_anchor (hb_ot_layout_t *layout, hb_codepoint_t glyph_id,
			  hb_position_t *x, hb_position_t *y) const {
      /* TODO Contour */
      *x = layout->gpos_info.x_scale * xCoordinate / 0x10000;
      *y = layout->gpos_info.y_scale * yCoordinate / 0x10000;
  }

  private:
  USHORT	format;			/* Format identifier--format = 2 */
  SHORT		xCoordinate;		/* Horizontal value--in design units */
  SHORT		yCoordinate;		/* Vertical value--in design units */
  USHORT	anchorPoint;		/* Index to glyph contour point */
};
ASSERT_SIZE (AnchorFormat2, 8);

struct AnchorFormat3 {

  friend struct Anchor;

  private:
  inline void get_anchor (hb_ot_layout_t *layout, hb_codepoint_t glyph_id,
			  hb_position_t *x, hb_position_t *y) const {
      /* TODO Device */
      *x += layout->gpos_info.x_scale * xCoordinate / 0x10000;
      *y += layout->gpos_info.y_scale * yCoordinate / 0x10000;
  }

  private:
  USHORT	format;			/* Format identifier--format = 3 */
  SHORT		xCoordinate;		/* Horizontal value--in design units */
  SHORT		yCoordinate;		/* Vertical value--in design units */
  OffsetTo<Device>
		xDeviceTable;		/* Offset to Device table for X
					 * coordinate-- from beginning of
					 * Anchor table (may be NULL) */
  OffsetTo<Device>
		yDeviceTable;		/* Offset to Device table for Y
					 * coordinate-- from beginning of
					 * Anchor table (may be NULL) */
};
ASSERT_SIZE (AnchorFormat3, 10);

struct Anchor {

  inline void get_anchor (hb_ot_layout_t *layout, hb_codepoint_t glyph_id,
			  hb_position_t *x, hb_position_t *y) const {
    *x = *y = 0;
    switch (u.format) {
    case 1: u.format1->get_anchor (layout, glyph_id, x, y); return;
    case 2: u.format2->get_anchor (layout, glyph_id, x, y); return;
    case 3: u.format3->get_anchor (layout, glyph_id, x, y); return;
    default:						    return;
    }
  }

  private:
  union {
  USHORT		format;		/* Format identifier */
  AnchorFormat1		format1[];
  AnchorFormat2		format2[];
  AnchorFormat3		format3[];
  } u;
};
ASSERT_SIZE (Anchor, 2);


struct MarkRecord {
  /* TODO */

  private:
  USHORT	klass;			/* Class defined for this mark */
  OffsetTo<Anchor>
		markAnchor;		/* Offset to Anchor table--from
					 * beginning of MarkArray table */
};
ASSERT_SIZE (MarkRecord, 4);

struct MarkArray {
  /* TODO */

  private:
  ArrayOf<MarkRecord>
		markRecord;	/* Array of MarkRecords--in Coverage order */
};
ASSERT_SIZE (MarkArray, 2);


/* Lookups */

struct SinglePosFormat1 {

  friend struct SinglePos;

  private:
  inline bool apply (LOOKUP_ARGS_DEF) const {
    /* TODO */
    return false;
  }

  private:
  USHORT	format;			/* Format identifier--format = 1 */
  OffsetTo<Coverage>
		coverage;		/* Offset to Coverage table--from
					 * beginning of subtable */
  USHORT	valueFormat;		/* Defines the types of data in the
					 * ValueRecord */
  ValueRecord	values;			/* Defines positioning
					 * value(s)--applied to all glyphs in
					 * the Coverage table */
};
ASSERT_SIZE (SinglePosFormat1, 6);

struct SinglePosFormat2 {

  friend struct SinglePos;

  private:
  inline bool apply (LOOKUP_ARGS_DEF) const {
    /* TODO */
    return false;
  }

  private:
  USHORT	format;			/* Format identifier--format = 2 */
  OffsetTo<Coverage>
		coverage;		/* Offset to Coverage table--from
					 * beginning of subtable */
  USHORT	valueFormat;		/* Defines the types of data in the
					 * ValueRecord */
  USHORT	valueCount;		/* Number of ValueRecords */
  ValueRecord	values;			/* Array of ValueRecords--positioning
					 * values applied to glyphs */
};
ASSERT_SIZE (SinglePosFormat2, 8);

struct SinglePos {

  friend struct PosLookupSubTable;

  private:

  inline bool apply (LOOKUP_ARGS_DEF) const {
    switch (u.format) {
    case 1: return u.format1->apply (LOOKUP_ARGS);
    case 2: return u.format2->apply (LOOKUP_ARGS);
    default:return false;
    }
  }

  private:
  union {
  USHORT		format;		/* Format identifier */
  SinglePosFormat1	format1[];
  SinglePosFormat2	format2[];
  } u;
};
ASSERT_SIZE (SinglePos, 2);


struct PairValueRecord {
  /* TODO */

  private:
  GlyphID	secondGlyph;		/* GlyphID of second glyph in the
					 * pair--first glyph is listed in the
					 * Coverage table */
  ValueRecord	values;			/* Positioning data for the first glyph
					 * followed by for second glyph */
};
ASSERT_SIZE (PairValueRecord, 2);

/* XXXXXXXXXXXXXXXXXXXX */
typedef ArrayOf<PairValueRecord> PairSet;
ASSERT_SIZE (PairSet, 2);

struct PairPosFormat1 {

  friend struct PairPos;

  private:
  inline bool apply (LOOKUP_ARGS_DEF) const {
    /* TODO */
    return false;
  }

  private:
  USHORT	format;			/* Format identifier--format = 1 */
  OffsetTo<Coverage>
		coverage;		/* Offset to Coverage table--from
					 * beginning of subtable */
  USHORT	valueFormat1;		/* Defines the types of data in
					 * ValueRecord1--for the first glyph
					 * in the pair--may be zero (0) */
  USHORT	valueFormat2;		/* Defines the types of data in
					 * ValueRecord2--for the second glyph
					 * in the pair--may be zero (0) */
  OffsetArrayOf<PairSet>
		pairSet;		/* Array of PairSet tables
					 * ordered by Coverage Index */
};
ASSERT_SIZE (PairPosFormat1, 10);

struct PairPosFormat2 {

  friend struct PairPos;

  private:
  inline bool apply (LOOKUP_ARGS_DEF) const {
    /* TODO */
    return false;
  }

  private:
  USHORT	format;			/* Format identifier--format = 2 */
  OffsetTo<Coverage>
		coverage;		/* Offset to Coverage table--from
					 * beginning of subtable */
  USHORT	valueFormat1;		/* ValueRecord definition--for the
					 * first glyph of the pair--may be zero
					 * (0) */
  USHORT	valueFormat2;		/* ValueRecord definition--for the
					 * second glyph of the pair--may be
					 * zero (0) */
  OffsetTo<ClassDef>
		classDef1;		/* Offset to ClassDef table--from
					 * beginning of PairPos subtable--for
					 * the first glyph of the pair */
  OffsetTo<ClassDef>
		classDef2;		/* Offset to ClassDef table--from
					 * beginning of PairPos subtable--for
					 * the second glyph of the pair */
  USHORT	class1Count;		/* Number of classes in ClassDef1
					 * table--includes Class0 */
  USHORT	class2Count;		/* Number of classes in ClassDef2
					 * table--includes Class0 */
  ValueRecord	values;			/* Matrix of value pairs:
					 * class1-major, class2-minor,
					 * Each entry has value1 and value2 */
};
ASSERT_SIZE (PairPosFormat2, 16);

struct PairPos {

  friend struct PosLookupSubTable;

  private:

  inline bool apply (LOOKUP_ARGS_DEF) const {
    switch (u.format) {
    case 1: return u.format1->apply (LOOKUP_ARGS);
    case 2: return u.format2->apply (LOOKUP_ARGS);
    default:return false;
    }
  }

  private:
  union {
  USHORT		format;		/* Format identifier */
  PairPosFormat1	format1[];
  PairPosFormat2	format2[];
  } u;
};
ASSERT_SIZE (PairPos, 2);


struct EntryExitRecord {
  /* TODO */

  private:
  OffsetTo<Anchor>
		entryAnchor;		/* Offset to EntryAnchor table--from
					 * beginning of CursivePos
					 * subtable--may be NULL */
  OffsetTo<Anchor>
		exitAnchor;		/* Offset to ExitAnchor table--from
					 * beginning of CursivePos
					 * subtable--may be NULL */
};
ASSERT_SIZE (EntryExitRecord, 4);

struct CursivePosFormat1 {

  friend struct CursivePos;

  private:
  inline bool apply (LOOKUP_ARGS_DEF) const {
    /* TODO */
    return false;
  }

  private:
  USHORT	format;			/* Format identifier--format = 1 */
  OffsetTo<Coverage>
		coverage;		/* Offset to Coverage table--from
					 * beginning of subtable */
  ArrayOf<EntryExitRecord>
		entryExitRecord;	/* Array of EntryExit records--in
					 * Coverage Index order */
};
ASSERT_SIZE (CursivePosFormat1, 6);

struct CursivePos {

  friend struct PosLookupSubTable;

  private:

  inline bool apply (LOOKUP_ARGS_DEF) const {
    switch (u.format) {
    case 1: return u.format1->apply (LOOKUP_ARGS);
    default:return false;
    }
  }

  private:
  union {
  USHORT		format;		/* Format identifier */
  CursivePosFormat1	format1[];
  } u;
};
ASSERT_SIZE (CursivePos, 2);


struct BaseRecord {
  /* TODO */

  private:
  Offset	baseAnchor[];		/* Array of offsets (one per class)
					 * to Anchor tables--from beginning
					 * of BaseArray table--ordered by
					 * class--zero--based */
};
ASSERT_SIZE (BaseRecord, 0);

struct BaseArray {
  /* TODO */

  private:
  USHORT	baseCount;		/* Number of BaseRecords */
  BaseRecord	baseRecord[];		/* Array of BaseRecords--in order of
					 * BaseCoverage Index */
};
ASSERT_SIZE (BaseArray, 2);

struct MarkBasePosFormat1 {

  friend struct MarkBasePos;

  private:
  inline bool apply (LOOKUP_ARGS_DEF) const {
    /* TODO */
    return false;
  }

  private:
  USHORT	format;			/* Format identifier--format = 1 */
  Offset	markCoverage;		/* Offset to MarkCoverage table--from
					 * beginning of MarkBasePos subtable */
  Offset	baseCoverage;		/* Offset to BaseCoverage table--from
					 * beginning of MarkBasePos subtable */
  USHORT	classCount;		/* Number of classes defined for marks */
  Offset	markArray;		/* Offset to MarkArray table--from
					 * beginning of MarkBasePos subtable */
  /* XXXXXXXXXXXXX */
  Offset	baseArray;		/* Offset to BaseArray table--from
					 * beginning of MarkBasePos subtable */
};
ASSERT_SIZE (MarkBasePosFormat1, 12);

struct MarkBasePos {

  friend struct PosLookupSubTable;

  private:

  inline bool apply (LOOKUP_ARGS_DEF) const {
    switch (u.format) {
    case 1: return u.format1->apply (LOOKUP_ARGS);
    default:return false;
    }
  }

  private:
  union {
  USHORT		format;		/* Format identifier */
  MarkBasePosFormat1	format1[];
  } u;
};
ASSERT_SIZE (MarkBasePos, 2);


struct ComponentRecord {
  /* TODO */

  private:
  OffsetTo<Anchor>
		ligatureAnchor[];	/* Array of offsets (one per class)
					 * to Anchor tables--from beginning
					 * of LigatureAttach table--ordered
					 * by class--NULL if a component
					 * does not have an attachment for a
					 * class--zero--based array */
};
ASSERT_SIZE (ComponentRecord, 0);

struct LigatureAttach {
  /* TODO */

  private:
    /* XXXXXXXXXXXXX */
  USHORT	componentCount;		/* Number of ComponentRecords in this
					 * ligature */
  ComponentRecord
		componentRecord[];	/* Array of ComponentRecords--ordered
					 * in writing direction */
};
ASSERT_SIZE (LigatureAttach, 2);

struct LigatureArray {
  /* TODO */

  private:
  OffsetArrayOf<LigatureAttach>
		ligatureAttach;		/* Array of LigatureAttach
					 * tables ordered by
					 * LigatureCoverage Index */
};
ASSERT_SIZE (LigatureArray, 2);

struct MarkLigPosFormat1 {

  friend struct MarkLigPos;

  private:
  inline bool apply (LOOKUP_ARGS_DEF) const {
    /* TODO */
    return false;
  }

  private:
  USHORT	format;			/* Format identifier--format = 1 */
  Offset	markCoverage;		/* Offset to Mark Coverage table--from
					 * beginning of MarkLigPos subtable */
  Offset	ligatureCoverage;	/* Offset to Ligature Coverage
					 * table--from beginning of MarkLigPos
					 * subtable */
  USHORT	classCount;		/* Number of defined mark classes */
  Offset	markArray;		/* Offset to MarkArray table--from
					 * beginning of MarkLigPos subtable */
  Offset	ligatureArray;		/* Offset to LigatureArray table--from
					 * beginning of MarkLigPos subtable */
};
ASSERT_SIZE (MarkLigPosFormat1, 12);

struct MarkLigPos {

  friend struct PosLookupSubTable;

  private:

  inline bool apply (LOOKUP_ARGS_DEF) const {
    switch (u.format) {
    case 1: return u.format1->apply (LOOKUP_ARGS);
    default:return false;
    }
  }

  private:
  union {
  USHORT		format;		/* Format identifier */
  MarkLigPosFormat1	format1[];
  } u;
};
ASSERT_SIZE (MarkLigPos, 2);


struct Mark2Record {
  /* TODO */

  private:
  OffsetTo<Anchor>
		mark2Anchor[];		/* Array of offsets (one per class)
					 * to Anchor tables--from beginning of
					 * Mark2Array table--zero--based array */
};

struct Mark2Array {
  /* TODO */

  private:
  USHORT	mark2Count;		/* Number of Mark2 records */
  Mark2Record	mark2Record[];		/* Array of Mark2Records--in Coverage
					 * order */
};
ASSERT_SIZE (Mark2Array, 2);

struct MarkMarkPosFormat1 {

  friend struct MarkMarkPos;

  private:
  inline bool apply (LOOKUP_ARGS_DEF) const {
    /* TODO */
    return false;
  }

  private:
  USHORT	format;			/* Format identifier--format = 1 */
  Offset	mark1Coverage;		/* Offset to Combining Mark Coverage
					 * table--from beginning of MarkMarkPos
					 * subtable */
  Offset	mark2Coverage;		/* Offset to Base Mark Coverage
					 * table--from beginning of MarkMarkPos
					 * subtable */
  USHORT	offset;			/* Mark1Array */
  Offset	mark2Array;		/* Offset to Mark2Array table for
					 * Mark2--from beginning of MarkMarkPos
					 * subtable */
};
ASSERT_SIZE (MarkMarkPosFormat1, 10);

struct MarkMarkPos {

  friend struct PosLookupSubTable;

  private:

  inline bool apply (LOOKUP_ARGS_DEF) const {
    switch (u.format) {
    case 1: return u.format1->apply (LOOKUP_ARGS);
    default:return false;
    }
  }

  private:
  union {
  USHORT		format;		/* Format identifier */
  MarkMarkPosFormat1	format1[];
  } u;
};
ASSERT_SIZE (MarkMarkPos, 2);


static inline bool position_lookup (LOOKUP_ARGS_DEF, unsigned int lookup_index);

struct ContextPos : Context {

  inline bool apply (LOOKUP_ARGS_DEF) const {
    return Context::apply (LOOKUP_ARGS, position_lookup);
  }
};
ASSERT_SIZE (ContextPos, 2);

struct ChainContextPos : ChainContext {

  inline bool apply (LOOKUP_ARGS_DEF) const {
    return ChainContext::apply (LOOKUP_ARGS, position_lookup);
  }
};
ASSERT_SIZE (ChainContextPos, 2);


struct ExtensionPosFormat1 {

  friend struct ExtensionPos;

  private:
  inline unsigned int get_type (void) const { return extensionLookupType; }
  inline unsigned int get_offset (void) const { return (extensionOffset[0] << 16) + extensionOffset[1]; }
  inline bool apply (LOOKUP_ARGS_DEF) const;

  private:
  USHORT	format;			/* Format identifier. Set to 1. */
  USHORT	extensionLookupType;	/* Lookup type of subtable referenced
					 * by ExtensionOffset (i.e. the
					 * extension subtable). */
  USHORT	extensionOffset[2];	/* Offset to the extension subtable,
					 * of lookup type subtable.
					 * Defined as two shorts to avoid
					 * alignment requirements. */
};
ASSERT_SIZE (ExtensionPosFormat1, 8);

struct ExtensionPos {

  friend struct PosLookup;
  friend struct PosLookupSubTable;

  private:

  inline unsigned int get_type (void) const {
    switch (u.format) {
    case 1: return u.format1->get_type ();
    default:return 0;
    }
  }

  inline bool apply (LOOKUP_ARGS_DEF) const {
    switch (u.format) {
    case 1: return u.format1->apply (LOOKUP_ARGS);
    default:return false;
    }
  }

  private:
  union {
  USHORT		format;		/* Format identifier */
  ExtensionPosFormat1	format1[];
  } u;
};
ASSERT_SIZE (ExtensionPos, 2);


/*
 * PosLookup
 */

enum {
  GPOS_Single			= 1,
  GPOS_Pair			= 2,
  GPOS_Cursive			= 3,
  GPOS_MarkBase			= 4,
  GPOS_MarkLig			= 5,
  GPOS_MarkMark			= 6,
  GPOS_Context			= 7,
  GPOS_ChainContext		= 8,
  GPOS_Extension		= 9,
};


struct PosLookupSubTable {

  friend struct PosLookup;

  inline bool apply (LOOKUP_ARGS_DEF, unsigned int lookup_type) const {

    switch (lookup_type) {
    case GPOS_Single:			return u.single->apply (LOOKUP_ARGS);
    case GPOS_Pair:			return u.pair->apply (LOOKUP_ARGS);
    case GPOS_Cursive:			return u.cursive->apply (LOOKUP_ARGS);
    case GPOS_MarkBase:			return u.markBase->apply (LOOKUP_ARGS);
    case GPOS_MarkLig:			return u.markLig->apply (LOOKUP_ARGS);
    case GPOS_MarkMark:			return u.markMark->apply (LOOKUP_ARGS);
    case GPOS_Context:			return u.context->apply (LOOKUP_ARGS);
    case GPOS_ChainContext:		return u.chainContext->apply (LOOKUP_ARGS);
    case GPOS_Extension:		return u.extension->apply (LOOKUP_ARGS);
    default:return false;
    }
  }

  private:
  union {
  USHORT			format;
  SinglePos			single[];
  PairPos			pair[];
  CursivePos			cursive[];
  MarkBasePos			markBase[];
  MarkLigPos			markLig[];
  MarkMarkPos			markMark[];
  ContextPos			context[];
  ChainContextPos		chainContext[];
  ExtensionPos			extension[];
  } u;
};
ASSERT_SIZE (PosLookupSubTable, 2);


struct PosLookup : Lookup {

  inline const PosLookupSubTable& get_subtable (unsigned int i) const {
    return (const PosLookupSubTable&) Lookup::get_subtable (i);
  }

  /* Like get_type(), but looks through extension lookups.
   * Never returns Extension */
  inline unsigned int get_effective_type (void) const {
    unsigned int type = get_type ();

    if (HB_UNLIKELY (type == GPOS_Extension)) {
      unsigned int count = get_subtable_count ();
      type = get_subtable(0).u.extension->get_type ();
      /* The spec says all subtables should have the same type.
       * This is specially important if one has a reverse type! */
      for (unsigned int i = 1; i < count; i++)
        if (get_subtable(i).u.extension->get_type () != type)
	  return 0;
    }

    return type;
  }

  inline bool apply_subtables (hb_ot_layout_t *layout,
			       hb_buffer_t    *buffer,
			       unsigned int    context_length,
			       unsigned int    nesting_level_left,
			       unsigned int    property) const {
    unsigned int lookup_type = get_type ();
    unsigned int lookup_flag = get_flag ();

    for (unsigned int i = 0; i < get_subtable_count (); i++)
      if (get_subtable (i).apply (LOOKUP_ARGS, lookup_type))
	return true;

    return false;
  }

  inline bool apply_once (hb_ot_layout_t *layout, hb_buffer_t *buffer) const {

    unsigned int lookup_flag = get_flag ();

    unsigned int property;
    if (!_hb_ot_layout_check_glyph_property (layout, IN_CURITEM (), lookup_flag, &property))
      return false;

    return apply_subtables (layout, buffer, NO_CONTEXT, MAX_NESTING_LEVEL, property);
  }

  bool apply_string (hb_ot_layout_t *layout,
		     hb_buffer_t    *buffer,
		     hb_ot_layout_feature_mask_t mask) const {

    bool ret = false;

    if (HB_UNLIKELY (!buffer->in_length))
      return false;

    layout->gpos_info.last = 0xFFFF; /* no last valid glyph for cursive pos. */

    buffer->in_pos = 0;
    while (buffer->in_pos < buffer->in_length) {

      bool done;
      if (~IN_PROPERTIES (buffer->in_pos) & mask) {
	  done = apply_once (layout, buffer);
	  ret |= done;
      } else {
          done = false;
	  /* Contrary to properties defined in GDEF, user-defined properties
	     will always stop a possible cursive positioning.                */
	  layout->gpos_info.last = 0xFFFF;
      }

      if (!done)
	buffer->in_pos++;
    }

    return ret;
  }
};
ASSERT_SIZE (PosLookup, 6);


/*
 * GPOS
 */

struct GPOS : GSUBGPOS {
  static const hb_tag_t Tag		= HB_TAG ('G','P','O','S');

  STATIC_DEFINE_GET_FOR_DATA (GPOS);
  /* XXX check version here? */

  inline const PosLookup& get_lookup (unsigned int i) const {
    return (PosLookup&)(((GSUBGPOS *)this)->get_lookup (i));
  }

  inline bool position_lookup (hb_ot_layout_t *layout,
			       hb_buffer_t    *buffer,
			       unsigned int    lookup_index,
			       hb_ot_layout_feature_mask_t  mask) const {
    return get_lookup (lookup_index).apply_string (layout, buffer, mask);
  }

};
ASSERT_SIZE (GPOS, 10);


/* Out-of-class implementation for methods recursing */

inline bool ExtensionPosFormat1::apply (LOOKUP_ARGS_DEF) const {
  unsigned int lookup_type = get_type ();

  if (HB_UNLIKELY (lookup_type ==  GPOS_Extension))
    return false;

  return ((PosLookupSubTable&)*(((char *) this) + get_offset ())).apply (LOOKUP_ARGS, lookup_type);
}

static inline bool position_lookup (LOOKUP_ARGS_DEF, unsigned int lookup_index) {
  const GPOS &gpos = *(layout->gpos);
  const PosLookup &l = gpos.get_lookup (lookup_index);

  if (HB_UNLIKELY (nesting_level_left == 0))
    return false;
  nesting_level_left--;

  if (HB_UNLIKELY (context_length < 1))
    return false;

  return l.apply_subtables (layout, buffer, context_length, nesting_level_left, property);
}


#endif /* HB_OT_LAYOUT_GPOS_PRIVATE_H */