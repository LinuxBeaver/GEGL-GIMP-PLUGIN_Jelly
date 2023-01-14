/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * 2023 Beaver (GEGL Jelly)
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

enum_start (gegl_blend_mode_typecbevel2)
  enum_value (GEGL_BLEND_MODE_TYPE_HARDLIGHT2, "Hardlight",
              N_("HardLight"))
  enum_value (GEGL_BLEND_MODE_TYPE_MULTIPLY2,      "Multiply",
              N_("Multiply"))
  enum_value (GEGL_BLEND_MODE_TYPE_COLORDODGE2,      "ColorDodge",
              N_("ColorDodge"))
  enum_value (GEGL_BLEND_MODE_TYPE_PLUS2,      "Plus",
              N_("Plus"))
  enum_value (GEGL_BLEND_MODE_TYPE_DARKEN2,      "Darken",
              N_("Darken"))
  enum_value (GEGL_BLEND_MODE_TYPE_LIGHTEN2,      "Lighten",
              N_("Lighten"))
  enum_value (GEGL_BLEND_MODE_TYPE_OVERLAY2,      "Overlay",
              N_("Overlay"))
  enum_value (GEGL_BLEND_MODE_TYPE_GRAINMERGE2,      "GrainMerge",
              N_("Grain Merge"))
  enum_value (GEGL_BLEND_MODE_TYPE_SOFTLIGHT2,      "Softlight",
              N_("Soft Light"))
  enum_value (GEGL_BLEND_MODE_TYPE_ADDITION2,      "Addition",
              N_("Addition"))
  enum_value (GEGL_BLEND_MODE_TYPE_EMBOSSBLEND2,      "EmbossBlend",
              N_("ImageandColorOverlayMode"))
enum_end (GeglBlendModeTypecbevel2)

property_enum (blendmode2, _("Blend Mode of Internal Emboss"),
    GeglBlendModeTypecbevel2, gegl_blend_mode_typecbevel2,
    GEGL_BLEND_MODE_TYPE_GRAINMERGE2)


property_int (depth, _("Bevel Goo Depth (makes darker)"), 100)
    description (_("Filter width"))
    value_range (10, 100)




property_color (value, _("Color"), "#ffffff")
    description(_("The color to make transparent."))
    ui_meta     ("role", "output-extent")



property_double (opacity, _("Opacity"), 6.0)
  value_range   (2.0, 10.0)
  ui_steps      (0.01, 0.10)

property_double (gaus, _("Internal Gaussian Blur"), 1)
   description (_("Standard deviation for the XY axis"))
   value_range (0.0, 2.0)
    ui_meta     ("role", "output-extent")


property_int (box, _("Internal Box Blur"), 1)
   description(_("Radius of square pixel region, (width and height will be radius*2+1)"))
   value_range (0, 3)
   ui_range    (0, 3)
   ui_gamma   (1.5)
    ui_meta     ("role", "output-extent")


property_int    (mask_radius, _("Internal Oilify"), 6)
    description (_("Radius of circle around pixel, can also be scaled per pixel by a buffer on the aux pad."))
    value_range (2, 12)
    ui_range (4, 12)
    ui_meta     ("unit", "pixel-distance")




property_color (coloroverlay, _("Color of Goo"), "#fc5ae8")
    description (_("The color to paint over the input"))


property_int (snn, _("Make Splat Effect Stronger"), 30)
    description(_("Radius of square pixel region, (width and height will be radius*2+1)"))
    value_range (8, 60)
    ui_range    (8, 60)
    ui_gamma    (1.5)
    ui_meta     ("unit", "pixel-distance")


property_double (tile_size2, _("Major Goo Size"), 20.0)
    description (_("Average diameter of each tile (in pixels)"))
    value_range (8.0, 70.0)
    ui_meta     ("unit", "pixel-distance")

property_double (tile_size, _("Minor Goo Size"), 10.0)
    description (_("Average diameter of each tile (in pixels)"))
    value_range (8.0, 20.0)
    ui_meta     ("unit", "pixel-distance")



