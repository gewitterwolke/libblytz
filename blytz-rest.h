#ifndef _BLYTZ_REST_H_
#define _BLYTZ_REST_H_

//#include <cstdint>
#include <map>

#define BLYTZ_USER_AGENT "blytz"
#define BLYTZ_COOKIE_FILE "/tmp/curl.cookie"

#define BLYTZ_DEFAULT_SERVER "https://blytz.tobik.eu"

// default 24 + null-terminator
#define BLYTZ_SESSIONID_STR_LEN 25

namespace blytz {
	
	// 
	typedef enum {
		NO_SESSIONID = 1
	} rest_error;

	// HTTP response codes for blytz errors
	typedef enum {
		HTTP_OK = 200,
		NO_CONTENT = 204,
		NO_ENTRY = 206,
		NOT_MODIFIED = 304,
		NO_CONNECTION = 404
	} http_error;

	inline std::string rest_error_to_str(rest_error err) {
		switch(err) {
			case NO_SESSIONID:
				return "No session id found";
			default:
				return "No error message";
		}
	}

	typedef std::map<std::string, std::string> headermap;

	typedef struct {
		int code;
		std::string body;
		headermap headers;
	} rest_response;

	rest_response rest_post(std::string url, std::string data, 
			const char content_type[] = "Content-Type: application/json");

	/*
	rest_response rest_post(std::string url, std::string data, 
			std::string content_type = "Content-Type: application/json");
			*/

	rest_response rest_put(std::string url, std::string data, 
			std::string content_type = "Content-Type: application/json");

	rest_response rest_get(std::string url);

	const char *get_sessionid();

	void delete_cookie();
	void set_cookiefile_permissions();

} // namespace blytz
#endif
