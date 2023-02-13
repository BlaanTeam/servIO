#ifndef __MIME_TYPES_H__
#define __MIME_TYPES_H__

#include <map>
#include <string>

#define DEFAULT_MIME_TYPE "text/plain"

using namespace std;

class MimeType : public map<const string, const char *> {
    public:
    // member types;
    typedef map<const string, const char *>::iterator iterator;
   public:
	MimeType();
	const char *getMimeType(const string &ext);
};

extern MimeType mimeTypes;


const string *getFileExtension(const char *file);

#endif