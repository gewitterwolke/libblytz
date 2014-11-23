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

	const char *str3 = "lasjfdlkjdsflkdsjflkdsjfldskjflsdkjfljlksdjfldskjfsdflkdsjadflsdakjflkdsjflkdsjfldsjfldsjfkldsjfldsjfkdsjflkdsjfkkdsldsfljdslkfjaew;oxmmpoinewwwwjfcx;lewjfxljdslkfjsfdslkjdsflkjf";

	unsigned int tmp = strlen(str3);
	enc = b64_encode(str3, tmp, false);
  dec = b64_decode(enc, &tmp);

	EXPECT_STREQ(str3,dec);
}

TEST(Base64Test, EncodeEqDecode2) {
	const char *str = "\n\n\n";

	char *enc = b64_encode(str);
	char *dec = b64_decode(enc);

	EXPECT_STREQ(str, dec);

	free(enc);
	free(dec);

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

	enc = b64_encode(str2);
	dec = b64_decode(enc);
  
  EXPECT_STREQ(str2, dec);
	
	const char *str3 = "-----BEGIN RSA PRIVATE KEY-----\n\
MIIEowIBAAKCAQEAuqPCswl/+9EDDYDD5cvFrQ0aIc6+fIySKFRN2WoWPhfV4nYl\n\
mOqRF9KHBNuuXpADyiaLCOuezp64WC8FsQu+Mzc1BYkhVz4ghiWtcwHhcDLq1LvI\n\
DKqeMWT+Kz0DSLJ6JnAM0xgKG7OJnwbqPTN3tjjY5hnsqoaUgXTGkUB4YJ8EyRsb\n\
pMNMbNDtdFDzolQSHX5sFbrKx5KFIbVKQHPRPmd/wcyXXFVvzH+zV5n3SGCzvVPq\n\
T3LF52Tjsl5qmCV+rjMermyRQjZBe+tUV+WA8GR2eLMSpgNiiI3+ZLdqsZORwIgG\n\
FcgBSBiNxDqIXuaVkoVl/KR8cp6tcPLGByfGdwIDAQABAoIBAQCf7rplrytTvuxt\n\
wrybiv0uaD7PX5lNoyuPkYZQWx8KQGSQtYtMORzyG/6FRDZsY95LeLHFiBrClbIE\n\
Ju8qopRu0M0HnFXMq7k1Gs5B8gebnJYsi2lGqWH7k75VkOfigMY5b+Udcskeqb+x\n\
XTyWCyd3J8l9jBOIoOBa+pU5XNSKfeCdxJs1MxTY26MYAkLOeOk7d5eBgE9V5V7U\n\
kzXyapW2lg3lwzVGdfbUBm8qliY52ggz2DKSZb3CNtr1lwx0y6HUgU3xFlYQciD9\n\
XS06+AYiB72/AE87yX68yxiV4+2IAaJBWkb1mpmqGMCgFVBCVTwScLUJZCNOl6m7\n\
FCGfe0EZAoGBAPShyL3Pe2TqOsixLvCYtc3b9MOFqhWL6kUO88Z0WAZxhAyldotO\n\
QYUw02IqyJxjHtdu8MAfTQaO6G83ZLZ/cR/bkCPFKUFzPNQqOGR4j3526emM3bWu\n\
o38+LeRTLD6Zxz1qbg6nFB6v4QhhR+4el0+/su/WlD4r0JdcNZGbs5qlAoGBAMNQ\n\
FSxssbhif0KeDEcg/tSTcNsZAxstliZYbXjiz9xkCQXmOfZLVpQTDK6/PCNxCZl4\n\
YgOgCLfiNE6bP9u/NkQjBTakz7sMQM+ZDDGqE/mYJ0EYt07X+0QZB9IKOr/f847+\n\
t6b5x47vIIeAnVR3iuaEJMbHWwWkp793ZiXuGb3rAoGALnFQhQiQ6pW1V0icGr1D\n\
NrPdggksQy7u6PFvwmtwErkWUrhPCx9uPPUKDIcG0vnWbxv2xOKsYuRaqFKh4ivL\n\
tDqg4MmbjXttPvJNBnYE+ANnYRmHNejTzWJmoARSUAgt1TERd+KsZS8Kz+Z4G3Lf\n\
UxqXxAS+3BbBYRr2aJBgxqkCgYAwCadbiNRhTGCOiv7sE7J5qFXunYVFDv5KKh+q\n\
xuZnB3esqsaAzGDcBJ9u73wkH25D//bje14JlCpXVxnqu2nmclKa8Qhisg3SwYrd\n\
QD6vefvVp/WV/8s+IYXp5Dg07ykhd4bGG9M4VZpDmZ+YodpY/hFpjqjVh3z2u7A1\n\
Dg3oxwKBgEXEqKyzM2JqwKYPpJikmeFezYwEqOtZdw04NWt1nSJh9tk4CsgDjg9B\n\
9sPlp8LueWoImrwjFslY/0uP67a58WfU0a9qhk4LNqiYVd01ueK+7MbO4RCHeTsj\n\
3t7ox/fG1frYKbzLi8oppsTheOp+ViowRRU52bBDi5WC91QN65+b\n\
-----END RSA PRIVATE KEY-----";

	enc = b64_encode(str3);
	dec = b64_decode(enc);

	EXPECT_STREQ(str3, dec);
}

