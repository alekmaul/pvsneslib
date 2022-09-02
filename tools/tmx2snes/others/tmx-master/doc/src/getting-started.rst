Getting Started
===============

**Tiled** maps are stored in :term:`TMX`, :term:`TSX` and :term:`TX` documents, that are based on the :term:`XML` format.
XML organizes data in a :term:`tree` structure, **libTMX** loads TMX maps in a C datastructure that is organized just
like the source XML document.

As **libTMX** has an ever-changing :term:`API` (and :term:`ABI`) therefore the recommended way to use it is to directly
incorporate it in the sources of your project.

For building libTMX, please read the :doc:`build` page.

Usage:

.. code-block:: c

   tmx_map *map = tmx_load(argv[1]);
   if (map == NULL) {
     tmx_perror("Cannot load map");
     return 1;
   }

Read the :doc:`renderer-from-scratch` article for an in-depth usage of libTMX.
