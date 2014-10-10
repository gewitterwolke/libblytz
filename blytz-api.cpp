// BLYTZ API
// 
// TODO: C Compatibility

#include <string>
#include <sstream>
#include <fstream>

#include <cstdlib>
#include <cstring>

#include "blytz-api.h"
#include "blytz-rest.h"
#include "blytz-enc.h"
#include "blytz-debug.h"

// usage:
// (optional) set_server_url
// set_application_name
// set_identifier
// set_encryption_pwd (if data should be encrypted)
// init()
// get_sessionid()
// get_qrcode_xyz()
// when telling the server new login credentials:
//  set_credentials(user,key)
// get_credentials()
// 
// ???
//
// profit

namespace blytz {
	bool initialized = false;
	settings blytz_settings;

	// initial connect to blytz server
	// (to obtain sessionid and to set application fingerprint/identifier)
	retval init() {

		retval ret;
		initialized = false;

		if (blytz_settings.server_url.length() == 0) {
			blytz_settings.server_url = BLYTZ_DEFAULT_SERVER;
		}

		if (blytz_settings.has_encpwd) {
			//
		}

		rest_response res;


		/*
		// FIXME: only SSH
		std::string connect_url = blytz_settings.server_url + "/connectSSH";

		fprintf( f, "conn url: %s\n", connect_url.c_str());
		fflush(f);

		// old
		std::stringstream sts;
		sts << "{\n\"hostname\":\"" << blytz_settings.identifier << "\"\n}";
		std::string post_data = sts.str();

		res = rest_post( connect_url, post_data);
		*/

		// new REST scheme for "first contact"
		std::string path = "/" + blytz_settings.name + "/" + blytz_settings.identifier;
		std::string connect_url = blytz_settings.server_url + "/connect" + path;

		printfd( "BLYTZ-API - Connect url %s\n", connect_url.c_str());
		
		res.code = HTTP_OK;
		res = rest_get( connect_url);

		if (res.code == HTTP_OK) {

			printfd( "BLYTZ_API - Connect returned HTTP_OK\n");

			initialized = true;

			ret.error = OK;
			return ret;
		}

		printfd( "Error connecting to BLYTZ server\n");

		strncpy(ret.message, "Error connecting to BLYTZ server", MAX_MSG_SIZE);
		return ret;
	}

	void cleanup() {

	}

	void set_server_url(std::string url) {
		blytz_settings.server_url = url;		
		printfd( "BLYTZ-API - set server url %s\n", url.c_str());
	}

	void set_server_url(const char *url) {
		blytz_settings.server_url = url;
		printfd("BLYTZ-API - set server url %s\n", url);
	}

	void set_application_name(const char* name) {
		blytz_settings.name = name;
	}

	void set_application_name(std::string name) {
		blytz_settings.name = name;
	}

	void set_identifier(const char *id) {
		printfd( "BLYTZ-API - set identifier %s\n", id);
		blytz_settings.identifier = id;
	}

	void set_identifier(std::string id) {
		printfd( "BLYTZ-API - set identifier %s\n", id.c_str());
		blytz_settings.identifier = id;
	}

	std::string create_password() {
		return "hehe";		
	}

	bool has_connection_old() {

		std::string conn_str = blytz_settings.server_url + "/credentials/get";
		rest_response res;

		res = rest_get(conn_str);

		if (res.code == HTTP_OK) {
			std::string creds = res.body;			
			if (creds.find("appSessionId") != std::string::npos) {
				return true;
			}
		}

		return false;
	}

	bool has_credentials_old() {

		std::string conn_str = blytz_settings.server_url + "/credentials/get";

		rest_response res;
		res = rest_get(conn_str);
		return res.code == HTTP_OK;
	}

	bool has_connection() {
		std::string conn_str = blytz_settings.server_url + "/hasConnection";
		rest_response res = rest_get(conn_str);

		return res.code == HTTP_OK;
	}

	bool has_credentials() {
		std::string conn_str = blytz_settings.server_url + "/hasCredentials";
		rest_response res = rest_get(conn_str);

		return res.code == HTTP_OK;
	}

	retval get_credentials() {

		retval ret;
		ret.error = OK;
		std::string conn_str = blytz_settings.server_url + "/credentials/get";

		rest_response res;
		res = rest_get(conn_str);
		printfd( "get, code: %d\n", res.code);

		if (res.code == NO_ENTRY) {
			ret.error = NO_PASSWORD;
		} else if (res.code == HTTP_OK || res.code == NOT_MODIFIED) {
			ret.error = OK;

			printfd( "get, %s\n", res.body.c_str());
		
			strncpy(ret.message, res.body.c_str(), MAX_MSG_SIZE);

			if (is_encrypted()) {

				if (blytz_settings.has_encpwd) {
					printfd( "Has encryption password\n");
				} else { 
					printfd( "Has no encryption password\n");
				}

				if (!has_encryption_pwd()) {

					printfw( "Encryption enabled on server side but no password specified\n");
					ret.error = NO_PASSWORD;
					strncpy(ret.message, 
							"Encryption enabled on server side but no password specified", MAX_MSG_SIZE);
					return ret;
				}

				// we have an encrypted message and a password
				printfd( "Encrypted ret.message: %s\n", ret.message);

				const char *dec = decrypt(ret.message, get_encryption_pwd());
				
				printfd( "Decrypted ret.message: %s\n", dec);
				strncpy(ret.message, dec, MAX_MSG_SIZE);
			}

		} else {

			strcpy(ret.message, "Error getting credentials");
			ret.error = GENERIC_ERROR;
		}

		return ret;
	}

