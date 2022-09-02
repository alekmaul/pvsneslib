The Data Structure
==================

**libTMX** loads maps into a plain old C datastructure, this page describes that data structure thoroughly.
**libTMX** reuse names from the `TMX Format`_ for all its structures and structure members.

.. _TMX Format: https://doc.mapeditor.org/en/stable/reference/tmx-map-format/

Definitions
-----------

.. _tile-flip-flags:

Tiled use the 3 leftmost bits of the :term:`GID` to store if a tile is flipped.

.. c:macro:: TMX_FLIPPED_HORIZONTALLY

   Used to tell if a cell has the horizontal flip flag set:

   .. code-block:: c

      int is_horizontally_flipped = cell & TMX_FLIPPED_HORIZONTALLY;

.. c:macro:: TMX_FLIPPED_VERTICALLY

   Used to tell if a cell has the vertical flip flag set:

   .. code-block:: c

      int is_horizontally_flipped = cell & TMX_FLIPPED_VERTICALLY;

.. c:macro:: TMX_FLIPPED_DIAGONALLY

   Used to tell if a cell has the diagonal flip flag set:

   .. code-block:: c

      int is_horizontally_flipped = cell & TMX_FLIPPED_DIAGONALLY;

.. c:macro:: TMX_FLIP_BITS_REMOVAL

   Used to remove all flip flags:

   .. code-block:: c

      int gid   = cell &  TMX_FLIP_BITS_REMOVAL;
      int flags = cell & ~TMX_FLIP_BITS_REMOVAL;

Enumerations
------------

.. c:type:: enum tmx_map_orient

   Map orientation (orthogonal, isometric, hexagonal, ...).

   +-----------------+-----------------------------------------------+
   | Map orientation | Description                                   |
   +=================+===============================================+
   | O_NONE          | In case of error, or unknown map orientation. |
   +-----------------+-----------------------------------------------+
   | O_ORT           | Orthogonal map.                               |
   +-----------------+-----------------------------------------------+
   | O_ISO           | Isometric map.                                |
   +-----------------+-----------------------------------------------+
   | O_STA           | Staggered map.                                |
   +-----------------+-----------------------------------------------+
   | O_HEX           | Hexagonal map.                                |
   +-----------------+-----------------------------------------------+

.. c:type:: enum tmx_map_renderorder

   How the tile layers should be drawn, the order in which tiles on tile layers are rendered.
   This is especially usefull if you're drawing overlapping tiles (see the perspective_walls.tmx example that ships with
   Tiled).

   +-----------------+-----------------------------------------------+
   | Map renderorder | Description                                   |
   +=================+===============================================+
   | R_NONE          | In case of error, or unknown map renderorder. |
   +-----------------+-----------------------------------------------+
   | R_RIGHTDOWN     | Draw tiles from right to left, top to bottom. |
   +-----------------+-----------------------------------------------+
   | R_RIGHTUP       | Draw tiles from right to left, bottom to top. |
   +-----------------+-----------------------------------------------+
   | R_LEFTDOWN      | Draw tiles from left to right, top to bottom. |
   +-----------------+-----------------------------------------------+
   | R_LEFTUP        | Draw tiles from left to right, bottom to top. |
   +-----------------+-----------------------------------------------+


.. c:type:: enum tmx_stagger_index

   For staggered and hexagonal maps, determines whether the "even" or "odd" indexes along the staggered axis are shifted.

   +---------------+---------------------------------------------+
   | Stagger index | Description                                 |
   +===============+=============================================+
   | SI_NONE       | In case of error, or unknown stagger index. |
   +---------------+---------------------------------------------+
   | SI_EVEN       | Odd.                                        |
   +---------------+---------------------------------------------+
   | SI_ODD        | Even.                                       |
   +---------------+---------------------------------------------+

.. c:type:: enum tmx_stagger_axis

   For staggered and hexagonal maps, determines which axis ("x" or "y") is staggered.

   +--------------+--------------------------------------------+
   | Stagger axis | Description                                |
   +==============+============================================+
   | SA_NONE      | In case of error, or unknown stagger axis. |
   +--------------+--------------------------------------------+
   | SA_X         | x axis.                                    |
   +--------------+--------------------------------------------+
   | SA_Y         | y axis.                                    |
   +--------------+--------------------------------------------+

