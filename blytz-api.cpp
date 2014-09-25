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

		FILE *f = fopen("/tmp/debugapi.txt", "a");

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

		fprintf( f, "BLYTZ-API - Connect url %s\n", connect_url.c_str());
		
		res.code = HTTP_OK;
		res = rest_get( connect_url);

		if (res.code == HTTP_OK) {

			fprintf( f, "BLYTZ_API - Connect returned HTTP_OK\n");
			fclose(f);

			initialized = true;

			ret.error = OK;
			return ret;
		}

		fprintf( f, "Error connecting to BLYTZ server\n");
		fclose(f);

		strncpy(ret.message, "Error connecting to BLYTZ server", MAX_MSG_SIZE);
		return ret;
	}

	void cleanup() {

	}

	void set_server_url(std::string url) {
		FILE *f = fopen("/tmp/debugapi.txt", "a");
		blytz_settings.server_url = url;		
		fprintf(f,"BLYTZ-API - set server url %s\n", url.c_str());
		fclose(f);
	}

	void set_server_url(const char *url) {
		FILE *f = fopen("/tmp/debugapi.txt", "a");
		blytz_settings.server_url = url;
		fprintf(f,"BLYTZ-API - set server url %s\n", url);
		fclose(f);
	}

	void set_application_name(const char* name) {
		blytz_settings.name = name;
	}

	void set_application_name(std::string name) {
		blytz_settings.name = name;
	}

	void set_identifier(const char *id) {
		FILE *f = fopen("/tmp/debugapi.txt", "a");
		fprintf(f,"BLYTZ-API - set identifier %s\n", id);
		blytz_settings.identifier = id;
		fclose(f);
	}

	void set_identifier(std::string id) {
		FILE *f = fopen("/tmp/debugapi.txt", "a");
		fprintf(f,"BLYTZ-API - set identifier %s\n", id.c_str());
		blytz_settings.identifier = id;
		fclose(f);
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

		FILE *f = fopen("/tmp/debugapi.txt", "a");

		rest_response res;
		res = rest_get(conn_str);
		fprintf( f, "get, code: %d\n", res.code);

		if (res.code == NO_ENTRY) {
			ret.error = NO_PASSWORD;
		} else if (res.code == HTTP_OK || res.code == NOT_MODIFIED) {
			ret.error = OK;
			fprintf( f, "get, %s\n", res.body.c_str());
		
			strncpy(ret.message, res.body.c_str(), MAX_MSG_SIZE);

			if (is_encrypted()) {

				if (blytz_settings.has_encpwd) {
					fprintf(f, "Has encryption password\n");
					fflush(f);
				} else { 
					fprintf(f, "Has no encryption password\n");
					fflush(f);
				}

				if (!has_encryption_pwd()) {

					fprintf(f, "Encryption enabled on server side but no password specified\n");
					ret.error = NO_PASSWORD;
					strncpy(ret.message, 
							"Encryption enabled on server side but no password specified", MAX_MSG_SIZE);
					return ret;
				}

				// we have an encrypted message and a password

				fprintf(f, "Encrypted ret.message: %s\n", ret.message);
				fflush(f);

				const char *dec = decrypt(ret.message, get_encryption_pwd());
				
				fprintf(f, "Decrypted ret.message: %s\n", dec);
				fflush(f);
				strncpy(ret.message, dec, MAX_MSG_SIZE);
			}

		} else {

			strcpy(ret.message, "Error getting credentials");
			ret.error = GENERIC_ERROR;
		}

		fclose(f);
		return ret;
	}

  retval get_password() {

		FILE *f = fopen("/tmp/debugapi.txt", "a");

		retval ret;
		ret.error = OK;
		std::string conn_str = blytz_settings.server_url + "/credentials/password";

		if (has_credentials()) {
			rest_response res = rest_get(conn_str);

			strncpy(ret.message, res.body.c_str(), MAX_MSG_SIZE);
			fprintf(f, "get_password: %s\n", ret.message);
			fflush(f);

			if (is_encrypted()) {
				fprintf(f, "decrypting\n");
				fflush(f);
				const char *dec = decrypt(ret.message, get_encryption_pwd());
				fprintf(f, "decrypted: %s\n", dec);
				fflush(f);
				strncpy(ret.message, dec, MAX_MSG_SIZE);
			}
	
			fclose(f);
			return ret;
		}

		fclose(f);
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
			FILE *f = fopen("/tmp/debugapi.txt", "a");
			fprintf(f, "get_password: %s\n", ret.message);

			if (is_encrypted()) {
				fprintf(f, "decrypting");
				const char *dec = decrypt(ret.message, get_encryption_pwd());
				fprintf(f, "decrypted: %s\n", dec);
				fflush(f);
				strncpy(ret.message, dec, MAX_MSG_SIZE);
			}
	
			fclose(f);
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

		// set POST data
		std::stringstream sts;
		sts << "{\n\"username\":\"" << user << "\",\n\"password\":\"" <<
			pwd << "\", \n\"status\":\"widgetsetcredentials\",\n" << 
			formatstr << "\n}";
			//pwd << "\"\n}"; //,\n\"status\":\"widgetsetcredentials\"\n}";
		std::string postdata = sts.str();

		// send to blytz server (-> app)
		std::string conn_str = blytz_settings.server_url + "/credentials/set";

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

		FILE *f = fopen("/tmp/debugapi.txt", "a");
		fprintf(f,"encryption state %d\n", res.code);
		fclose(f);
		return res.code == HTTP_OK;
	}

	bool has_encryption_pwd() {
		return blytz_settings.has_encpwd;
	}
} // namespace blytz