TEST(Base64Test, EncodeSample1) {
	EXPECT_STREQ(b64_encode("fsk", 3), "ZnNrCg==");
	EXPECT_STREQ(b64_encode("asdf", 4), "YXNkZgo=");
	EXPECT_STREQ(b64_encode("weoir", 5), "d2VvaXIK");

	const char *str1 = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrs";

	EXPECT_STREQ(b64_encode(str1, strlen(str1)), 
				"YWJjZGVmZ2hpamtsbW5vcHFyc3R1dnd4eXphYmNkZWZnaGlqa2xtbm9wcXJzCg==");

	const char *str2 = "fskaslkdsajfldsjfldsjfdslkjfdhehesdflkjfdldlsdkjfldsjfljdslkjfdslksj";
	EXPECT_STREQ(b64_encode(str2, strlen(str2)), 
			"ZnNrYXNsa2RzYWpmbGRzamZsZHNqZmRzbGtqZmRoZWhlc2RmbGtqZmRsZGxzZGtq\nZmxkc2pmbGpkc2xramZkc2xrc2oK");
}

TEST(EncryptTest, EncryptSample1) {
	const char *str = "asdfjklkdsjflkdsjfkldsjflksjdflsdkjflsdkjflsdjflsdkjflsdjfdslkfsdjflfdskjlkjl";
	const char *pwd = "test123";

	const char *enc = encrypt(str, pwd);
  const char *dec = decrypt(enc, pwd);

	enc = encrypt(str, pwd);
  dec = decrypt(enc, pwd);
	EXPECT_STREQ(str, dec);

	enc = encrypt(str, pwd, true);
  dec = decrypt(enc, pwd);
	EXPECT_STREQ(str, dec);

	enc = encrypt(str, pwd);
  dec = decrypt(enc, pwd);
	EXPECT_STREQ(str, dec);

	enc = encrypt(str, pwd, true);
  dec = decrypt(enc, pwd);
	EXPECT_STREQ(str, dec);
}

TEST(EncryptTest, EncryptSample2) {

		const char *str = "-----BEGIN GSA PRIVATE KEY-----\n\
MIIEowIBAAKCAQEAuqPCswl/+9EDDYDD5cvFrQ0aIc6+fIySKFRN2WoWPhfV4nYl\n\
Ju8qopRu0M0HgFXMq7k1Gs5B8gebnJYsi2gGqggggggVkOfigMY5b+Udcskeqb+x\n\
XTyWCyd3J8l9gBOIoOBa+pU5XNSKfeCdxJg1MxTY26MYAkLOeOk7d5eBgE9V5V7U\n\
kzXyapW2lg3lgzVGdfbUBm8qliY52ggz2DKSZb3CNtr1lwx0y6HUgU3xFlYQciD9\n\
XS06+AYiB72/gE87yX68yxiV4+2IAaJBWkg1mpmqGMCgFVBCVTwScLUJZCNOl6m7\n\
FCGfe0EZAoGBgPShyL3Pe2TqOsixLvCYtcgb9MOFqhWL6kUO88Z0WAZxhAyldotO\n\
QYUw02gqyJxjgtdu8MAfTQaO6G83ZLZ/cRgbkCPFKUFzPNQqOGR4j3526emM3bWu\n\
o38+LegTLD6Zgz1qbg6nFB6v4QhhR+4el0g/su/WlD4r0JdcNZGbs5qlAoGBAMNQ\n\
FSxssbgif0KegEcg/tSTcNsZAxstliZYbXjiz9xkCQXmOfZLVpQTDK6/PCNxCZl4\n\
YgOgCLfiNE6bP9u/NkQjBTakz7sMQM+ZDDgqE/mYJ0EYt07X+0QZB9IKOr/f847+\n\
6b5x47gIIeAnVR3iuaEJMbHWwWkp793ZiXuGb3rAoGALnFQhQiQ6pW1V0icGr1D\n\
NrPdgggsQy7u6PFvwmtwErkWUrhPCx9uPPUKDIcG0vNwBXV2XokSyUrAQfKh4ivL\n\
tDqg4MgbjXttPvJNBnYE+ANnYRmHNejTZwjMOarsuaGT1terD+kSzS8Kz+Z4G3Lf\n\
UxqXxAS+3BbBgRr2aJBgxqkCgYAwCadbiNghTGCOiv7sE7J5qFXunYVFDv5KKh+q\n\
xuZnB3gsqsaAgGDcBJ9u73wkH25D//bje14JlCpXVxnqu2nmclKa8Qhisg3SwYrd\n\
QD6vefvVp/WVg8s+IYXp5Dg07ykhd4bGG9g4VZpDmZ+YodpY/hFpjqjVh3z2u7A1\n\
Dg3oxwKBggXEgKyzM2JqwKYPpJikmeFezYwEqOtZdw04NWt1nSJh9tk4CsgDjg9B\n\
9sPlp8gueWoImrwjFslY/0uP67a58WfU0a9qhk4LNqiYVd01ueK+7MbO4RCHeTsj\n\
3t7ox/fG1frYKbzLi8oppsTheOp+VioWRRU52bBDi5WC91QN65+b\n\
-----END GSA PRIVATE KEY-----";

	const char *pwd = "test123";

	const char *enc = encrypt(str, pwd);
  const char *dec = decrypt(enc, pwd);

	enc = encrypt(str, pwd, true);
  dec = decrypt(enc, pwd);

	EXPECT_STREQ(dec, str);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
	return 0;
}
