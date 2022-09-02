Error Handling
==============

.. warning::
   Error management is the only non thread-safe functionality in **libTMX**, due to the *tmx_errno* global it is based on.

Error detection
---------------

.. c:var:: tmx_error_codes tmx_errno

   Every time a load function fails (map load functions would return NULL, resource load functions would return 0) this
   global is set to an error code.

Error codes
-----------

.. c:type:: enum tmx_error_codes

   Possible values for :c:data:`tmx_errno`.

   +------------+---------------------------------------------------------------------------------------------+
   | Error code | Description                                                                                 |
   +============+=============================================================================================+
   | E_NONE     | No error so far.                                                                            |
   +------------+---------------------------------------------------------------------------------------------+
   | E_UNKN     | Unknown error, call :c:func:`tmx_strerr` to get a description of the error.                 |
   +------------+---------------------------------------------------------------------------------------------+
   | E_INVAL    | Invalid argument, example: you passed NULL to :c:func:`tmx_load`.                           |
   +------------+---------------------------------------------------------------------------------------------+
   | E_ALLOC    | Memory allocation failed (running out of memory).                                           |
   +------------+---------------------------------------------------------------------------------------------+
   | E_ACCESS   | Missing privileges to access the file.                                                      |
   +------------+---------------------------------------------------------------------------------------------+
   | E_NOENT    | File not found.                                                                             |
   +------------+---------------------------------------------------------------------------------------------+
   | E_FORMAT   | Unsupported/Unknown file format (libTMX only supports the TMX/XML format).                  |
   +------------+---------------------------------------------------------------------------------------------+
   | E_ENCCMP   | Unsupported/Unknown data encoding/compression (libTMX only supports gzip and base64).       |
   +------------+---------------------------------------------------------------------------------------------+
   | E_FONCT    | Feature not enabled (ZLib not enabled when libTMX was built).                               |
   +------------+---------------------------------------------------------------------------------------------+
   | E_BDATA    | Base64 bad data (could not decode base64 encoded data).                                     |
   +------------+---------------------------------------------------------------------------------------------+
   | E_ZDATA    | Zlib corrupted data (could not decompress data).                                            |
   +------------+---------------------------------------------------------------------------------------------+
   | E_XDATA    | XML corrupted data (XML document is invalid).                                               |
   +------------+---------------------------------------------------------------------------------------------+
   | E_CDATA    | CSV corrupted data (CSV layer data is invalid).                                             |
   +------------+---------------------------------------------------------------------------------------------+
   | E_MISSEL   | Missing element, incomplete source (example: a <map> element missing its height attribute). |
   +------------+---------------------------------------------------------------------------------------------+

   .. note::
      ``tmx_errno`` is never reset to ``E_NONE``.

   .. note::
      Error code handling is not necessary as an error message is also generated, see :c:func:`tmx_strerr`.

Error functions
---------------

.. c:function:: void tmx_perror(const char *prefix)

   Print to stderr a string describing the last error.
   Equivalent to ``fprintf(stderr, "%s: %s\n", prefix, tmx_strerr());``

.. c:function:: const char* tmx_strerr(void)

   Return a string describing the last error, the returned error message is maximum 256 characters long (including the
   NULL terminator).
   Returned value is never NULL.

Error management example
------------------------

This is an example of how you could handle errors:

.. code-block:: c

   tmx_map *map = NULL;
   if (!(map = tmx_load("assets/maps/map01.tmx"))) {
     my_error_handler(tmx_strerr()); /* call user defined error handling function */
     return -1;
   }
   /* process map... */
   return 0;
