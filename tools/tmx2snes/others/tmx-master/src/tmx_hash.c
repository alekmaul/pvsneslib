/*
	Hashtable

	This implementation is based on libxml/hash.h provided by libxml2.
*/

#include <libxml/hash.h>

#include "tmx_utils.h"

void* mk_hashtable(unsigned int initial_size) {
	// Auto-resize is supported
	setup_libxml_mem();
	return (void*)xmlHashCreate(initial_size);
}

void hashtable_set(void *hashtable, const char *key, void *val, hashtable_entry_deallocator deallocator) {
	// Set or update value, key string is duplicated, deallocator may be NULL if values were not allocated
	xmlHashUpdateEntry((xmlHashTablePtr)hashtable, (const xmlChar*)key, val, (xmlHashDeallocator)deallocator);
}

void* hashtable_get(void *hashtable, const char *key) {
	return xmlHashLookup((xmlHashTablePtr)hashtable, (const xmlChar*)key);
}

void hashtable_rm(void *hashtable, const char *key, hashtable_entry_deallocator deallocator) {
	xmlHashRemoveEntry((xmlHashTablePtr)hashtable, (const xmlChar*)key, (xmlHashDeallocator)deallocator);
}

void free_hashtable(void *hashtable, hashtable_entry_deallocator deallocator) {
	xmlHashFree((xmlHashTablePtr)hashtable, (xmlHashDeallocator)deallocator);
}

void hashtable_foreach(void *hashtable, hashtable_foreach_functor functor, void *userdata) {
	xmlHashScan((xmlHashTablePtr)hashtable, (xmlHashScanner)functor, userdata);
}