.. c:type:: enum tmx_obj_alignment

   Controls the alignment of tiles.
   When unspecified, tile objects use OA_BOTTOMLEFT in orthogonal mode and OA_BOTTOM in isometric mode (see :c:type:`tmx_map_orient`).

   +----------------+----------------------------------+
   | Object align   | Description                      |
   +================+==================================+
   | OA_NONE        | Object alignment is unspecified. |
   +----------------+----------------------------------+
   | OA_TOP         | Top.                             |
   +----------------+----------------------------------+
   | OA_LEFT        | Left.                            |
   +----------------+----------------------------------+
   | OA_BOTTOM      | Bottom.                          |
   +----------------+----------------------------------+
   | OA_RIGHT       | Right.                           |
   +----------------+----------------------------------+
   | OA_TOPLEFT     | Top-Left.                        |
   +----------------+----------------------------------+
   | OA_TOPRIGHT    | Top-Right.                       |
   +----------------+----------------------------------+
   | OA_BOTTOMLEFT  | Bottom-Left.                     |
   +----------------+----------------------------------+
   | OA_BOTTOMRIGHT | Bottom-Right.                    |
   +----------------+----------------------------------+

.. c:type:: enum tmx_layer_type

   Type of :term:`layer`.

   +------------+------------------------------------------------------+
   | Layer type | Description                                          |
   +============+======================================================+
   | L_NONE     | In case of error, or unknown layer type.             |
   +------------+------------------------------------------------------+
   | L_LAYER    | Tile layer type, use `content.gids`.                 |
   +------------+------------------------------------------------------+
   | L_OBJGR    | Objectgroup layer type, use `content.objgr`.         |
   +------------+------------------------------------------------------+
   | L_IMAGE    | Image layer type, use `content.image`.               |
   +------------+------------------------------------------------------+
   | L_GROUP    | Group of layer layer type, use `content.group_head`. |
   +------------+------------------------------------------------------+

.. c:type:: enum tmx_objgr_draworder

   Whether the :term:`objects <object>` are drawn according to the order of appearance ("index") or sorted by their
   y-coordinate ("topdown").

   +------------------+------------------------------------------------------+
   | Object draworder | Description                                          |
   +==================+======================================================+
   | G_NONE           | In case of error, or unknown draw order.             |
   +------------------+------------------------------------------------------+
   | G_INDEX          | Draw objects as they are ordered in the linked-list. |
   +------------------+------------------------------------------------------+
   | G_TOPDOWN        | Draw objects sorted by their y-coordinate, objects   |
   |                  | must then be reordered by their y-coordinate.        |
   +------------------+------------------------------------------------------+

.. c:type:: enum tmx_obj_type

   Type of :term:`object`.

   +-------------+----------------------------------------------------------+
   | Object type | Description                                              |
   +=============+==========================================================+
   | OT_NONE     | In case of error, or unknown object type.                |
   +-------------+----------------------------------------------------------+
   | OT_SQUARE   | Square, use members `x`, `y`, `width` and `height`.      |
   +-------------+----------------------------------------------------------+
   | OT_POLYGON  | Polygon, use `content.shape`.                            |
   +-------------+----------------------------------------------------------+
   | OT_POLYLINE | Polyline, use `content.shape`.                           |
   +-------------+----------------------------------------------------------+
   | OT_ELLIPSE  | Ellipse, use members `x`, `y`, width (horizontal radius) |
   |             | and height (vertical radius)                             |
   +-------------+----------------------------------------------------------+
   | OT_TILE     | Tile, use `content.gid`.                                 |
   +-------------+----------------------------------------------------------+
   | OT_TEXT     | Text, use `content.text`.                                |
   +-------------+----------------------------------------------------------+
   | OT_POINT    | Point, use members `x`, `y`.                             |
   +-------------+----------------------------------------------------------+

