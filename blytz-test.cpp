#include <string.h>

#include "blytz-base64.h"

#include <gtest/gtest.h>

TEST(Base64Test, EncLength) {
	// caution: newline at the end mandatory for this test
	const char *str = "ihehsdifjslkdfjllskdjflkdsjflkjsflkdsjfldslla sdjflsd jflkds jfldsjlkfsk\n";

	unsigned int enclen = b64_get_encoded_len(str, true);
	char *enc = b64_encode(str);

	EXPECT_EQ(strlen(enc), enclen);
	free(enc);
}

TEST(Base64Test, EncodeEqDecode) {
	const char *str = "sdjflsdjflkdsjfldsjlkfsk";

	char *enc = b64_encode(str);
	char *dec = b64_decode(enc);

	EXPECT_STREQ(str, dec);

	free(enc);
	free(dec);

	const char *str2 = "ihehsdifjslkdfjllskdjflkdsjflkjsflkdsjfldslla sdjflsd jflkds jfldsjlkfsk";

	enc = b64_encode(str2);
	dec = b64_decode(enc);

	EXPECT_STREQ(str2, dec);

	free(enc);
	free(dec);
}

TEST(Base64Test, EncodeSample1) {
	EXPECT_STREQ(b64_encode("fsk"), "ZnNrCg==");
	EXPECT_STREQ(b64_encode("asdf"), "YXNkZgo=");
	EXPECT_STREQ(b64_encode("weoir"), "d2VvaXIK");
	EXPECT_STREQ(b64_encode("fskaslkdsajfldsjfldsjfdslkjfdhehesdflkjfdldlsdkjfldsjfljdslkjfdslksj"), 
			"ZnNrYXNsa2RzYWpmbGRzamZsZHNqZmRzbGtqZmRoZWhlc2RmbGtqZmRsZGxzZGtq\nZmxkc2pmbGpkc2xramZkc2xrc2oK");
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
	return 0;
}
