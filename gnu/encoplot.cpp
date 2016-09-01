#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <unistd.h>
//#include "Objbase.h"
//#include "rpc.h"

typedef unsigned long int t4gram;
typedef unsigned long long int t8gram;
typedef __uint128_t t16gram;

//CrG rsort
#define fr(x,y) for (int x=0; x<y ; x++)

int* index_rsort_ngrams(unsigned char *x, int l, int DEPTH) {
	int NN = l - DEPTH + 1;
	if (NN>0) {
		unsigned char *pin = x + NN;
		unsigned char *pout = x;
		int *ix = (int *)malloc(NN* sizeof(int));
		int *ox = (int *)malloc(NN* sizeof(int));
		const int RANGE = 256;
		int counters[RANGE];
		int startpos[RANGE];
		fr(i, NN) ix[i] = i;
		// radix sort , the input is x ,
		// the output rank is ix
		fr(k, RANGE) counters[k] = 0;
		fr(i, NN) counters[*(x + i)]++;
		fr(j, DEPTH) {
			int ofs = j;// low endian
			int sp = 0;
			fr(k, RANGE) {
				startpos[k] = sp;
				sp += counters[k];
			}
			fr(i, NN) {
				unsigned char c = x[ofs + ix[i]];
				ox[startpos[c]++] = ix[i];
			}
			memcpy(ix, ox, NN* sizeof(ix[0]));
			// update counters
			if (j<DEPTH - 1) {
				counters[*pout++]--;
				counters[*pin++]++;
			}
		}
		free(ox);
		return ix;
	}
}

#define MAXBUFSIZ 8000123
unsigned char file1[MAXBUFSIZ];
unsigned char file2[MAXBUFSIZ];
int l1, l2;

inline t4gram readat4(const unsigned char *buf, int poz){
	return *(t4gram *)(buf + poz);
}

inline t8gram readat8(const unsigned char *buf, int poz){
	return *(t8gram *)(buf + poz);
}

inline t16gram readat16(const unsigned char *buf, int poz){
	return *(t16gram *)(buf + poz);
}

int enco16(unsigned *data, int maxlen)
{
	std::vector<int> v;
	int depth = sizeof(t16gram);
	int * ix1 = index_rsort_ngrams(file1, l1, depth);
	int * ix2 = index_rsort_ngrams(file2, l2, depth);
	int i1 = 0; int i2 = 0;//merge

	t16gram s1 = readat16(file1, ix1[i1]);
	t16gram s2 = readat16(file2, ix2[i2]);

	l1 -= (depth - 1);
	l2 -= (depth - 1);
	while (i1 < l1 && i2 < l2) {
		if (s1 == s2) {
#ifdef EXE
			printf("%d\t%d\n", ix1[i1], ix2[i2]);
#else
			v.push_back(ix1[i1]); v.push_back(ix2[i2]);
#endif
			i1++; if (i1 < l1) s1 = readat16(file1, ix1[i1]);
			i2++; if (i2 < l2) s2 = readat16(file2, ix2[i2]);
		}
		else if (s1 < s2) {
			i1++;
			if (i1 < l1) s1 = readat16(file1, ix1[i1]);
		}
		else if (s2 < s1) {
			i2++;
			if (i2 < l2) s2 = readat16(file2, ix2[i2]);
		}
	}
	free(ix2);
	free(ix1);

	//
	if (data == NULL) return 0;
	int len = v.size();
	size_t size = len*sizeof(int);
	memcpy(data, v.data(), size);
	return len;
}