.. c:type:: enum tmx_property_type

   Type of property.

   +---------------+--------------------------------------------------------+
   | Property type | Description                                            |
   +===============+========================================================+
   | PT_NONE       | In case of error, or unknown property type.            |
   +---------------+--------------------------------------------------------+
   | PT_INT        | Integer, use `value.integer`.                          |
   +---------------+--------------------------------------------------------+
   | PT_FLOAT      | Float, use `value.decimal`.                            |
   +---------------+--------------------------------------------------------+
   | PT_BOOL       | Boolean, use `value.boolean`.                          |
   +---------------+--------------------------------------------------------+
   | PT_STRING     | String, use `value.string`.                            |
   +---------------+--------------------------------------------------------+
   | PT_COLOR      | Color, use `value.color` (RGBA encoded in an integer). |
   +---------------+--------------------------------------------------------+
   | PT_FILE       | Path to a file, use `value.file`.                      |
   +---------------+--------------------------------------------------------+

.. c:type:: enum tmx_horizontal_align

   Horizontal alignment of the text within the object.

   +------------+------------------------------------------+
   | Text align | Description                              |
   +============+==========================================+
   | HA_NONE    | In case of error, or unknown text align. |
   +------------+------------------------------------------+
   | HA_LEFT    | Left.                                    |
   +------------+------------------------------------------+
   | HA_CENTER  | Center.                                  |
   +------------+------------------------------------------+
   | HA_RIGHT   | Right.                                   |
   +------------+------------------------------------------+

.. c:type:: enum tmx_vertical_align

   Vertical alignment of the text within the object.

   +------------+------------------------------------------+
   | Text align | Description                              |
   +============+==========================================+
   | VA_NONE    | In case of error, or unknown text align. |
   +------------+------------------------------------------+
   | VA_TOP     | Top.                                     |
   +------------+------------------------------------------+
   | VA_CENTER  | Center.                                  |
   +------------+------------------------------------------+
   | VA_BOTTOM  | Bottom.                                  |
   +------------+------------------------------------------+


Data Structures
---------------

The datastructure is a :term:`tree`, just like the source :term:`XML` document, from the root (:c:type:`tmx_map`)
you can access everything.

.. c:type:: tmx_map

   The :term:`root <Tree>` of the datastructure.

   .. c:member:: enum tmx_map_orient orient

      Map orientation, see :c:type:`tmx_map_orient`.

   .. c:member:: unsigned int width

      The width of the map in cells.

   .. c:member:: unsigned int height

      The height of the map in cells.

   .. c:member:: unsigned int tile_width

      The width of tiles in pixels.

   .. c:member:: unsigned int tile_height

      The height of tiles in pixels.

   .. c:member:: enum tmx_stagger_index stagger_index

      Stagger index, see :c:type:`tmx_stagger_index`.

   .. c:member:: enum tmx_stagger_axis stagger_axis

      Stagger axis, see :c:type:`tmx_stagger_axis`.

   .. c:member:: int hexsidelength

      Only for hexagonal maps. Determines the width or height (depending on the staggered axis) of the tile’s edge,
      in pixels.

   .. c:member:: unsigned int backgroundcolor

      Global background colour, encoded in an integer, 4 bytes: ARGB.

   .. c:member:: enum tmx_map_renderorder renderorder

      Map render order, see :c:type:`tmx_map_renderorder`.

   .. c:member:: tmx_properties *properties

      Properties of the map, see :c:type:`tmx_properties`.

   .. c:member:: tmx_tileset_list *ts_head

      Head of the tileset :term:`linked list`, see :c:type:`tmx_`.

   .. c:member:: tmx_layer *ly_head

      Head of the layers :term:`linked list`, see :c:type:`tmx_layer`.

   .. c:member:: unsigned int tilecount

      length of the :c:member:`tiles` array described below.

   .. c:member:: tmx_tile **tiles

      :term:`GID` indexed tile array (array of pointers to :c:type:`tmx_tile`).

   .. c:member:: tmx_user_data user_data

      Use that member to store your own data, see :c:type:`tmx_user_data`.

