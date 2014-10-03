#include <string.h>

#include "blytz-base64.h"

#include <gtest/gtest.h>

TEST(Base64Test, EncLength) {
	//const char *str = "ihehsdifjslkdfjllskdjflkdsjflkjsflkdsjfldslla sdjflsd jflkds jfldsjlkfsk";
	const char *str = "fsk";

	unsigned int enclen = b64_get_encoded_len(str, true);

	char *enc = b64_encode(str);
	EXPECT_EQ(strlen(enc), enclen);
}

TEST(Base64Test, EncodeEqDecode) {
	const char *str = "ihehsdifjslkdfjllskdjflkdsjflkjsflkdsjfldslla sdjflsd jflkds jfldsjlkfsk";

	char *enc = b64_encode(str);
	char *dec = b64_decode(enc);

	EXPECT_STREQ(str, dec);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
	return 0;
}
