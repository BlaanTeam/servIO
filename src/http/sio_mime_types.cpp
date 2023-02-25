#include "sio_mime_types.hpp"

MimeType::MimeType(void) {
	insert(make_pair("html", "text/html"));
	insert(make_pair("htm", "text/html"));
	insert(make_pair("shtml", "text/html"));
	insert(make_pair("css", "text/css"));
	insert(make_pair("xml", "text/xml"));
	insert(make_pair("gif", "image/gif"));
	insert(make_pair("jpeg", "image/jpeg"));
	insert(make_pair("jpg", "image/jpeg"));
	insert(make_pair("js", "application/javascript"));
	insert(make_pair("atom", "application/atom+xml"));
	insert(make_pair("rss", "application/rss+xml"));
	insert(make_pair("mml", "text/mathml"));
	insert(make_pair("txt", "text/plain"));
	insert(make_pair("jad", "text/vnd.sun.j2me.app-descriptor"));
	insert(make_pair("wml", "text/vnd.wap.wml"));
	insert(make_pair("htc", "text/x-component"));
	insert(make_pair("avif", "image/avif"));
	insert(make_pair("png", "image/png"));
	insert(make_pair("svg", "image/svg+xml"));
	insert(make_pair("svgz", "image/svg+xml"));
	insert(make_pair("tif", "image/tiff"));
	insert(make_pair("tiff", "image/tiff"));
	insert(make_pair("wbmp", "image/vnd.wap.wbmp"));
	insert(make_pair("webp", "image/webp"));
	insert(make_pair("ico", "image/x-icon"));
	insert(make_pair("jng", "image/x-jng"));
	insert(make_pair("bmp", "image/x-ms-bmp"));
	insert(make_pair("woff", "font/woff"));
	insert(make_pair("woff2", "font/woff2"));
	insert(make_pair("jar", "application/java-archive"));
	insert(make_pair("war", "application/java-archive"));
	insert(make_pair("ear", "application/java-archive"));
	insert(make_pair("json", "application/json"));
	insert(make_pair("hqx", "application/mac-binhex40"));
	insert(make_pair("doc", "application/msword"));
	insert(make_pair("pdf", "application/pdf"));
	insert(make_pair("ps", "application/postscript"));
	insert(make_pair("eps", "application/postscript"));
	insert(make_pair("ai", "application/postscript"));
	insert(make_pair("rtf", "application/rtf"));
	insert(make_pair("m3u8", "application/vnd.apple.mpegurl"));
	insert(make_pair("kml", "application/vnd.google-earth.kml+xml"));
	insert(make_pair("kmz", "application/vnd.google-earth.kmz"));
	insert(make_pair("xls", "application/vnd.ms-excel"));
	insert(make_pair("eot", "application/vnd.ms-fontobject"));
	insert(make_pair("ppt", "application/vnd.ms-powerpoint"));
	insert(make_pair("odg", "application/vnd.oasis.opendocument.graphics"));
	insert(make_pair("odp", "application/vnd.oasis.opendocument.presentation"));
	insert(make_pair("ods", "application/vnd.oasis.opendocument.spreadsheet"));
	insert(make_pair("odt", "application/vnd.oasis.opendocument.text"));
	insert(make_pair("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
	insert(make_pair("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
	insert(make_pair("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
	insert(make_pair("wmlc", "application/vnd.wap.wmlc"));
	insert(make_pair("wasm", "application/wasm"));
	insert(make_pair("7z", "application/x-7z-compressed"));
	insert(make_pair("cco", "application/x-cocoa"));
	insert(make_pair("jardiff", "application/x-java-archive-diff"));
	insert(make_pair("jnlp", "application/x-java-jnlp-file"));
	insert(make_pair("run", "application/x-makeself"));
	insert(make_pair("pl", "application/x-perl"));
	insert(make_pair("pm", "application/x-perl"));
	insert(make_pair("prc", "application/x-pilot"));
	insert(make_pair("pdb", "application/x-pilot"));
	insert(make_pair("rar", "application/x-rar-compressed"));
	insert(make_pair("rpm", "application/x-redhat-package-manager"));
	insert(make_pair("sea", "application/x-sea"));
	insert(make_pair("swf", "application/x-shockwave-flash"));
	insert(make_pair("sit", "application/x-stuffit"));
	insert(make_pair("tk", "application/x-tcl"));
	insert(make_pair("tcl", "application/x-tcl"));
	insert(make_pair("der", "application/x-x509-ca-cert"));
	insert(make_pair("crt", "application/x-x509-ca-cert"));
	insert(make_pair("pem", "application/x-x509-ca-cert"));
	insert(make_pair("xpi", "application/x-xpinstall"));
	insert(make_pair("xhtml", "application/xhtml+xml"));
	insert(make_pair("xspf", "application/xspf+xml"));
	insert(make_pair("zip", "application/zip"));
	insert(make_pair("bin", "application/octet-stream"));
	insert(make_pair("dll", "application/octet-stream"));
	insert(make_pair("exe", "application/octet-stream"));
	insert(make_pair("deb", "application/octet-stream"));
	insert(make_pair("dmg", "application/octet-stream"));
	insert(make_pair("iso", "application/octet-stream"));
	insert(make_pair("img", "application/octet-stream"));
	insert(make_pair("msm", "application/octet-stream"));
	insert(make_pair("msi", "application/octet-stream"));
	insert(make_pair("msp", "application/octet-stream"));
	insert(make_pair("mid", "audio/midi"));
	insert(make_pair("midi", "audio/midi"));
	insert(make_pair("kar", "audio/midi"));
	insert(make_pair("mp3", "audio/mpeg"));
	insert(make_pair("ogg", "audio/ogg"));
	insert(make_pair("m4a", "audio/x-m4a"));
	insert(make_pair("ra", "audio/x-realaudio"));
	insert(make_pair("3gpp", "video/3gpp"));
	insert(make_pair("3gp", "video/3gpp"));
	insert(make_pair("ts", "video/mp2t"));
	insert(make_pair("mp4", "video/mp4"));
	insert(make_pair("mpeg", "video/mpeg"));
	insert(make_pair("mpg", "video/mpeg"));
	insert(make_pair("mov", "video/quicktime"));
	insert(make_pair("webm", "video/webm"));
	insert(make_pair("flv", "video/x-flv"));
	insert(make_pair("m4v", "video/x-m4v"));
	insert(make_pair("mng", "video/x-mng"));
	insert(make_pair("asx", "video/x-ms-asf"));
	insert(make_pair("asf", "video/x-ms-asf"));
	insert(make_pair("wmv", "video/x-ms-wmv"));
	insert(make_pair("avi", "video/x-msvideo"));

	(*this)[""] = DEFAULT_MIME_TYPE;
}

#include <libgen.h>

MimeType::mapped_type &MimeType::choiceMimeType(const string &path) {
	
	char bname[PATH_MAX] = {0};
	basename_r(path.c_str(), bname);

	size_t idx = path.find_last_of('.');

	if (idx == string::npos)
		return (*this)[""];
	string ext = path.substr(idx + 1);
	
	return (*this)[ext];
}

MimeType::mapped_type &MimeType::operator[](const key_type &key) {
	return find(key) != end() ? Base::operator[](key) : Base::operator[]("");
}

MimeType mimeTypes;
