#pragma once
#pragma once
#include <cstring>
class RSA_PARAM
{
public:
	unsigned __int64    p, q;
	unsigned __int64    f;
	unsigned __int64    n, e;
	unsigned __int64    d;
	unsigned __int64    s;

};
class RSA
{
	private:

	public:
		RSA();
		~RSA();
		unsigned __int64 MulMod(unsigned __int64 a, unsigned __int64 b, unsigned __int64 n);
		unsigned __int64 PowMod(unsigned __int64 &base, unsigned __int64 &pow, unsigned __int64 &n);
		long RabinMillerKnl(unsigned __int64 &n);
		long RabinMiller(unsigned __int64 &n, long loop);
		unsigned __int64 RandomPrime(char bits);
		unsigned __int64 EuclidGcd(unsigned __int64 &p, unsigned __int64 &q);
		unsigned __int64 SteinGcd(unsigned __int64 &p, unsigned __int64 &q);
		unsigned __int64 Euclid(unsigned __int64 &a, unsigned __int64 &b);
		RSA_PARAM RsaGetParam(void);
};

