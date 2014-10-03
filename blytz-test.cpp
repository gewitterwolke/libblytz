#include <string.h>

#include "blytz-base64.h"

bool test_b64() {

	const char *str = "ihehsdifjslkdfjllskdjflkdsjflkjsflkdsjfldslla\
			sdjflsd jflkds jfldsjlkfsk";

	char *enc = b64_encode(str);
	char *dec = b64_decode(enc);

	if (strcmp(enc, dec)) {
		return false;
	} else {
		return true;
	}

}
