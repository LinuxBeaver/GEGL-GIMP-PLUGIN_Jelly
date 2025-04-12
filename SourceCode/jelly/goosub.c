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

/* GEGL GRAPH OF JELLY TO USE WITHOUT INSTALLING

id=1 src-in aux=[ ref=1 cubism tile-size=8 tile-saturation=0.45 seed=2781
color-overlay value=#ffffff
cubism tile-size=34 seed=2822
oilify mask-radius=6
snn-mean radius=12
custom-bevel gaus=1 box=1 blendmode=GrainMerge coloroverlay=#00ff40
 ]
opacity value=5
median-blur radius=2 alpha-percentile=68



Fun fact this was a joke filter just to show off GEGL's potential */


#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES




property_int (depth, _("Bevel Goo Depth (makes darker)"), 100)
    description (_("Internal Bevel Filter width"))
    value_range (10, 100)



property_double (opacity, _("Opacity to make Goo more prominent"), 2.0)
  value_range   (1.0, 2.0)
  ui_steps      (0.01, 0.10)




property_int    (mask_radius, _("Internal Oilify"), 6)
    description (_("Radius of circle around pixel, can also be scaled per pixel by a buffer on the aux pad."))
    value_range (2, 12)
    ui_range (4, 12)
    ui_meta     ("unit", "pixel-distance")


property_color (coloroverlay, _("Color of Goo"), "#fc5ae8")
    description (_("The color to paint the goo"))


property_int (snn, _("Make Splat Effect Stronger"), 30)
    description(_("SNN Mean to make a splat effect"))
    value_range (8, 60)
    ui_range    (8, 60)
    ui_gamma    (1.5)
    ui_meta     ("unit", "pixel-distance")


property_double (tile_size2, _("Major Goo Size"), 34.0)
    description (_("This slider mostly dicates the size of large goo"))
    value_range (8.0, 70.0)
    ui_meta     ("unit", "pixel-distance")

property_double (tile_size, _("Minor Goo Size"), 17.0)
    description (_("This slider mostly dictates the size of small goo"))
    value_range (8.0, 20.0)
    ui_meta     ("unit", "pixel-distance")



property_double (tile_saturation, _("Goo Spacing"), 0.55)
    description (_("At lower values this will space the Goo"))
    value_range (0.22, 1.0)
    ui_range (0.22, 0.6)


property_seed (seed, _("Goo Random seed"), rand)


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
  GeglColor *hidden_color_jelly = gegl_color_new ("#ffffff");

    GeglNode *input    = gegl_node_get_input_proxy (gegl, "input");
    GeglNode *output   = gegl_node_get_output_proxy (gegl, "output");



    GeglNode *cb    = gegl_node_new_child (gegl,
                                  "operation", "lb:custom-bevel", "blendmode", 8,
                                  NULL);

/* My Custom Bevel's 8th blend mode is Grain Merge. This is NOT present in the build of custom bevel tht was excepted in Gimp.
In the future I wish it would be possible to just type in the blend mode name */

    GeglNode *in    = gegl_node_new_child (gegl,
                                  "operation", "gegl:src-in",
                                  NULL);


    GeglNode *cubism    = gegl_node_new_child (gegl,
                                  "operation", "gegl:cubism",
                                  NULL);

    GeglNode *cubism2    = gegl_node_new_child (gegl,
                                  "operation", "gegl:cubism",
                                  NULL);

    GeglNode *oilify    = gegl_node_new_child (gegl,
                                  "operation", "gegl:oilify",
                                  NULL);


    GeglNode *color    = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",
                                   "value", hidden_color_jelly, NULL);


    GeglNode *snn    = gegl_node_new_child (gegl,
                                  "operation", "gegl:snn-mean",
                                  NULL);


    GeglNode *opacity    = gegl_node_new_child (gegl,
                                  "operation", "gegl:opacity",
                                  NULL);

    GeglNode *nop    = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

    GeglNode *median2    = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur", "radius", 0,  "abyss-policy",  GEGL_ABYSS_NONE,
                                  NULL);


    GeglNode *median    = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur", "radius", 2,  "abyss-policy",  GEGL_ABYSS_NONE,
                                  NULL);

  gegl_operation_meta_redirect (operation, "tile_size", cubism, "tile-size");
  gegl_operation_meta_redirect (operation, "tile_size2", cubism2, "tile-size");
  gegl_operation_meta_redirect (operation, "tile_saturation", cubism, "tile-saturation");
  gegl_operation_meta_redirect (operation, "seed", cubism, "seed");
  gegl_operation_meta_redirect (operation, "seed", cubism2, "seed");
  gegl_operation_meta_redirect (operation, "mask_radius", oilify, "mask-radius");
  gegl_operation_meta_redirect (operation, "opacity", opacity, "value");
  gegl_operation_meta_redirect (operation, "depth", cb, "depth");
  gegl_operation_meta_redirect (operation, "coloroverlay", cb, "coloroverlay");
  gegl_operation_meta_redirect (operation, "snn", snn, "radius");
  gegl_operation_meta_redirect (operation, "alpha_percentile", median, "alpha-percentile");


  gegl_node_link_many (input, in, nop, median, opacity, median2, output, NULL);
  gegl_node_link_many (input, cubism, color, cubism2, oilify, snn, cb, NULL);
  gegl_node_connect (in, "aux", cb, "output");


}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "lb:jelly",
    "title",       _("Jelly"),
    "categories",  "Aristic",
    "reference-hash", "godo6a24o1600g0fjf25sb2ac",
    "description", _("GEGL puts Jelly/Goo spots on a subject/object. You are expected to use a duplicate layer above and Gimp's layer mask or eraser to remove undersirable goo regions. This works well with the hard light blend mode on union or normal at 60-80% opacity. This filter is expected to be extremely slow. "
                     ""),
    NULL);
}

#endif
