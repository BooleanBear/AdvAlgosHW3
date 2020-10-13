#include<iostream>
#include<string>
#include<cmath>
#include <sstream>
#include<time.h>

#define ubigint unsigned long long int
#define bigint long long int

using namespace std;

// Binary exponentiation under mod p
ubigint modpow(ubigint x, ubigint y, ubigint p) {
	ubigint res = 1;

	x = x % p;

	if (x == 0) return 0;

	while (y > 0) {
		// If odd multiply by x
		if (y & 1) res = (res * x) % p;

		y = y / 2;
		x = (x*x) % p;
	}

	return res;
}

// Encode a string into an integer using base 27
ubigint stringToInt(string s) {
	ubigint sum = 0;
	int power = s.length() - 1;
	for (int i = 0; i < s.length(); i++) {
		char c = s.at(i);
		int charCode;
		if (c == ' ') charCode = 0;
		else charCode = toupper(s.at(i)) - 64;
		sum += charCode * pow(27, power);
		power--;
	}
	
	return sum;
}

// Decode an integer into a string using base 27
string intToString(ubigint e) {
	ubigint remainder;
	stringstream s;

	// Keep getting remainder and dividing by 27 to get 'BEARCATII' values for each character
	while (e != 0) {
		char c = (e % 27);
		if (c == 0) c = ' ';
		else c += 64;
		s << c;

		e = e / 27;
	}

	string out = s.str();
	reverse(out.begin(), out.end());
	
	return out;
}

// Miller-Rabin primality test
bool millerRabin(ubigint n, ubigint m, ubigint k) {
	ubigint a = rand() % (n - 3) + 2; // random between 2 and n-2

	// b = a^m mod n
	ubigint b = modpow(a, m, n);

	if (b == 1 || b == (n - 1)) return true;
	for (ubigint j = 1; j <= k; j++) {
		b = (b * b) % n;
		if (b == 1) return false;
		if (b == n - 1) return true;
	}
	
	return false;
}

// Test if a number is prime using rounds of Miller-Rabin test
bool isPrime(ubigint n, int rounds) {
	if (n == 2 || n == 3) return true;
	if (ubigint(n / 2.0) == n / 2.0 || n == 1) return false; // 1 and even numbers are not prime
	
	// find m and k such that n-1 = 2^k * m, with m odd
	ubigint k = 0;
	ubigint m = n - 1;
	while (ubigint(m / 2.0) == m / 2.0) {
		m = m / 2;
		k++;
	}

	// Perform Miller-Rabin test 'rounds' times.
	// If it ever returns false we know n is not prime
	for (int i = 0; i < rounds; i++) {
		if (!millerRabin(n, m, k)) return false;
	}

	// Else n is prime with high certainty
	return true;
}

// Euclid's Extended GCD algorithm
ubigint gcd(ubigint a, ubigint b, bigint &s, bigint &t) {
	if (b == 0) {
		s = 1;
		t = 0;
		return a;
	}
	ubigint r = a % b;
	bigint s_, t_;
	ubigint ret_val = gcd(b, r, s_, t_);
	t = s_ - t_ * ((a - r) / b);
	s = t_;
	return ret_val;
}

// Encrypt or decrypt, depending on the key passed
string rsa(string M, ubigint key, ubigint n) {
	ubigint code = stringToInt(M);

	// C = m^e mod n
	ubigint enc = modpow(code, key, n);

	return intToString(enc);
}

int main() {
	srand(time(NULL));

	// Generate primes p and q
	unsigned int p = 1;
	while(!isPrime(p, 10)) p = rand() % UINT_MAX + 1;
	unsigned int q = 1;
	while (!isPrime(q, 10) || p == q) q = rand() % UINT_MAX + 1;
	cout << "p: " << p << endl;
	cout << "q: " << q << endl;

	ubigint n = p * q;
	ubigint phi_n = (p - 1) * (q - 1);

	cout << "n: " << n << endl;

	bigint s, t; // s will be private key
	ubigint e = phi_n;
	ubigint g = gcd(e, phi_n, s, t);
	while (g != 1) {
		cout << "Enter a public key: ";
		cin >> e; cin.ignore();
		g = gcd(e, phi_n, s, t);
		if (g != 1) {
			cout << "Not coprime to n, try again." << endl;
		}
	}

	if (s < 0) s += phi_n;
	cout << "Private Key: " << s << endl;

	string message;
	cout << "Enter message to encrypt: ";
	getline(cin, message);

	// Encrypt and decrypt using RSA
	string encrypted = rsa(message, e, n);
	string decrypted = rsa(encrypted, s, n);

	cout << "M: " << message << endl;
	cout << "C: " << encrypted << endl;
	cout << "P: " << decrypted << endl;

	return 0;
}