int enco8(unsigned *data, int maxlen)
{
	std::vector<int> v;
	int depth = sizeof(t8gram);
	int * ix1 = index_rsort_ngrams(file1, l1, depth);
	int * ix2 = index_rsort_ngrams(file2, l2, depth);
	int i1 = 0; int i2 = 0;//merge

	t8gram s1 = readat8(file1, ix1[i1]);
	t8gram s2 = readat8(file2, ix2[i2]);

	l1 -= (depth - 1);
	l2 -= (depth - 1);
	while (i1 < l1 && i2 < l2) {
		if (s1 == s2) {
#ifdef EXE
			printf("%d\t%d\n", ix1[i1], ix2[i2]);
#else
			v.push_back(ix1[i1]); v.push_back(ix2[i2]);
#endif
			i1++; if (i1 < l1) s1 = readat8(file1, ix1[i1]);
			i2++; if (i2 < l2) s2 = readat8(file2, ix2[i2]);
		}
		else if (s1 < s2) {
			i1++;
			if (i1 < l1) s1 = readat8(file1, ix1[i1]);
		}
		else if (s2 < s1) {
			i2++;
			if (i2 < l2) s2 = readat8(file2, ix2[i2]);
		}
	}
	free(ix2);
	free(ix1);

	//
	if (data == NULL) return 0;
	int len = v.size();
	size_t size = len*sizeof(int);
	memcpy(data, v.data(), size);
	return len;
}

int enco4(unsigned *data, int maxlen)
{
	std::vector<int> v;
	int depth = sizeof(t4gram);
	int * ix1 = index_rsort_ngrams(file1, l1, depth);
	int * ix2 = index_rsort_ngrams(file2, l2, depth);
	int i1 = 0; int i2 = 0;//merge

	t4gram s1 = readat4(file1, ix1[i1]);
	t4gram s2 = readat4(file2, ix2[i2]);

	l1 -= (depth - 1);
	l2 -= (depth - 1);
	while (i1 < l1 && i2 < l2) {
		if (s1 == s2) {
#ifdef EXE
			printf("%d\t%d\n", ix1[i1], ix2[i2]);
#else
			v.push_back(ix1[i1]); v.push_back(ix2[i2]);
#endif
			i1++; if (i1 < l1) s1 = readat4(file1, ix1[i1]);
			i2++; if (i2 < l2) s2 = readat4(file2, ix2[i2]);
		}
		else if (s1 < s2) {
			i1++;
			if (i1 < l1) s1 = readat4(file1, ix1[i1]);
		}
		else if (s2 < s1) {
			i2++;
			if (i2 < l2) s2 = readat4(file2, ix2[i2]);
		}
	}
	free(ix2);
	free(ix1);

	//
	if (data == NULL) return 0;
	int len = v.size();
	size_t size = len*sizeof(int);
	memcpy(data, v.data(), size);
	return len;
}

int enco(int k, unsigned *data, int maxlen)
{
	if (k == 4) return enco4(data, maxlen);
	if (k == 8) return enco8(data, maxlen);
	if (k == 16) return enco16(data, maxlen);
}


extern "C" {

#define IG_API __declspec(dllexport)

	IG_API int GetSize()
	{
		int depth = sizeof(t16gram);
		return depth;
	}

	IG_API int encoplot_ff(int k, const char * fn1, const char * fn2, unsigned *data, int maxlen)
	{
		errno_t err;
		FILE *f1, *f2;
		err = fopen_s(&f1, fn1, "rt");
		l1 = fread(file1, 1, MAXBUFSIZ, f1);
		err = fclose(f1);
		err = fopen_s(&f2, fn2, "rt");
		l2 = fread(file2, 1, MAXBUFSIZ, f2);
		fclose(f2);
		//
		return enco(k, data, maxlen);
	}

	IG_API int encoplot_ss(int k, const char * txt1, const char * txt2, unsigned *data, int maxlen)
	{
		l1 = strlen(txt1);
		l2 = strlen(txt2);
		strncpy((char *)file1, txt1, l1);
		strncpy((char *)file2, txt2, l2);
		//
		return enco(k, data, maxlen);
	}
}

#ifdef EXE
int main( int argc , char ** argv ) {
	//encoplot_ff(4, argv[1], argv[2], NULL, 0);
	//encoplot_ff(8, argv[1], argv[2], NULL, 0);
	encoplot_ff(16, argv[1], argv[2], NULL, 0);
	return 0;
}
#endif