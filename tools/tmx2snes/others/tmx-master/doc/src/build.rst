Building libTMX
===============

.. warning::
   As **libTMX** has an ever-changing API (and ABI) therefore the recommended way to use it is to directly incorporate
   it in the sources of your project.

Prerequisites
-------------

* A C compiler (`Clang`_, `GCC`_, ...) **libTMX** is written in C89 but
  uses the ``<stdint.h>`` header that is in the C99 standard, therefore you may build **libTMX** using a C89 compiler as
  long as you provide that header.
* `ZLib`_ to uncompress layers (optional, select an uncompressed layer format in the properties).
* `ZStandard`_ to uncompress layers (optional, select an uncompressed layer format in the properties).
* `libxml2`_ to load maps; **libTMX** uses the `IO api`_ to load from various sources
  (protip: if libxml2 was built with the built-in HTTP client, then **libTMX** will be able to load maps from a remote
  server via HTTP), and the `xmlreader api`_ to parse XML documents, and the `hash api`_ as a hashtable implementation.
* The `CMake`_ build system builder.

Get the source code
-------------------

Fetch the source code from the `releases page`_ on github (you can also use git and fetch the needed tag).

Building libTMX
---------------

This documentation page only covers the building of **libTMX** using CMake and Make, but keep in mind that CMake being
a build system builder, it is able to generate build scripts and project files for many other tools such as Ninja,
Visual Studio, NMake, ... see the CMake `documentation on generators`_.

Configure using cmake
^^^^^^^^^^^^^^^^^^^^^

First, it is recommended to create a sub directory (known as the *binary dir*) to generate the build system.

CMake stores the configuration for the build in a cache file: CMakeCache.txt, this file will be created at the first
invocation.

CMake has a GUI (Windows only) and a ncurses UI (Linux/BSD/MacOS) to ease the editing of its cache, you can also
manipulate this cache using CMake's command line interface. See the `running CMake page`_.

**libTMX**'s cmake script declares two cache variables to configure the build:

+--------------------+---------------------------------------------------------------------+
| Cache Variable     | Description                                                         |
+====================+=====================================================================+
| WANT_ZLIB          | Link with zlib (ability to decompress layers data).                 |
+--------------------+---------------------------------------------------------------------+
| WANT_ZSTD          | Link with zstd (ability to decompress layers data).                 |
+--------------------+---------------------------------------------------------------------+
| ZSTD_PREFER_STATIC | Use the static build of zstd (Defaults to On).                      |
+--------------------+---------------------------------------------------------------------+
| BUILD_SHARED_LIBS  | Build shared libraries (dll / so), static libraries is the default. |
+--------------------+---------------------------------------------------------------------+

Default cache variables of interest:

+----------------------+-------------------------------------------------------+
| Cache Variable       | Description                                           |
+======================+=======================================================+
| CMAKE_BUILD_TYPE     | Specifies the build type (Debug, Release, ...).       |
+----------------------+-------------------------------------------------------+
| CMAKE_INSTALL_PREFIX | Install directory.                                    |
+----------------------+-------------------------------------------------------+
| LibXml2_ROOT         | Custom search path to find libxml2 (lib and headers). |
+----------------------+-------------------------------------------------------+
| ZLIB_ROOT            | Custom search path to find ZLib (lib and headers).    |
+----------------------+-------------------------------------------------------+

Compile and install
^^^^^^^^^^^^^^^^^^^

Once CMake has sucessfully generated the MakeFile, then run the usual:

.. code-block:: sh

   make && make install

Building the dependencies
-------------------------

Libxml2
^^^^^^^

Libxml2 pulls some unnecessary dependencies: ICU, iconv, ... as **libTMX** does not need these unicode libraries (unless
your software does) you may safely disable support along with all the unused features:

Recommended configuration (Unixes):

.. code-block:: sh

   ./autogen.sh \
       --prefix=/usr/local \
       --without-c14n \
       --without-catalog \
       --without-debug \
       --without-docbook \
       --without-ftp \
       --without-http \
       --without-html \
       --without-xpath \
       --without-xptr \
       --without-xinclude \
       --without-iconv \
       --without-icu \
       --without-iso8859x \
       --without-zlib \
       --without-lzma \
       --without-debug \
       --without-mem_debug \
       --without-run_debug \
       --without-schemas \
       --without-schematron \
       --without-regexps \
       --without-modules \
       --without-writer \
       --without-pattern \
       --without-push \
       --without-valid \
       --without-sax1 \
       --without-legacy \
       --without-output \
       --without-python \
       --with-tree \
       --with-writer \
       --with-reader

To generate debug symbols, remove ``--without-debug`` and add ``--with-debug``.

On Windows:

.. code-block:: batch

   cscript configure.js^
       trio=no^
       ftp=no^
       http=no^
       html=no^
       c14n=no^
       catalog=no^
       docb=no^
       xpath=no^
       xptr=no^
       xinclude=no^
       iconv=no^
       icu=no^
       iso8859x=no^
       zlib=no^
       lzma=no^
       debug=no^
       xml_debug=no^
       mem_debug=no^
       run_debug=no^
       schemas=no^
       schematron=no^
       regexps=no^
       modules=no^
       tree=yes^
       reader=yes^
       writer=yes^
       walker=no^
       pattern=no^
       push=no^
       valid=no^
       sax1=no^
       legacy=no^
       output=no^
       python=no

Creates a NMake build script. To create a MakeFile for MinGW, add: ``compiler=mingw``.

To create static libraries using MSVC, add: ``cruntime=/MT``.

To generate debug symbols, set ``debug=yes``.

ZLib
^^^^
Building ZLib requires no special configuration.


.. _Clang: https://clang.llvm.org/
.. _GCC: https://gcc.gnu.org/
.. _ZLib: http://zlib.net/
.. _ZStandard: http://zstd.net/
.. _libxml2: http://xmlsoft.org/
.. _IO api: http://xmlsoft.org/html/libxml-xmlIO.html
.. _xmlreader api: http://xmlsoft.org/html/libxml-xmlreader.html
.. _hash api: http://xmlsoft.org/html/libxml-hash.html
.. _CMake: https://cmake.org/
.. _releases page: https://github.com/baylej/tmx/releases
.. _documentation on generators: https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html
.. _running CMake page: https://cmake.org/runningcmake/