.. c:type:: tmx_layer

   :term:`Layer` data.

   .. c:member:: int id

      A map-unique ID of the :term:`layer`.

   .. c:member:: char *name

      Name of the :term:`layer` (user defined).

   .. c:member:: double opacity

      Opacity of the layer (0.0 = transparent, 1.0 = opaque).

   .. c:member:: int visible

      Boolean, visibility of the layer (0 = false, any other value = true).

   .. c:member:: int offsetx

      Horizontal offset in pixels, a positive value shifts the layer to the right.

   .. c:member:: int offsety

      Vertical offset in pixels, a positive value shifts the layer down.

   .. c:member:: uint32_t tintcolor

      Optional tint colour, encoded in an integer, 4 bytes: ARGB.

   .. c:member:: enum tmx_layer_type type

      Type of layer, see :c:type:`tmx_layer_type`, tells you which member to use in :c:member:`tmx_layer.content`.

   .. c:member:: union layer_content content

      Content of the layer, as there are several types of layers (tile, object, image, ...),
      the content is different for each type.

      .. note::
         You should check the value of member :c:member:`tmx_layer.type` to use the correct union member.

      .. c:type:: union layer_content

         .. c:member:: int32_t *gids

            Array of layer :term:`cells <Cell>`.

            .. warning::
               GID=0 (zero) is a special :term:`GID` which means that this :term:`cell` is empty!

            Example: iterate on all cells, from left to right, top to bottom:

            .. code-block:: c

               for(int cell_y = 0; cell_y < map->height; cell_y++) {
                  for(int cell_x = 0; cell_x < map->width; cell_x++) {
                     int32_t cell = layer->content.gids[cell_y * map->width + cell_x];
                     int32_t GID = cell & TMX_FLIP_BITS_REMOVAL;
                     /* Draw tile operation... */
                  }
               }

            Example: Direct access to the cell:

            .. code-block:: c

               int32_t get_cell_at(tmx_layer *layer, unsigned int map_width, unsigned int x, unsigned int y) {
                  return layer->content.gids[y * map_width + x];
               }

         .. c:member:: tmx_object_group *objgr

            This layer is an object group.

         .. c:member:: tmx_image *image

            This layer is an image layer.

         .. c:member:: tmx_layer *group_head

            This layer is a group of layer, pointer to the head of a :term:`linked list` of children layers.

   .. c:member:: tmx_user_data user_data

      Use that member to store your own data, see :c:type:`tmx_user_data`.

   .. c:member:: tmx_properties *properties

      Properties of the layer, see :c:type:`tmx_properties`.

   .. c:member:: tmx_layer *next

      Next element of the :term:`linked list`, if NULL then you reached the last element.

.. c:type:: tmx_tileset_list

   In map :term:`tileset` data.

   .. c:member:: int is_embedded

      Private member used internally to free this tileset (depends on the usage of a resource manager).

   .. c:member:: unsigned int firstgid

      GID (Global ID) of the first tile in this tileset.

   .. c:member:: char *source

      In case of external tileset, path to the :term:`TSX` file (should be relative to the location of this map on the
      file system), NULL otherwise.

   .. c:member:: tmx_tileset *tileset

      Tileset data, see :c:type:`tmx_tileset`.

   .. c:member:: tmx_tileset_list *next

      Next element of the :term:`linked list`, if NULL then you reached the last element.

.. c:type:: tmx_tileset

   :term:`Tileset` data, usually loaded from an external :term:`TSX` file.

   .. c:member:: char *name

      Name of the tileset (user defined).

   .. c:member:: unsigned int tile_width

      The width of tiles in pixels.

   .. c:member:: unsigned int tile_height

      The height of tiles in pixels.

   .. c:member:: unsigned int spacing

      The spacing in pixels between the tiles in this tileset (applies to the tileset image).

   .. c:member:: unsigned int margin

      The margin around the tiles in this tileset (applies to the tileset image).

   .. c:member:: int x_offset

      Horizontal offset in pixels, a positive value shifts the drawing of tiles to the right.

   .. c:member:: int y_offset

      Vertical offset in pixels, a positive value shifts the drawing of tiles down.

   .. c:member:: enum tmx_obj_alignment

      Controls the alignment of tiles, see :c:type:`tmx_obj_alignment`.

   .. c:member:: unsigned int tilecount

      The number of tiles in this tileset, length of the :c:member:`tmx_tileset.tiles` array.

   .. c:member:: tmx_image *image

      Image for this tileset, may be NULL if this tileset is a collection of single images (one image per tile).

   .. c:member:: tmx_user_data user_data

      Use that member to store your own data, see :c:type:`tmx_user_data`.

   .. c:member:: tmx_properties *properties

      Properties of the tileset, see :c:type:`tmx_properties`.

   .. c:member:: tmx_tile *tiles

      Array of :c:type:`tmx_tile`, its length is :c:member:`tmx_tileset.tilecount`.

