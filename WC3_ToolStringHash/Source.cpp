#define _CRT_NONSTDC_NO_DEPRECATE

#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

typedef uint32_t ub4;
typedef uint8_t ub1;

#define dye_black 0
#define dye_blue 1 
#define dye_green 2 
#define dye_aqua 3 
#define dye_red 4 
#define dye_purple 5 
#define dye_yellow 6 
#define dye_white 7 
#define dye_grey 8 
#define dye_light_blue 9 
#define dye_light_green 10 
#define dye_light_aqua 11 
#define dye_light_red 12 
#define dye_light_purple 13 
#define dye_light_yellow 14 
#define dye_bright_white 15

void PrintColor(int line, int cl, string str)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cl);
	if (line)
		cout << str << endl;
	else
		cout << str;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), dye_white);
}

void LogMsg(const char* filename, const char* Msg...)
{
	va_list arguments;
	va_start(arguments, Msg);

	int len = _vscprintf(Msg, arguments) + 1;
	char* text = new char[len];
	vsprintf_s(text, len, Msg, arguments);
	va_end(arguments);

	FILE* plik;
	fopen_s(&plik, filename, "a");
	if (plik)
	{
		fprintf(plik, "%s\n", text);
		fclose(plik);
	}
	delete[] text;
}

#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

ub4 hashhash(ub1* k, ub4 length, ub4 initval)
{
	ub4 a, b, c, len;

	len = length;
	a = b = 0x9e3779b9;
	c = initval;

	while (len >= 12)
	{
		a += (k[0] + ((ub4)k[1] << 8) + ((ub4)k[2] << 16) + ((ub4)k[3] << 24));
		b += (k[4] + ((ub4)k[5] << 8) + ((ub4)k[6] << 16) + ((ub4)k[7] << 24));
		c += (k[8] + ((ub4)k[9] << 8) + ((ub4)k[10] << 16) + ((ub4)k[11] << 24));
		mix(a, b, c);
		k += 12; len -= 12;
	}

	c += length;
	switch (len) 
	{
		case 11: c += ((ub4)k[10] << 24);
		case 10: c += ((ub4)k[9] << 16);
		case 9: c += ((ub4)k[8] << 8);
		case 8: b += ((ub4)k[7] << 24);
		case 7: b += ((ub4)k[6] << 16);
		case 6: b += ((ub4)k[5] << 8);
		case 5: b += k[4];
		case 4: a += ((ub4)k[3] << 24);
		case 3: a += ((ub4)k[2] << 16);
		case 2: a += ((ub4)k[1] << 8);
		case 1: a += k[0];
	}
	mix(a, b, c);
	return c;
}

ub4 SStrHash2(const char* key) 
{
	ub1 buff[0x400];
	ub4 len = 0;
	while (*key) 
	{
		if (*key < 'a' || *key>'z') 
		{
			if (*key == '/') 
			{
				buff[len] = '\\';
			}
			else {
				buff[len] = *key;
			}
		}
		else 
		{
			buff[len] = *key - 0x20;
		}
		key++;
		len++;
	}
	return hashhash(buff, len, 0);
}

string SStrHash2_str(const char* key)
{
	return to_string((int)SStrHash2(key));
}

class StringHashBreakerThread 
{
private:
	string ALPHABET = "abcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_-";
	long attempts = 0;
	bool broken = false;
	string hash;
	string kytudau;
	string plaintext = "(unknown)";

	void tryCombinations(char set[], int k) 
	{
		if (!broken) 
		{
			int n = strlen(set);
			tryCombinationsRecursive(set, "", n, k);
		}
	}

	void tryCombinationsRecursive(char set[], string prefix, int n, int k) 
	{
		if (k == 0) 
		{
			string deobietten_var = kytudau + prefix;
			string hash = SStrHash2_str(deobietten_var.c_str());
			attempts++;
			if (hash == this->hash)
			{
				this->plaintext = kytudau + prefix;
				this->broken = true;
			}
			return;
		}

		for (int i = 0; i < n; ++i) 
		{
			string newPrefix = prefix + set[i];
			if (!broken) 
			{
				tryCombinationsRecursive(set, newPrefix, n, k - 1);
			}
		}
	}

	void breakHash() 
	{
		while (!broken) 
		{
			for (int i = 1; i < 12; i++) 
			{
				tryCombinations((char*)ALPHABET.data(), i);
			}
		}
	}

public:
	StringHashBreakerThread(string hash, string kytudau) 
	{
		this->hash = hash;
		this->kytudau = kytudau;
	}

	void run() 
	{
		breakHash();
	}

	long getAttempts() 
	{
		return attempts;
	}

	void setAttempts(long attempts) 
	{
		this->attempts = attempts;
	}

	bool isBroken() 
	{
		return broken;
	}

	void setBroken(bool broken) 
	{
		this->broken = broken;
	}

	string getHash() 
	{
		return hash;
	}

	void setHash(string hash) 
	{
		this->hash = hash;
	}

	string getPlaintext() 
	{
		return plaintext;
	}

	void setPlaintext(string plaintext) 
	{
		this->plaintext = plaintext;
	}
};

int main()
{
	PrintColor(1, dye_light_green, "[Tool lam boi Thai Son v0.1]");
	PrintColor(1, dye_light_yellow, "1: Create StringHash");
	PrintColor(1, dye_light_yellow, "2: Decode StringHash");
	PrintColor(1, dye_light_yellow, "3: Decode StringHash Bonus");
	PrintColor(1, dye_light_yellow, "4: Exit");

	PrintColor(0, dye_light_red, "Lua Chon: ");
	int LuaChon;
	cin >> LuaChon;

	if (LuaChon == 1)
	{
		system("cls");

		PrintColor(0, dye_light_aqua, "Nhap Text: ");
		string strText;
		getline(cin >> ws, strText);

		PrintColor(0, dye_light_aqua, "StringHash: ");
		string ketqua = SStrHash2_str(strText.c_str());
		cout << ketqua.c_str() << endl;

		LogMsg("ToolStringHash.log", "Nhap Text: %s -> StringHash: %s", strText.c_str(), ketqua.c_str());
	}

	if (LuaChon == 2)
	{
		system("cls");

		PrintColor(0, dye_light_aqua, "Nhap StringHash: ");
		string number;
		getline(cin >> ws, number);

		StringHashBreakerThread* thread = new StringHashBreakerThread(number, "");
		thread->run();

		PrintColor(0, dye_light_aqua, "Text: ");
		string ketqua = thread->getPlaintext();
		cout << ketqua.c_str() << endl;

		LogMsg("ToolStringHash.log", "Nhap StringHash: %s -> Text: %s", number.c_str(), ketqua.c_str());
	}

	if (LuaChon == 3)
	{
		system("cls");

		PrintColor(0, dye_light_aqua, "Nhap StringHash: ");
		string number;
		getline(cin >> ws, number);

		PrintColor(0, dye_light_aqua, "Nhap Ky tu dau StringHash: ");
		string kytudau = "";
		getline(cin >> ws, kytudau);

		StringHashBreakerThread* thread = new StringHashBreakerThread(number, kytudau);
		thread->run();

		PrintColor(0, dye_light_aqua, "Text: ");
		string ketqua = thread->getPlaintext();
		cout << ketqua.c_str() << endl;

		LogMsg("ToolStringHash.log", "Nhap StringHash: %s -> Text: %s", number.c_str(), ketqua.c_str());
	}

	if (LuaChon == 4)
	{
		exit(-1);
		return 0;
	}

	system("pause");
}