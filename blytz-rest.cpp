#include <string>
#include <fstream>
#include <cstdlib>

#include <memory.h>

// TODO: should this (and chmod(), unlink()) be here?
#include <sys/stat.h>
#include <unistd.h>

#include <curl/curl.h>

#include "blytz-rest.h"
#include "blytz-api.h"
#include "blytz-debug.h"

namespace blytz {

	extern bool initialized;

	std::string cookie_filename = BLYTZ_COOKIE_FILE;

	bool has_cookie = false;

	int write_callback(void *data, size_t size, size_t nmemb, void *userdata) {

		static int count = 0;
		count++;

		rest_response *r;
		r = (rest_response*) userdata;
		r->body.append((char*) data, size * nmemb);

		//FILE *f = fopen("/tmp/debugrestwrite.txt", "a");
		printfd("WRITE count %d\n", count);
		printfd("WRITE addr: %p\n", &r->body);
		printfd("WRITE cstr: %s\n", (char *)r->body.c_str());
		//fclose(f);

		return (size * nmemb);
	}

	void rest_set_curl_options(CURL *curl) {

		// set callback function
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

		// user agent
		curl_easy_setopt(curl, CURLOPT_USERAGENT, BLYTZ_USER_AGENT);

		// add cookie
		if (has_cookie) {
			curl_easy_setopt( curl, CURLOPT_COOKIEFILE, cookie_filename.c_str());
		} else {
			curl_easy_setopt( curl, CURLOPT_COOKIEJAR, cookie_filename.c_str());
			has_cookie = true;
			set_cookiefile_permissions();
		}

		// disable ssl verification for now
		curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt( curl, CURLOPT_SSL_VERIFYHOST, 0);
	}

	rest_response rest_post(std::string url, std::string data, 
			const char content_type[]) {

		CURLcode res;

		rest_response ret;

		CURL *curl = curl_easy_init();

		if (curl) {

			// set default curl options
			rest_set_curl_options(curl);

			// query URL
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			curl_easy_setopt(curl, CURLOPT_POST, 1L);

			// post fields
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.size());

			// data object to pass to callback function
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);

			// set content-type header
			curl_slist* header = NULL;
			header = curl_slist_append(header, content_type);
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);

			// perform post
			res = curl_easy_perform(curl);

			long http_code = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

			ret.code = (int) http_code;

			curl_slist_free_all(header);
			curl_easy_cleanup(curl);
			curl_global_cleanup();
		}

		return ret;
	}

	rest_response rest_get(std::string url) {

		rest_response ret;

		if (!initialized) {

			// FIXME
			ret.code = 501;
		}

		CURLcode res;

		CURL *curl = curl_easy_init();

		if (curl) {
			printfd("GET url %s\n", url.c_str());

			// set default curl options
			rest_set_curl_options(curl);

			// query URL
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			// data object to pass to callback function
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);

			// perform get
			res = curl_easy_perform(curl);

			long http_code = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

			ret.code = (int) http_code;

			printfd("GET code %d\n", ret.code);
			printfd("GET addr %p\n", &ret.body);
			printfd( "GET len %lud\n", ret.body.length());

			curl_easy_cleanup(curl);
			curl_global_cleanup();
		}

		return ret;
	}

	void set_cookiefile_permissions() {

		// restrict access to cookie file, just in case
		chmod(cookie_filename.c_str(), S_IRUSR | S_IWUSR);
	}

	void delete_cookie() {

		has_cookie = false;
		unlink(cookie_filename.c_str());
	}

	// get the session id from CURL's session cookie
	const char *get_sessionid() {
		std::string sessionid;

		printfd("BLYTZ-API - in get_sessionid()\n");

		// FIXME 
		const char pre_sid_garbage[] = "s\%3A";

		std::ifstream cookie_file;
		cookie_file.open(cookie_filename.c_str());

		printfd("BLYTZ-API - in get_sessionid2()\n");

		if (!cookie_file.is_open()) {
			return rest_error_to_str(NO_SESSIONID).c_str();
			printfd("BLYTZ-API - failed to open cookie file %s\n",
					cookie_filename.c_str()); 
		}

		printfd("BLYTZ-API - opened cookie file %s\n", cookie_filename.c_str());

		std::string line;

		while (!cookie_file.eof() && !cookie_file.fail()) {

			std::getline(cookie_file, line);
			size_t pos_sid;

			if ((pos_sid = line.find(pre_sid_garbage)) != std::string::npos) {

				pos_sid += strlen(pre_sid_garbage);
				size_t pos_end_sid = line.find_first_of(".", pos_sid);

				if (pos_end_sid != std::string::npos) { 

					size_t sid_len = pos_end_sid - pos_sid;
					sessionid = line.substr(pos_sid, sid_len);
					// FIXME: Sessionid not in public log file
					printfd( "BLYTZ-API - found session id %s\n", sessionid.c_str());

					cookie_file.close();
					break;
				}
			}
		}

		cookie_file.close();
		printfd("BLYTZ-API - closed cookie file %s\n", cookie_filename.c_str());

		// copy, user has to delete it
		printfd("BLYTZ-API - length of sessionid string %lu\n", sessionid.size());
		char *sessionid_ch = (char *) malloc(BLYTZ_SESSIONID_STR_LEN);
		//memcpy(sessionid_ch, sessionid.c_str(), sessionid.size());
		sessionid_ch = strncpy(sessionid_ch, sessionid.c_str(), 
				BLYTZ_SESSIONID_STR_LEN);
		//strcpy(sessionid_ch, sessionid.c_str());

		printfd( "BLYTZ-API - returning session id %s\n", sessionid_ch);
		return sessionid_ch;
	}
}
