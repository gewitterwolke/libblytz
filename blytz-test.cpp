#include <string.h>

#include "blytz-base64.h"
#include "blytz-enc.h"

#include <gtest/gtest.h>

using namespace blytz;

// TEST(Base64Test, EncLength) {
// 	// caution: newline at the end mandatory for this test
// 	const char *str = "ihehsdifjslkdfjllskdjflkdsjflkjsflkdsjfldslla sdjflsd jflkds jfldsjlkfsk\n";
// 
// 	//unsigned int enclen = b64_get_encoded_len(str, true);
// 	char *enc = b64_encode(str);
// 
// 	//EXPECT_EQ(strlen(enc), enclen);
// 	free(enc);
// }

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
	EXPECT_STREQ(b64_encode("fsk", 3), "ZnNrCg==");
	EXPECT_STREQ(b64_encode("asdf", 4), "YXNkZgo=");
	EXPECT_STREQ(b64_encode("weoir", 5), "d2VvaXIK");

	EXPECT_STREQ(b64_encode("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrs", 45), 
				"YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXphYmNkZWZnaGlqa2xtbm9wcXJzCg==");

	EXPECT_STREQ(b64_encode("fskaslkdsajfldsjfldsjfdslkjfdhehesdflkjfdldlsdkjfldsjfljdslkjfdslksj", 68), 
			"ZnNrYXNsa2RzYWpmbGRzamZsZHNqZmRzbGtqZmRoZWhlc2RmbGtqZmRsZGxzZGtq\nZmxkc2pmbGpkc2xramZkc2xrc2oK");
}

TEST(EncryptTest, EncryptSample1) {
	const char *str = "asdfjkl";
	const char *pwd = "test123";

	const char *enc = encrypt(str, pwd);
  const char *dec = decrypt(enc, pwd);

	EXPECT_STREQ(dec, str);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
	return 0;
}