.. c:type:: tmx_tile

   :term:`Tile` data.

   .. c:member:: unsigned int id

      :term:`LID` (Local ID) of the tile.

      To compute the :term:`GID` in a map from the LID from a tileset, add that LID with the
      :c:member:`tmx_tileset_list.firstgid` of its in map tileset reference.

      .. code-block:: c

         unsigned int GID = tileset_list->firstgid + LID;

   .. c:member:: tmx_tileset *tileset

      The owner of this tile, see :c:type:`tmx_tileset`.

   .. c:member:: unsigned int ul_x

      Upper-left x coordinate of this tile on the tileset image, irrelevant if the this tile has its own image.

   .. c:member:: unsigned int ul_y

      Upper-left y coordinate of this tile on the tileset image, irrelevant if the this tile has its own image.

   .. c:member:: tmx_image *image

      Image for this tile, may be NULL if this tileset use a single image (:term:`atlas`) for all tiles.

   .. c:member:: tmx_object *collision

      Collision shape of this tile, may be NULL (optional).

   .. c:member:: unsigned int animation_len

      Length of the :c:member:`tmx_tile.animation` array.

   .. c:member:: tmx_anim_frame *animation

      Array of :c:type:`tmx_anim_frame` animation frames.

   .. c:member:: char *type

      Type (user defined).

   .. c:member:: tmx_properties *properties

      Properties of the tile, see :c:type:`tmx_properties`.

   .. c:member:: tmx_user_data user_data

      Use that member to store your own data, see :c:type:`tmx_user_data`.

.. c:type:: tmx_object_group

   .. c:member:: unsigned int color

      Colour of the object group, encoded in an integer, 4 bytes: ARGB.

   .. c:member:: enum tmx_objgr_draworder draworder

      Draw order, see :c:type:`tmx_objgr_draworder`.

   .. c:member:: tmx_object *head

      Head of the object :term:`linked list`, see :c:type:`tmx_object`.

.. c:type:: tmx_object

   :term:`Object` data.

   .. c:member:: unsigned int id

      Object ID.

   .. c:member:: enum tmx_obj_type obj_type

      Type of object, see :c:type:`tmx_obj_type`, tells you which member to use in :c:member:`tmx_object.content`.

   .. c:member:: double x

      Upper-left x coordinate of the object in pixels.

   .. c:member:: double y

      Upper-left y coordinate of the object in pixels.

   .. c:member:: double width

      Width of the object in pixels.

   .. c:member:: double height

      Height of the object in pixels.

   .. c:member:: union object_content content

      Content of the object, as there are several types of objects (tile, square, polygon, ...) the content is different
      for each type.

      .. note::
         You should check the value of member :c:member:`tmx_object.type` to use the correct union member.

      .. c:type:: union object_content

         .. c:member:: int gid

            GID of the tile for Tile objects.

         .. c:member:: tmx_shape *shape

            List of points for polygon and polyline objects, see :c:type:`tmx_shape`.

         .. c:member:: tmx_text *text

            Text and formatting for text objects, see :c:type:`tmx_text`.

   .. c:member:: int visible

      Boolean, visibility of the object (0 = false, any other value = true).

   .. c:member:: double rotation

      Rotation in degrees clockwise.

   .. c:member:: char *name

      Name (user defined).

   .. c:member:: char *type

      Type (user defined).

   .. c:member:: tmx_template *template

      Optional object template, holds default values that this object overrides.

   .. c:member:: tmx_properties *properties

      Properties of the object, see :c:type:`tmx_properties`.

   .. c:member:: tmx_object *next

      Next element of the :term:`linked list`, if NULL then you reached the last element.

