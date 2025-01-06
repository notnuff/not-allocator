#include "cache_for_caches.h"


// this is trick so our static variable is initialized
// before used

MemoryCache& getCacheForCaches() {
  static MemoryCache cache_for_caches({
    sizeof(MemoryCache),
    alignof(MemoryCache),
    true,
  });
  return cache_for_caches;
}

MemoryCache& getCacheForSlabHeaders() {
  static MemoryCache cache_for_slabs_headers({
      sizeof(MemorySlab),
      alignof(MemorySlab),
      true,
  });
  return cache_for_slabs_headers;
}