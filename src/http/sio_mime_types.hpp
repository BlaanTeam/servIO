#ifndef __MIME_TYPES_H__
#define __MIME_TYPES_H__

#include <map>
#include <string>

#define DEFAULT_MIME_TYPE "text/plain"

using namespace std;

class MimeType : public map<string, const char *> {
	typedef map<string, const char *> Base;

   public:
	MimeType();

	mapped_type &operator[](const key_type &key);
};

extern MimeType mimeTypes;

#endif