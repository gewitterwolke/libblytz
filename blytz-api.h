#ifndef _BLYTZ_API_H_
#define _BLYTZ_API_H_

#include "blytz-rest.h"
#include <string>

namespace blytz {

	struct settings {
		std::string server_url;
		// identifies the application (e.g. ssh)
		std::string name;
		// addition identifier like ip/hostname etc
		std::string identifier;
		std::string user;
		std::string pwd;
		// password for (transport) encryption of data
		std::string encpwd;
		bool has_encpwd;

		settings() : has_encpwd(false) {};
	};

	extern settings blytz_settings;

	typedef enum {
		PRIVATE = 0,
		PUBLIC = 1
	} privacy_mode;

	typedef enum {
		OK = 0,
		GENERIC_ERROR = 1,
		SERVER_UNAVAILABLE = 2,
		OTHER_ERROR = 3,
		WRONG_ARGUMENTS = 4,
		NO_PASSWORD = 5
	} blytz_err;

#define MAX_MSG_SIZE 10000
#define MAX_ENC_PWD_LEN 25

	struct retval {
		blytz_err error;
		char message[MAX_MSG_SIZE];

		retval() : error(GENERIC_ERROR), message() {};
	};

	void set_server_url(std::string url);
	void set_server_url(const char *url);

	void set_application_name(const char*);
	void set_application_name(std::string name);
	void set_identifier(const char*);
	void set_identifier(std::string id);
	
	// transport encryption password
	void set_encryption_pwd(const char *);
	void set_encryption_pwd(std::string );
	bool has_encryption_pwd();
	const char *get_encryption_pwd();
	bool is_encrypted();

	retval init();

	retval set_credentials();
	retval set_credentials(const char *user, const char *pwd);
	retval set_credentials(std::string user, std::string pwd);
	retval get_credentials();
  retval get_password();
  retval get_username();

	bool has_connection();
	bool has_connection_old();
	bool has_credentials();
	bool has_credentials_old();

}
#endif
