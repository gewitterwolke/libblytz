struct retval {
	blytz_err error;
	char message[MAX_MSG_SIZE];
};

void blytz_set_server_url(const char *url);

void blytz_set_application_name(const char*);
void blytz_set_identifier(const char*);

// transport encryption password
void blytz_set_encryption_pwd(const char *);
int blytz_has_encryption_pwd();
const char *blytz_get_encryption_pwd();
int blytz_is_encrypted();

retval blytz_init();

retval blytz_set_credentials();
retval blytz_set_credentials(const char *user, const char *pwd);
retval blytz_get_credentials();
retval blytz_get_password();
retval blytz_get_username();

int blytz_has_connection();
int blytz_has_connection_old();
int blytz_has_credentials();
int blytz_has_credentials_old();
