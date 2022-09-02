Override behaviours
===================

**libTMX** has several global public function pointers that may be used to override some of its behaviours.
These pointers are also known as callback functions.

Memory management
-----------------

Override the memory alloc and free functions used by libTMX and its dependencies.

.. c:var:: void* (*tmx_alloc_func) (void *address, size_t len)

   Same definition as the standard `realloc`_ function.

.. c:var:: void (*tmx_free_func) (void *address)

   Same definition as the standard `free`_ function.

.. _realloc: https://en.cppreference.com/w/c/memory/realloc
.. _free:    https://en.cppreference.com/w/c/memory/free

Example, compare how many times memory has been allocated vs freed to detect a memory leak:

.. code-block:: c

   static int alloc_counter = 0;

   void* dbg_alloc(void *address, size_t len) {
     if (!address) alloc_counter++; /* ignores reallocs */
     return realloc(address, len);
   }

   void dbg_free(void *address) {
     if (address) alloc_counter--;
     free(address);
   }

   int main(int argc, char *argv[]) {
     tmx_alloc_func = dbg_alloc;
     tmx_free_func  = dbg_free;
     /* ... load/free maps and tilesets ...*/
     if (alloc_counter != 0) {
       printf("There are %d not freed memory allocations\n", alloc_counter);
     }
   }

.. _image-autoload-autofree:

Image Autoload/Autofree
-----------------------

Set (there is no default implementation) load and free functions to load/free images automatically as they are read in
the map source.

.. c:var:: void* (*tmx_img_load_func) (const char *path)

   Load the resource (image) at the given path, return a pointer to void.
   The returned value is then stored in a :c:member:`tmx_image.resource_image`.

.. c:var:: void (*tmx_img_free_func) (void *address)

   Free the resource (image) at the given memory address,
   that resource was created previously using :c:data:`tmx_img_load_func`.

Note: these functions should probably use the :c:data:`tmx_alloc_func` and :c:data:`tmx_free_func` functions.

Example, read the file into a buffer (will probably not fit your use case, but provided as a demonstration):

.. code-block:: c

   struct rc_image {
     size_t data_length;
     unsigned char *data;
   };

   void free_img(void *address) {
     tmx_free_func(((struct rc_image *)address)->data);
     tmx_free_func(address);
   }

   void* load_img(const char *path) {
     FILE *fd = fopen(path, "rb");
     if (fd == NULL) {
       return NULL;
     }

     struct rc_image *res = tmx_alloc_func(NULL, sizeof(struct rc_image));
     res->data_length = 0;
     res->data = NULL;

     char buf[4096];
     int byte_count = 1;

     while (byte_count > 0) {
       byte_count = fread(buf, 1, 4096, fd);
       if (byte_count < 0) {
         break;
       }
       res->data = tmx_alloc_func(res->data, res->data_length + byte_count);
       memcpy(res->data + res->data_length, buf, byte_count);
       res->data_length += byte_count;
     }
     if (ferror(fd))
     {
       free_img(res);
       res = NULL;
     }

     fclose(fd);
     return res;
   }

   int main(int argc, char *argv[]) {
     tmx_img_load_func = load_img;
     tmx_img_free_func = free_img;
     /* ... load/free maps and tilesets ...*/
     /* tmx_image->resource_image holds the pointer returned by load_img. */
   }
