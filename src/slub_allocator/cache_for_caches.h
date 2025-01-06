#ifndef CACHE_FOR_CACHES_H
#define CACHE_FOR_CACHES_H

#include "memory_cache.h"

MemoryCache& getCacheForCaches();
MemoryCache& getCacheForSlabHeaders();

#endif //CACHE_FOR_CACHES_H