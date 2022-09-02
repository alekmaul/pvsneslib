Functions
=========

**libTMX** provides a few functions to load maps and use the hasmaps. To access a loaded map, see :doc:`datastructure`.

.. _load-functions:

Load maps
---------

.. c:function:: tmx_map* tmx_load(const char *path)

   Load a TMX map.

.. c:function:: tmx_map* tmx_load_buffer(const char *buffer, int len)

   Load a TMX map from the given buffer whose length is len.

.. c:function:: tmx_map* tmx_load_fd(int fd)

   Load a TMX map from a C file descriptor.

.. c:type:: typedef int (*tmx_read_functor)(void *userdata, char *buffer, int len)

   Definition of the tmx_read_functor callback.

.. c:function:: tmx_map* tmx_load_callback(tmx_read_functor callback, void *userdata)

   Load a TMX map using a callback function as defined above.

.. c:function:: void tmx_map_free(tmx_map *map)

   Free a loaded TMX map.

External resources
------------------

libTMX has a resource manager to store tilesets and object templates to avoid loading them twice or more.  

.. c:type:: tmx_resource_manager

   tmx_resource_manager is a private type.

.. c:function:: tmx_resource_manager* tmx_make_resource_manager()

   Create a new resource manager.

.. c:function:: void tmx_free_resource_manager(tmx_resource_manager *rc_mgr)

   Free a resource manager.

Load resources and maps
-----------------------

Tilesets
^^^^^^^^

.. c:function:: int tmx_load_tileset(tmx_resource_manager *rc_mgr, const char *path)

   Load a tileset at the given path in a resource manager.

.. c:function:: int tmx_load_tileset_buffer(tmx_resource_manager *rc_mgr, const char *buffer, int len, const char *key)

   Load a tileset from the given buffer in a resource manager.

.. c:function:: int tmx_load_tileset_fd(tmx_resource_manager *rc_mgr, int fd, const char *key)

   Load a tileset from a C file descriptor in a resource manager.

.. c:function:: int tmx_load_tileset_callback(tmx_resource_manager *rc_mgr, tmx_read_functor callback, void *userdata, const char *key)

   Load a tileset using a callback function in a resource manager.

Object Templates
^^^^^^^^^^^^^^^^

.. c:function:: int tmx_load_template(tmx_resource_manager *rc_mgr, const char *path)

   Load a tileset at the given path in a resource manager.

.. c:function:: int tmx_load_template_buffer(tmx_resource_manager *rc_mgr, const char *buffer, int len, const char *key)

   Load a tileset from the given buffer in a resource manager.

.. c:function:: int tmx_load_template_fd(tmx_resource_manager *rc_mgr, int fd, const char *key)

   Load a tileset from a C file descriptor in a resource manager.

.. c:function:: int tmx_load_template_callback(tmx_resource_manager *rc_mgr, tmx_read_functor callback, void *userdata, const char *key)

   Load a tileset using a callback function in a resource manager.

Maps
^^^^

.. c:function:: tmx_map* tmx_rcmgr_load(tmx_resource_manager *rc_mgr, const char *path)

   Load a TMX map, use a resource manager to resolve/store external resources.

.. c:function:: tmx_map* tmx_rcmgr_load_buffer(tmx_resource_manager *rc_mgr, const char *buffer, int len)

   Load a TMX map from the given buffer whose length is len, use a resource manager to resolve/store external resources.

.. c:function:: tmx_map* tmx_rcmgr_load_fd(tmx_resource_manager *rc_mgr, int fd)

   Load a TMX map from a C file descriptor, use a resource manager to resolve/store external resources.

.. c:function:: tmx_map* tmx_rcmgr_load_callback(tmx_resource_manager *rc_mgr, tmx_read_functor callback, void *userdata)

   Load a TMX map using a callback function as defined above. `userdata` is passed as-is.
   See :c:type:`tmx_read_functor`.

Utilities
---------

.. c:function:: tmx_tile* tmx_get_tile(tmx_map *map, unsigned int gid)

   .. deprecated:: 1.0
      use `map->tiles[gid]` instead, see :c:member:`tmx_map.tiles`.

.. c:function:: tmx_layer* tmx_find_layer_by_id(const tmx_map *map, int id)

   Get a layer by its ID, see :c:member:`tmx_layer.id`.

.. c:function:: tmx_layer* tmx_find_layer_by_name(const tmx_map *map, const char *name)

   Get a layer by its name (user defined string, use with care, may not be unique), see :c:member:`tmx_layer.name`.

.. c:function:: tmx_property* tmx_get_property(tmx_properties *hash, const char *key)

   Get a property by its name.

.. c:type:: typedef void (*tmx_property_functor)(tmx_property *property, void *userdata)

   Definition of the tmx_property_functor callback, to be used with :c:func:`tmx_property_foreach`.

.. c:function:: void tmx_property_foreach(tmx_properties *hash, tmx_property_functor callback, void *userdata)

   Call the given callback function for each properties, userdata is forwarded as-is.
   See :c:type:`tmx_property_functor`.

Colour conversion functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Helper functions to convert the unsigned int colour from the datastructure, see :c:member:`tmx_map.backgroundcolor`,
:c:member:`tmx_object_group.color`, :c:member:`tmx_property_value.color`.

.. c:type:: tmx_col_bytes

   4 unsigned bytes in the following order: r, g, b, a.

.. c:function:: tmx_col_bytes tmx_col_to_bytes(uint32_t color);

   Splits the colour into 4 bytes.

.. c:type:: tmx_col_floats

   4 floats in the following order: r, g, b, a.

.. c:function:: tmx_col_floats tmx_col_to_floats(uint32_t color);

   Splits the colour into 4 floats.