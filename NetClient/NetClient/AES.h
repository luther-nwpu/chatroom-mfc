#pragma once
enum { encrypt, decrypt };
class AES
{
public:
	void f_func(bool in[32], const bool ki[48]);
	void s_func(bool out[32], const bool in[48]);
	void transform(bool *out, bool *in, const char *table, int len);
	void fxor(bool *ina, const bool *inb, int len);
	void rotatel(bool *in, int len, int loop);
	void bytetobit(bool *out, const char *in, int bits);
	void bittobyte(char *out, const bool *in, int bits);
	void des_run(char out[8], char in[8], bool type);
	void des_setkey(const char key[8]);
	AES();
	~AES();
};