  retval get_password() {

		retval ret;
		ret.error = OK;
		std::string conn_str = blytz_settings.server_url + "/credentials/password";

		if (has_credentials()) {
			rest_response res = rest_get(conn_str);

			strncpy(ret.message, res.body.c_str(), MAX_MSG_SIZE);
			printfd("get_password: %s\n", ret.message);

			if (is_encrypted()) {
				printfd("decrypting\n");
				const char *dec = decrypt(ret.message, get_encryption_pwd());
				printfd("decrypted: %s\n", dec);
				strncpy(ret.message, dec, MAX_MSG_SIZE);
			}
	
			return ret;
		}

		ret.error = NO_PASSWORD;
		return ret;
	}

  retval get_username() {

		retval ret;
		ret.error = OK;
		std::string conn_str = blytz_settings.server_url + "/credentials/username";

		if (has_credentials()) {
			rest_response res = rest_get(conn_str);

			strncpy(ret.message, res.body.c_str(), MAX_MSG_SIZE);
			printfd("get_password: %s\n", ret.message);

			if (is_encrypted()) {
				printfd("decrypting");
				const char *dec = decrypt(ret.message, get_encryption_pwd());
				printfd("decrypted: %s\n", dec);
				strncpy(ret.message, dec, MAX_MSG_SIZE);
			}
	
			return ret;
		}

		ret.error = NO_PASSWORD;
		return ret;
	}

	retval set_credentials(const char *user, const char *pwd) {
		return set_credentials(std::string(user), std::string(pwd));
	}

	retval set_credentials(std::string user, std::string pwd) {

		retval ret;
		ret.error = OK;

		if (user.length() == 0 || pwd.length() == 0) {

			strncpy(ret.message, "Wrong arguments for set_credentials", MAX_MSG_SIZE);
			ret.error = WRONG_ARGUMENTS;
			return ret;
		}

		// encrypt password data
		const char *formatstr;
		if (has_encryption_pwd()) {
			const char *encpwd = encrypt(pwd.c_str(), blytz_settings.encpwd.c_str());
			pwd = encpwd;
			formatstr = "\"format\":\"encrypted\"";
		} else {
			formatstr = "\"format\":\"plain\"";
		}

		printfd("Encryption successful\n");

		// set POST data
		std::stringstream sts;
		sts << "{\n\"username\":\"" << user << "\",\n\"password\":\"" <<
			pwd << "\", \n\"status\":\"widgetsetcredentials\",\n" << 
			formatstr << "\n}";
			//pwd << "\"\n}"; //,\n\"status\":\"widgetsetcredentials\"\n}";
		std::string postdata = sts.str();

		// send to blytz server (-> app)
		std::string conn_str = blytz_settings.server_url + "/credentials/set";

		printfd("Setting credentials");

		rest_response res;
		res = rest_post(conn_str, postdata);

		if (res.code != HTTP_OK) {
			ret.error = GENERIC_ERROR;
			strncpy(ret.message, "Error settings credentials", MAX_MSG_SIZE);
		}

		return ret;
	}

	retval set_credentials() {
		return set_credentials(blytz_settings.user, blytz_settings.pwd);
	}

	void set_username(std::string username) {
		blytz_settings.user = username;
	}

	void set_username(const char *username) {
		blytz_settings.user = username;
	}

	void set_password(std::string password) {
		blytz_settings.pwd = password;
	}

	void set_password(const char *password) {
		blytz_settings.pwd = password;
	}

	const char *get_encryption_pwd() {
		return blytz_settings.encpwd.c_str();
	}

	void set_encryption_pwd(const char *password) {
		blytz_settings.encpwd = password;
		blytz_settings.has_encpwd = true;
	}
	
	void set_encryption_pwd(std::string password) {
		blytz_settings.encpwd = password;
		blytz_settings.has_encpwd = true;
	}

  // returns yes if current credentials are encrypted
	bool is_encrypted() {
		std::string conn_str = blytz_settings.server_url + "/credentials/encrypted";

		rest_response res;
		res = rest_get(conn_str);

		printfd("encryption state %d\n", res.code);
		return res.code == HTTP_OK;
	}

	bool has_encryption_pwd() {
		return blytz_settings.has_encpwd;
	}
} // namespace blytz