property_double (tile_saturation, _("Make lower for Goo Spacing"), 0.5)
    description (_("Expand tiles by this amount"))
    value_range (0.22, 0.6)


property_seed (seed, _("Goo Random seed"), rand)

property_int  (radius, _("Radius"), 2)
  value_range (1, 3)
  ui_range    (1, 3)
  ui_meta     ("unit", "pixel-distance")
  description (_("Neighborhood radius, a negative value will calculate with inverted percentiles"))
    ui_meta     ("role", "output-extent")


property_double  (alpha_percentile, _("Median's Alpha Percentile"), 63)
  value_range (51, 76)
  description (_("Neighborhood alpha percentile"))

#else

#define GEGL_OP_META
#define GEGL_OP_NAME     goosub
#define GEGL_OP_C_SOURCE goosub.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *cb, *in, *cubism, *cubism2, *oilify, *nop, *color, *snn, *opacity, *median, *output;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");



  cb    = gegl_node_new_child (gegl,
                                  "operation", "gegl:custom-bevel",
                                  NULL);

  in    = gegl_node_new_child (gegl,
                                  "operation", "gegl:src-in",
                                  NULL);


  cubism    = gegl_node_new_child (gegl,
                                  "operation", "gegl:cubism",
                                  NULL);

  cubism2    = gegl_node_new_child (gegl,
                                  "operation", "gegl:cubism",
                                  NULL);

  oilify    = gegl_node_new_child (gegl,
                                  "operation", "gegl:oilify",
                                  NULL);


  color    = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",
                                  NULL);

  snn    = gegl_node_new_child (gegl,
                                  "operation", "gegl:snn-mean",
                                  NULL);


  opacity    = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity",
                                  NULL);

  nop    = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);



  median    = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur",
                                  NULL);







  gegl_operation_meta_redirect (operation, "value", color, "value");
  gegl_operation_meta_redirect (operation, "tile_size", cubism, "tile-size");
  gegl_operation_meta_redirect (operation, "tile_size2", cubism2, "tile-size");
  gegl_operation_meta_redirect (operation, "tile_saturation", cubism, "tile-saturation");
  gegl_operation_meta_redirect (operation, "seed", cubism, "seed");
  gegl_operation_meta_redirect (operation, "seed", cubism2, "seed");
  gegl_operation_meta_redirect (operation, "mask_radius", oilify, "mask-radius");
  gegl_operation_meta_redirect (operation, "snn_mean", snn, "radius");
  gegl_operation_meta_redirect (operation, "opacity", opacity, "value");
  gegl_operation_meta_redirect (operation, "gaus", cb, "gaus");
  gegl_operation_meta_redirect (operation, "box", cb, "box");
  gegl_operation_meta_redirect (operation, "depth", cb, "depth");
  gegl_operation_meta_redirect (operation, "coloroverlay", cb, "coloroverlay");
  gegl_operation_meta_redirect (operation, "blendmode2", cb, "blendmode");
  gegl_operation_meta_redirect (operation, "snn", snn, "radius");
  gegl_operation_meta_redirect (operation, "radius", median, "radius");
  gegl_operation_meta_redirect (operation, "alpha_percentile", median, "alpha-percentile");






  gegl_node_link_many (input, in, nop, median, opacity, output, NULL);
  gegl_node_link_many (input, cubism, color, cubism2, oilify, snn, cb, NULL);
  gegl_node_connect_from (in, "aux", cb, "output");


}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:jelly",
    "title",       _("Jelly"),
    "categories",  "Aristic",
    "reference-hash", "godo6a24o1600g0fjf25sb2ac",
    "description", _("GEGL puts Jelly/Goo spots on a subject/object. You are expected to use a duplicate layer above and Gimp's layer mask or eraser to remove undersirable goo regions. This works well with the hard light blend mode on union or normal at 60-80% opacity. This filter is expected to be extremely slow. "
                     ""),
    NULL);
}

#endif