.. c:type:: tmx_shape

   Points for object types Polyline and Polygon.

   .. c:member:: double **points

      Array of points (x,y coordinate couples).

      Usage:

      .. code-block:: c

         double x, y;
         for(int it = 0; it < shape->points_len; it++) {
           x = shape->point[it][0];
           y = shape->point[it][1];
           /* Draw operation... */
         }

   .. c:member:: int points_len

      Length of the :c:member:`tmx_shape.points` array.

.. c:type:: tmx_text

   For object type Text.

   .. c:member:: char *fontfamily

      Name of font to use.

   .. c:member:: int pixelsize

      Size of font in pixels.

   .. c:member:: unsigned int color

      Colour of the text, encoded in an integer, 4 bytes: ARGB.

   .. c:member:: int wrap

      Boolean, word wrapping (0 = false, any other value = true).

   .. c:member:: int bold

      Boolean, bold text (0 = false, any other value = true).

   .. c:member:: int italic

      Boolean, italic text (0 = false, any other value = true).

   .. c:member:: int underline

      Boolean, underlined text (0 = false, any other value = true).

   .. c:member:: int strikeout

      Boolean, striked out text (0 = false, any other value = true).

   .. c:member:: int kerning

      Boolean, use kerning (0 = false, any other value = true).

   .. c:member:: enum tmx_horizontal_align halign

      Horizontal alignment of text, see :c:type:`tmx_horizontal_align`.

   .. c:member:: enum tmx_vertical_align valign

      Vertical alignment of text, see :c:type:`tmx_vertical_align`.

   .. c:member:: char *text

      String to render.

.. c:type:: tmx_template

   Object template.

   .. c:member:: int is_embedded

      Private member used internally to free this object template.

   .. c:member:: tmx_tileset_list *tileset_ref

      Head of the :term:`linked list` of templates referenced by this object template, may be NULL.

   .. c:member:: tmx_object *object

      Template object.

.. c:type:: tmx_anim_frame

   .. c:member:: unsigned int tile_id

      LID of the :c:type:`tmx_tile` to be drawn during this frame.

   .. c:member:: unsigned int duration

      Duration of this frame in milliseconds.

.. c:type:: tmx_image

   .. c:member:: char *source

      Path to the image file (user defined, should be relative to the location of this map on the file system).

   .. c:member:: unsigned int trans

      Transparency colour, encoded in an integer, 3 bytes: RGB.

   .. c:member:: int uses_trans

      Boolean, if the transparency should (0 = false, any other value = true).

   .. c:member:: unsigned long width

      The image width in pixels.

   .. c:member:: unsigned long height

      The image height in pixels.

   .. c:member:: void *resource_image

      NULL unless the :ref:`image-autoload-autofree` callback functions are set, then holds the value returned by
      :c:data:`tmx_img_load_func`.

.. c:type:: tmx_properties

   This type is private, you can manipulate it using the :c:func:`tmx_get_property` and :c:func:`tmx_property_foreach`
   functions.

.. c:type:: tmx_property

   .. c:member:: char *name

      Name of the property (user defined).

   .. c:member:: enum tmx_property_type type

      Type of the property (String, Boolean, Path, ...), see :c:type:`tmx_property_type`,
      tells you which member to use in :c:member:`tmx_property.value`.

   .. c:member:: union tmx_property_value value

      Value of this property.

      .. note::
         You should check the value of member :c:member:`tmx_property.type` to use the correct union member.

      .. c:type:: union tmx_property_value

         .. c:member:: int integer

            Integer.

         .. c:member:: int boolean

            Boolean (0 = false, any other value = true).

         .. c:member:: float decimal

            Float.

         .. c:member:: char *string

            String.

         .. c:member:: char *file

            String (path).

         .. c:member:: unsigned int color

            Colour, encoded in an integer, 4 bytes: ARGB.

.. c:type:: union tmx_user_data

   .. c:member:: int integer

      Integer.

   .. c:member:: float decimal

      Float.

   .. c:member:: void *pointer

      Pointer.
