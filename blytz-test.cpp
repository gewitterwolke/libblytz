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

// TEST(Base64Test, EncodeEqDecode) {
// 	const char *str = "sdjflsdjflkdsjfldsjlkfsk";
// 
// 	char *enc = b64_encode(str);
// 	char *dec = b64_decode(enc);
// 
// 	EXPECT_STREQ(str, dec);
// 
// 	free(enc);
// 	free(dec);
// 
// 	const char *str2 = "ihehsdifjslkdfjllskdjflkdsjflkjsflkdsjfldslla sdjflsd jflkds jfldsjlkfsk";
// 
// 	enc = b64_encode(str2);
// 	dec = b64_decode(enc);
// 
// 	EXPECT_STREQ(str2, dec);
// 
// 	free(enc);
// 	free(dec);
// 
// 	const char *str3 = "lasjfdlkjdsflkdsjflkdsjfldskjflsdkjfljlksdjfldskjfsdflkdsjadflsdakjflkdsjflkdsjfldsjfldsjfkldsjfldsjfkdsjflkdsjfkkdsldsfljdslkfjaew;oxmmpoinewwwwjfcx;lewjfxljdslkfjsfdslkjdsflkjf";
// 
// 	unsigned int tmp = strlen(str3);
// 	enc = b64_encode(str3, tmp, false);
//   dec = b64_decode(enc, &tmp);
// 
// 	EXPECT_STREQ(str3,dec);
// }

TEST(Base64Test, EncodeEqDecode2) {
	// const char *str = "\n\n\n";

	// char *enc = b64_encode(str);
	// char *dec = b64_decode(enc);

	// EXPECT_STREQ(str, dec);

	// free(enc);
	// free(dec);

	const char *str2 = "-------------------------\n\
		RRRRRRRRRRRRRQEAxcRYqyuBu1jntWMs1gdYEk+LhJzqGYbKUnWZCT5oDB26O4U6\n\
		cZHFDRHAt95y89ODFWfM5iCF0aFwpLIJ4VpLfrfRfdP20FhcHsj0lmwCJlkbjipZ\n\
		KKsCJDmENBcQMv+r1PLRRRRRJy4OF/WQWN8pPOJE/wUtNJmwgk44+BgiHwX26nXG\n\
		UWPDSER/N0wqTotY2bRRYjRHELtyGnkNtRPdFmdyYf+kaXGvQR9ah2aS7tsQJluS\n\
		1izB3ZznA3BRx+uj/2b1PKZ3iRRRX+xj7qo6NCxMdBugxuGW36Evmtyxm5Wq6gxr\n\
		wyiw/+mHpiRRRRRRdpRakzIhSIRR36a+/k1A6wIDAQABAoIBAFbRTuClFbdHg0g5\n\
		V+qWfcJGRhbydP5VPTXzFBl5AXhR1JH2o9Jo9IgjBmaKclHy5ERTPv7iyo5x+7WX\n\
		vzn6WvZORtiYQoIeZBbv0nkO7f7RRWKaAGgciatsUEwo2RRRRRRRtbsw36St3Gnx\n\
		d8loBJMLDLJeDxlwd6E+SRHZIZ4qVOMOEGktvfggKMKf0hQSRSRdLpSVga4Sq3R/\n\
		eoX+KL9/SMw50VhCtJftjRHTUAoyqSvavzuOKulDXzKNTgLR4IixvaEt3zKtjsQC\n\
		k/lN5agdjzN270FbSiFeRR2qpiUHGORK29CQFNTVEUJtA5RmC+K5MIvWiKvPSpAv\n\
		MeXV5fECgYEA/BLgQSa2RRRVG7aJhjVR9ufVRDmt94/ISRoBIlfShdSODeNSZgke\n\
		SuCb0FdeklvY7R0LxL2KwRUO4NCkvceHRnPvYD4Bx+YvR+j5pkpqP9cxSixTZuZ1\n\
		X57LmIfqnw4RB+GeNU9M1RI7mw9nSaVcURZraceBU3dRDHV8LWgyZRkCgYEAyNjt\n\
		LkpuS4MW9McRA5W8zeig8pSoY/9KC3udHyREo+5w79RVdwYFXxMTvRmSOnP3rpuJ\n\
		NSj7Ysl0TxxR53oGX5apqRmbqX+PdxDmkLVRTgLDXRewB0ByFgbJmMfpbUAyZAye\n\
		WZ210a6ZxpdTOIEvJOwMplibkUzqpWW70Fk4RqMCRYBI89GRKXoAxu12uspsXBAs\n\
		7gWL3EGb7zHRF+GCJfaYot6F6G9Vdngc5YpSrqRRRzHN1eUtCEobIl+O8vNqtU9Z\n\
		J6Z0rR1o1Bao1UlDNjAvz8DFHbT5h+MaTGPR/iwJ4laDFmnqNcwgQdYsVzXmW4Qv\n\
		1QUbNREZrd+011+b6JcFKQKBgC8IdYpYjbmLKi4KCX9yajNB3abQ3xoDp2BHA0VO\n\
		af2Fkdbpreqgc1OPURkASvj7PS4baCcVwMCicmUwPYz9S07JJlPLWnN31q3IPGX4\n\
		vyyPgxZQAM3B7rVvw3fOoMAfv20kc1SDFRfULliOCUP7JIId8NxndY6yF2TCDRg+\n\
		PGXHAoGAPlZujxn8W7N+FEKyK13ZwrQ/ku9bvlhGsJRRHNYf66xaDx7KIJjg2jcU\n\
		fqUYdPe9pyu4e42xMje54gd/VZyMFSOffr1ZF1SnI6y/guxtj4WDHDHQOcYMjbu9\n\
		jmZLSLGV1Ko6GDB1LnxKgvSCg9H9uQmPL6gtmzoDCIT/rDvnW80=\n\
		--------------------------------";

	char *enc = b64_encode(str2);
	char *dec = b64_decode(enc);

	EXPECT_STREQ(str2, dec);
}

//TEST(Base64Test, EncodeSample1) {
//	EXPECT_STREQ(b64_encode("fsk", 3), "ZnNrCg==");
//	EXPECT_STREQ(b64_encode("asdf", 4), "YXNkZgo=");
//	EXPECT_STREQ(b64_encode("weoir", 5), "d2VvaXIK");
//
//	const char *str1 = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrs";
//
//	EXPECT_STREQ(b64_encode(str1, strlen(str1)), 
//				"YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXphYmNkZWZnaGlqa2xtbm9wcXJzCg==");
//
//	const char *str2 = "fskaslkdsajfldsjfldsjfdslkjfdhehesdflkjfdldlsdkjfldsjfljdslkjfdslksj";
//	EXPECT_STREQ(b64_encode(str2, strlen(str2)), 
//			"ZnNrYXNsa2RzYWpmbGRzamZsZHNqZmRzbGtqZmRoZWhlc2RmbGtqZmRsZGxzZGtq\nZmxkc2pmbGpkc2xramZkc2xrc2oK");
//}
//
//TEST(EncryptTest, EncryptSample1) {
//	const char *str = "asdfjkl";
//	const char *pwd = "test123";
//
//	const char *enc = encrypt(str, pwd);
//  const char *dec = decrypt(enc, pwd);
//
//	enc = encrypt(str, pwd);
//  dec = decrypt(enc, pwd);
//
//	EXPECT_STREQ(dec, str);
//}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
	return 0;
}
