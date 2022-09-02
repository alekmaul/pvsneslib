Glossary
========

These are special term used by both **Tiled** and **libTMX**, or only the latter.

.. glossary::
   :sorted:

   ABI
      *Application Binary Interface*, an interface between two binary program modules, here between your software and
      the compiled version of **libTMX**. See the `ABI page on wikipedia`_.

   API
      *Application Programming Interface*, in C, a set of structure and function definitions used to build new sofware
      using existing material. See the `API page on wikipedia`_.

   Atlas
      A *texture atlas* is a set of :term:`textures <Texture>` assembled in a single image to reduce the overhead of
      loading several images from non volatile storage (hard drives, CD, ...) and to optimize the usage of :term:`VRAM`.

   Cell
      An entry of a tile layer, the content of a cell is the :term:`GID`, possibly tainted with a
      :ref:`tile flip flag <tile-flip-flags>`.

      See :c:member:`layer_content.gids`.

   GID
      *Global tile Identifier*, uniquely identifies a tile at map scope.

      To compute the GID from a :term:`LID` in a given map: ``int32_t GID = tileset_list->firstgid + tile->id;``.

      .. warning::
         GID=0 (zero) is a special GID which means that this :term:`cell` is empty!

   Layer
      See the `Working with Layers`_ page on Tiled documentation.

   LID
      *Local tile Identifier*, uniquely identifies a tile at tileset scope.

      See :c:member:`tmx_tile.id`.

   Linked List
      A linear collection of data elements, each element points to the next.
      See the `wikipedia page on linked lists`_.

   Object
      See the `Working with Objects`_ page on Tiled documentation.

   Texture
      Name given to images meant to be used in video games, usually given to images once they are loaded in memory
      or in the :term:`VRAM`.

   Tile
      The atomic element that constitute tile :term:`layers <Layer>` and :term:`tilesets <Tileset>`,
      usually a small square image.

   Tileset
      A set of tiles, in **Tiled** tilesets are usually external resources, hence they have a scope that is different
      from the scope of the map, explaining the :term:`GID` and :term:`LID` paradigm.
      Tilesets can be based on a single image (an :term:`atlas`) or a collection of images (one for each tile).

   TMX
      *Tile Map XML* map format used by **Tiled**, it is based on the :term:`XML` format.

   Tree
      A data structure of nodes, starting at a root node, each nodes may have zero, one or several child nodes.
      See the `wikipedia page on trees`_.

   TSX
      *Tile Set XML* tileset format used by **Tiled**, it is based on the :term:`XML` format.

   TX
      *Template XML* template format used by **Tiled**, it is based on the :term:`XML` format.

   VRAM
      *Video RAM*, name of the volatile random access memory attributed to the GPU.

   XML
      *Extensible Markup Language* is the markup language used by the `TMX map format`_.
      See the `XML specification`_.


.. _ABI page on wikipedia: https://en.wikipedia.org/wiki/Application_binary_interface
.. _API page on wikipedia: https://en.wikipedia.org/wiki/Application_programming_interface
.. _Working with Layers: http://docs.mapeditor.org/en/stable/manual/layers/
.. _wikipedia page on linked lists: https://en.wikipedia.org/wiki/Linked_list
.. _Working with Objects: http://docs.mapeditor.org/en/stable/manual/objects/
.. _TMX map format: http://docs.mapeditor.org/en/stable/reference/tmx-map-format/
.. _wikipedia page on trees: https://en.wikipedia.org/wiki/Tree_(data_structure)
.. _XML specification: https://www.w3.org/TR/2008/REC-xml-20081126/
