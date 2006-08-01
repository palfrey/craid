#ifndef UUIDHASH
#define UUIDHASH  1

#include <glib.h>

extern guint qsf_uuid_hash(gconstpointer key);
gint qsf_uuid_compare(uuid_t a, uuid_t b);

#endif
