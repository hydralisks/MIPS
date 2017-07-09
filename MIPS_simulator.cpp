#include<map>
#include<string>
#include<cmath>
#include<cstdio>
#include<cstring>
#include<iostream>
#include<algorithm>
#include <vector>
#include <map> 
#include <fstream>
using namespace std;

map <string, int> registerMap;
int stackTop, heapTail;
int registerArray[36];
bool registersign[36];

unsigned char ram[4 * 1024 * 1024 + 10];
unsigned char typ[4 * 1024 * 1024 + 10]; // 0:NULL, 1:byte, 2:half, 3:word, 4:space


void pre () {
	
	registerMap["$zero"] = 0;
	registerMap["$at"] = 1;
	registerMap["$v0"] = 2;
	registerMap["$v1"] = 3;
	registerMap["$a0"] = 4;
	registerMap["$a1"] = 5;
	registerMap["$a2"] = 6;
	registerMap["$a3"] = 7;
	registerMap["$t0"] = 8;
	registerMap["$t1"] = 9;
	registerMap["$t2"] = 10;
	registerMap["$t3"] = 11;
	registerMap["$t4"] = 12;
	registerMap["$t5"] = 13;
	registerMap["$t6"] = 14;
	registerMap["$t7"] = 15;
	registerMap["$s0"] = 16;
	registerMap["$s1"] = 17;
	registerMap["$s2"] = 18;
	registerMap["$s3"] = 19;
	registerMap["$s4"] = 20;
	registerMap["$s5"] = 21;
	registerMap["$s6"] = 22;
	registerMap["$s7"] = 23;
	registerMap["$t8"] = 24;
	registerMap["$t9"] = 25;
	registerMap["$k0"] = 26;
	registerMap["$k1"] = 27;
	registerMap["$gp"] = 28;
	registerMap["$sp"] = 29;
	registerMap["$fp"] = 30;
	registerMap["$ra"] = 31;
	registerMap["$lo"] = 32;
	registerMap["$hi"] = 33;
	registerMap["$PC"] = 34;
	registerMap["$0"] = 0;
	registerMap["$1"] = 1;
	registerMap["$2"] = 2;
	registerMap["$3"] = 3;
	registerMap["$4"] = 4;
	registerMap["$5"] = 5;
	registerMap["$6"] = 6;
	registerMap["$7"] = 7;
	registerMap["$8"] = 8;
	registerMap["$9"] = 9;
	registerMap["$10"] = 10;
	registerMap["$11"] = 11;
	registerMap["$12"] = 12;
	registerMap["$13"] = 13;
	registerMap["$14"] = 14;
	registerMap["$15"] = 15;
	registerMap["$16"] = 16;
	registerMap["$17"] = 17;
	registerMap["$18"] = 18;
	registerMap["$19"] = 19;
	registerMap["$20"] = 20;
	registerMap["$21"] = 21;
	registerMap["$22"] = 22;
	registerMap["$23"] = 23;
	registerMap["$24"] = 24;
	registerMap["$25"] = 25;
	registerMap["$26"] = 26;
	registerMap["$27"] = 27;
	registerMap["$28"] = 28;
	registerMap["$29"] = 29;
	registerMap["$30"] = 30;
	registerMap["$31"] = 31;
	registerMap["$32"] = 32;
	registerMap["$33"] = 33;
	registerMap["$34"] = 34;
	
	for (int i = 0; i < 34; i++) registerArray[i] = 0;
	registerArray[29] = 4 * 1024 * 1024;
	
	
}

vector <string> prog;
int progLines;

struct label {
	int lineNumber;
	int ramNumber;
	
	bool operator < (const label & rlabel) {
		if (lineNumber < rlabel.lineNumber) return true;
		if (lineNumber > rlabel.lineNumber) return false;
		return ramNumber < rlabel.ramNumber;
	}
	
};

map <string, label> labelMap;

bool unshown (char u) {
	return u == ' ' || u == '\t' || u == '\r' || u == '\n' || u == '\0';
}

bool isnumber (char u) {
	return (u >= '0' && u <= '9') || u == '-';
}

bool isletter (char u) {
	return u >= 'a' && u <= 'z';
}

bool IsLetter (char u) {
	return u >= 'A' && u <= 'Z';
}

bool isregi (char u) {
	return u == '$' || isnumber (u) || isletter (u);
}



void simulatePointData () {
	bool isData = false;
	for (int z = 0; z < progLines; z++) {
		int i = 0, j = 0;
		while (i < prog[z].length() && unshown (prog[z][i])) i++;
		if (i == prog[z].length()) continue;
		while (i + j < prog[z].length() && !unshown (prog[z][i + j])) j++;
		string s1 = prog[z].substr (i, j);
		i += j;
		if (prog[z][i - 1] == ':') {
			string s2 = s1.substr (0, j - 1);
			if (s2 == "main") continue;
			label a;
			a.lineNumber = z + 1;
			a.ramNumber = heapTail;
			labelMap[s2] = a;
		}
		if (!isData) {
			if (s1 == ".data") isData = true;
		}
		else {
			if (s1 == ".text") isData = false;
			else {
				if (s1 == ".align") {
					while (i < prog[z].length() && !isnumber (prog[z][i])) i++;
					int n1 = 0;
					while (i < prog[z].length() && isnumber (prog[z][i])) {
						n1 *= 10;
						n1 += (int)prog[z][i] - (int)'0';
						i++;
					}
					if (heapTail & (1 << n1) - 1) {
						heapTail = (heapTail >> n1) + 1 << n1;
					}
				}
				if (s1 == ".ascii") {
					while (i < prog[z].length() && prog[z][i] != '\"') i++;
					i++;
					j = 1;
					while (i + j < prog[z].length() && prog[z][i + j] != '\"') j++;
					string s2 = prog[z].substr (i + 1, j - 1);
					for (int x = 0; x < s2.length(); x++) {
						if (s2[x] == '\\') {
							if (s2[x + 1] == '\\') ram[heapTail] = '\\';
							if (s2[x + 1] == 'n') ram[heapTail] = '\n';
							if (s2[x + 1] == 't') ram[heapTail] = '\t';
							if (s2[x + 1] == 'r') ram[heapTail] = '\r';
							if (s2[x + 1] == '\'') ram[heapTail] = '\'';
							if (s2[x + 1] == '\"') ram[heapTail] = '\"';
							typ[heapTail] = 1;
							heapTail++;
							x++;
							continue;
						}
						ram[heapTail] = s2[x];
						typ[heapTail] = 1;
						heapTail++;
					}
				}
				if (s1 == ".asciiz") {
					while (i < prog[z].length() && prog[z][i] != '\"') i++;
					j = 1;
					while (i + j < prog[z].length() && prog[z][i + j] != '\"') j++;
					string s2 = prog[z].substr (i + 1, j - 1);
					for (int x = 0; x < s2.length(); x++) {
						if (s2[x] == '\\') {
							if (s2[x + 1] == '\\') ram[heapTail] = '\\';
							if (s2[x + 1] == 'n') ram[heapTail] = '\n';
							if (s2[x + 1] == 't') ram[heapTail] = '\t';
							if (s2[x + 1] == 'r') ram[heapTail] = '\r';
							if (s2[x + 1] == '\'') ram[heapTail] = '\'';
							if (s2[x + 1] == '\"') ram[heapTail] = '\"';
							typ[heapTail] = 1;
							heapTail++;
							x++;
							continue;
						}
						ram[heapTail] = s2[x];
						typ[heapTail] = 1;
						heapTail++;
					}
					ram[heapTail] = '\0';
					typ[heapTail] = 1;
					heapTail++;
				}
				if (s1 == ".byte") {
					while (i < prog[z].length()) {
						while (i < prog[z].length() && prog[z][i] != '\'') i++;
						i++;
						ram[heapTail] = prog[z][i];
						typ[heapTail] = 1;
						heapTail++;
						i++;
						i++;
					}
				}
				if (s1 == ".half") {
					while (i < prog[z].length()) {
						while (i < prog[z].length() && !isnumber (prog[z][i])) i++;
						int n1 = 0;
						while (i < prog[z].length() && isnumber (prog[z][i])) {
							n1 *= 10;
							n1 += (int)prog[z][i] - (int)'0';
							i++;
						}
						for (int x = 1; x >= 0; x--) {
							ram[heapTail + x] = n1 % 256;
							typ[heapTail + x] = 2;
							n1 /= 256;
						}
						heapTail += 2;
					}
				}
				if (s1 == ".word") {
					while (i < prog[z].length()) {
						while (i < prog[z].length() && !isnumber (prog[z][i])) i++;
						int n1 = 0;
						while (i < prog[z].length() && isnumber (prog[z][i])) {
							n1 *= 10;
							n1 += (int)prog[z][i] - (int)'0';
							i++;
						}
						for (int x = 3; x >= 0; x--) {
							ram[heapTail + x] = n1 % 256;
							typ[heapTail + x] = 3;
							n1 /= 256;
						}
						heapTail += 4;
					}
				}
				if (s1 == ".space") {
					while (i < prog[z].length() && !isnumber (prog[z][i])) i++;
					int n1 = 0;
					while (i < prog[z].length() && isnumber (prog[z][i])) {
						n1 *= 10;
						n1 += (int)prog[z][i] - (int)'0';
						i++;
					}
					heapTail += n1;
				}
			}
		}
	}
}



int simulatePointText () {
	bool findMain = false;
	for (int z = 0; z < progLines; z++) {
		int i = 0, j = 0;
		while (i < prog[z].length() && unshown (prog[z][i])) i++;
		while (i + j < prog[z].length() && !unshown (prog[z][i + j])) j++;
		string s1 = prog[z].substr (i, j);
		i += j;
		if (!findMain && s1 != "main:") continue;
		if (s1 == "main:") {
			findMain = true;
			continue;
		}
		
		if (s1 == "add" || s1 == "addu" || s1 == "addiu") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			int n3 = registerArray[registerMap[s3]];
			int n4;
			if (s4[0] == '$') n4 = registerArray[registerMap[s4]];
			else {
				n4 = 0;
				int x = 0;
				if (s4[0] == '-') x++;
				for (; x < s4.length(); x++) {
					n4 *= 10;
					n4 += (int)s4[x] - (int)'0';
				}
				if (s4[0] == '-') n4 = -n4;
			}
			registerArray[registerMap[s2]] = n3 + n4;
		}
		
		if (s1 == "sub" || s1 == "subu") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			int n3 = registerArray[registerMap[s3]];
			int n4;
			if (s4[0] == '$') n4 = registerArray[registerMap[s4]];
			else {
				n4 = 0;
				int x = 0;
				if (s4[0] == '-') x++;
				for (; x < s4.length(); x++) {
					n4 *= 10;
					n4 += (int)s4[x] - (int)'0';
				}
				if (s4[0] == '-') n4 = -n4;
			}
			registerArray[registerMap[s2]] = n3 - n4;
		}
		
		if (s1 == "mul" || s1 == "mulu") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			if (i < prog[z].length()) {
				while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
				string s4 = prog[z].substr (i, j);
				i += j;
				int n3 = registerArray[registerMap[s3]];
				int n4;
				if (s4[0] == '$') n4 = registerArray[registerMap[s4]];
				else {
					n4 = 0;
					int x = 0;
					if (s4[0] == '-') x++;
					for (; x < s4.length(); x++) {
						n4 *= 10;
						n4 += (int)s4[x] - (int)'0';
					}
					if (s4[0] == '-') n4 = -n4;
				}
				registerArray[registerMap[s2]] = n3 * n4;
			}
			else {
				int n2 = registerArray[registerMap[s2]];
				int n3;
				if (s3[0] == '$') n3 = registerArray[registerMap[s3]];
				else {
					n3 = 0;
					int x = 0;
					if (s3[0] == '-') x++;
					for (; x < s3.length(); x++) {
						n3 *= 10;
						n3 += (int)s3[x] - (int)'0';
					}
					if (s3[0] == '-') n3 = -n3;
				}
				long long nn = (long long)n2 * n3;
				registerArray[registerMap["$hi"]] = nn >> 32 & (1LL << 32) - 1LL;
				registerArray[registerMap["$lo"]] = nn & (1LL << 32) - 1LL;
			}
		}
		
		if (s1 == "div" || s1 == "divu") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			if (i < prog[z].length()) {
				while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
				string s4 = prog[z].substr (i, j);
				i += j;
				int n3 = registerArray[registerMap[s3]];
				int n4;
				if (s4[0] == '$') n4 = registerArray[registerMap[s4]];
				else {
					n4 = 0;
					int x = 0;
					if (s4[0] == '-') x++;
					for (; x < s4.length(); x++) {
						n4 *= 10;
						n4 += (int)s4[x] - (int)'0';
					}
					if (s4[0] == '-') n4 = -n4;
				}
				registerArray[registerMap[s2]] = n3 / n4;
			}
			else {
				int n2 = registerArray[registerMap[s2]];
				int n3;
				if (s3[0] == '$') n3 = registerArray[registerMap[s3]];
				else {
					n3 = 0;
					int x = 0;
					if (s3[0] == '-') x++;
					for (; x < s3.length(); x++) {
						n3 *= 10;
						n3 += (int)s3[x] - (int)'0';
					}
					if (s3[0] == '-') n3 = -n3;
				}
				registerArray[registerMap["$hi"]] = n2 % n3;
				registerArray[registerMap["$lo"]] = n2 / n3;
			}
		}
		
		if (s1 == "xor" || s1 == "xoru") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			int n3 = registerArray[registerMap[s3]];
			int n4;
			if (s4[0] == '$') n4 = registerArray[registerMap[s4]];
			else {
				n4 = 0;
				int x = 0;
				if (s4[0] == '-') x++;
				for (; x < s4.length(); x++) {
					n4 *= 10;
					n4 += (int)s4[x] - (int)'0';
				}
				if (s4[0] == '-') n4 = -n4;
			}
			registerArray[registerMap[s2]] = n3 ^ n4;
		}
		
		if (s1 == "neg" || s1 == "negu") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			int n3 = registerArray[registerMap[s3]];
			registerArray[registerMap[s2]] = -n3;
		}
		
		if (s1 == "rem" || s1 == "remu") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			int n3 = registerArray[registerMap[s3]];
			int n4;
			if (s4[0] == '$') n4 = registerArray[registerMap[s4]];
			else {
				n4 = 0;
				int x = 0;
				if (s4[0] == '-') x++;
				for (; x < s4.length(); x++) {
					n4 *= 10;
					n4 += (int)s4[x] - (int)'0';
				}
				if (s4[0] == '-') n4 = -n4;
			}
			registerArray[registerMap[s2]] = n3 % n4;
		}
		
		if (s1 == "li") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			int n3 = 0;
			int x = 0;
			if (s3[0] == '-') x++;
			for (; x < s3.length(); x++) {
				n3 *= 10;
				n3 += (int)s3[x] - (int)'0';
			}
			if (s3[0] == '-') n3 = -n3;
			registerArray[registerMap[s2]] = n3;
		}
		
		
		
		
		if (s1 == "seq") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			int n3 = registerArray[registerMap[s3]];
			int n4;
			if (s4[0] == '$') n4 = registerArray[registerMap[s4]];
			else {
				n4 = 0;
				int x = 0;
				if (s4[0] == '-') x++;
				for (; x < s4.length(); x++) {
					n4 *= 10;
					n4 += (int)s4[x] - (int)'0';
				}
				if (s4[0] == '-') n4 = -n4;
			}
			registerArray[registerMap[s2]] = n3 == n4;
		}
		
		if (s1 == "sge") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			int n3 = registerArray[registerMap[s3]];
			int n4;
			if (s4[0] == '$') n4 = registerArray[registerMap[s4]];
			else {
				n4 = 0;
				int x = 0;
				if (s4[0] == '-') x++;
				for (; x < s4.length(); x++) {
					n4 *= 10;
					n4 += (int)s4[x] - (int)'0';
				}
				if (s4[0] == '-') n4 = -n4;
			}
			registerArray[registerMap[s2]] = n3 >= n4;
		}
		
		if (s1 == "sgt") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			int n3 = registerArray[registerMap[s3]];
			int n4;
			if (s4[0] == '$') n4 = registerArray[registerMap[s4]];
			else {
				n4 = 0;
				int x = 0;
				if (s4[0] == '-') x++;
				for (; x < s4.length(); x++) {
					n4 *= 10;
					n4 += (int)s4[x] - (int)'0';
				}
				if (s4[0] == '-') n4 = -n4;
			}
			registerArray[registerMap[s2]] = n3 > n4;
		}
		
		if (s1 == "sle") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			int n3 = registerArray[registerMap[s3]];
			int n4;
			if (s4[0] == '$') n4 = registerArray[registerMap[s4]];
			else {
				n4 = 0;
				int x = 0;
				if (s4[0] == '-') x++;
				for (; x < s4.length(); x++) {
					n4 *= 10;
					n4 += (int)s4[x] - (int)'0';
				}
				if (s4[0] == '-') n4 = -n4;
			}
			registerArray[registerMap[s2]] = n3 <= n4;
		}
		
		if (s1 == "slt") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			int n3 = registerArray[registerMap[s3]];
			int n4;
			if (s4[0] == '$') n4 = registerArray[registerMap[s4]];
			else {
				n4 = 0;
				int x = 0;
				if (s4[0] == '-') x++;
				for (; x < s4.length(); x++) {
					n4 *= 10;
					n4 += (int)s4[x] - (int)'0';
				}
				if (s4[0] == '-') n4 = -n4;
			}
			registerArray[registerMap[s2]] = n3 < n4;
		}
		
		if (s1 == "sne") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			int n3 = registerArray[registerMap[s3]];
			int n4;
			if (s4[0] == '$') n4 = registerArray[registerMap[s4]];
			else {
				n4 = 0;
				int x = 0;
				if (s4[0] == '-') x++;
				for (; x < s4.length(); x++) {
					n4 *= 10;
					n4 += (int)s4[x] - (int)'0';
				}
				if (s4[0] == '-') n4 = -n4;
			}
			registerArray[registerMap[s2]] = n3 != n4;
		}
		
		if (s1 == "b") {
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			z = labelMap[s2].lineNumber - 1;
		}
		
		if (s1 == "beq") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			int n2 = registerArray[registerMap[s2]];
			int n3;
			if (s3[0] == '$') n3 = registerArray[registerMap[s3]];
			else {
				n3 = 0;
				int x = 0;
				if (s3[0] == '-') x++;
				for (; x < s3.length(); x++) {
					n3 *= 10;
					n3 += (int)s3[x] - (int)'0';
				}
				if (s3[0] == '-') n3 = -n3;
			}
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			if (n2 == n3) {
				z = labelMap[s4].lineNumber - 1;
			}
		}
		
		if (s1 == "bne") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			int n2 = registerArray[registerMap[s2]];
			int n3;
			if (s3[0] == '$') n3 = registerArray[registerMap[s3]];
			else {
				n3 = 0;
				int x = 0;
				if (s3[0] == '-') x++;
				for (; x < s3.length(); x++) {
					n3 *= 10;
					n3 += (int)s3[x] - (int)'0';
				}
				if (s3[0] == '-') n3 = -n3;
			}
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			if (n2 != n3) {
				z = labelMap[s4].lineNumber - 1;
			}
		}
		
		if (s1 == "bge") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			int n2 = registerArray[registerMap[s2]];
			int n3;
			if (s3[0] == '$') n3 = registerArray[registerMap[s3]];
			else {
				n3 = 0;
				int x = 0;
				if (s3[0] == '-') x++;
				for (; x < s3.length(); x++) {
					n3 *= 10;
					n3 += (int)s3[x] - (int)'0';
				}
				if (s3[0] == '-') n3 = -n3;
			}
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			if (n2 >= n3) {
				z = labelMap[s4].lineNumber - 1;
			}
		}
		
		if (s1 == "ble") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			int n2 = registerArray[registerMap[s2]];
			int n3;
			if (s3[0] == '$') n3 = registerArray[registerMap[s3]];
			else {
				n3 = 0;
				int x = 0;
				if (s3[0] == '-') x++;
				for (; x < s3.length(); x++) {
					n3 *= 10;
					n3 += (int)s3[x] - (int)'0';
				}
				if (s3[0] == '-') n3 = -n3;
			}
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			if (n2 <= n3) {
				z = labelMap[s4].lineNumber - 1;
			}
		}
		
		if (s1 == "bgt") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			int n2 = registerArray[registerMap[s2]];
			int n3;
			if (s3[0] == '$') n3 = registerArray[registerMap[s3]];
			else {
				n3 = 0;
				int x = 0;
				if (s3[0] == '-') x++;
				for (; x < s3.length(); x++) {
					n3 *= 10;
					n3 += (int)s3[x] - (int)'0';
				}
				if (s3[0] == '-') n3 = -n3;
			}
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			if (n2 > n3) {
				z = labelMap[s4].lineNumber - 1;
			}
		}
		
		if (s1 == "blt") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			int n2 = registerArray[registerMap[s2]];
			int n3;
			if (s3[0] == '$') n3 = registerArray[registerMap[s3]];
			else {
				n3 = 0;
				int x = 0;
				if (s3[0] == '-') x++;
				for (; x < s3.length(); x++) {
					n3 *= 10;
					n3 += (int)s3[x] - (int)'0';
				}
				if (s3[0] == '-') n3 = -n3;
			}
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			if (n2 < n3) {
				z = labelMap[s4].lineNumber - 1;
			}
		}
		
		if (s1 == "beqz") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			int n2 = registerArray[registerMap[s2]];
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			if (n2 == 0) {
				z = labelMap[s3].lineNumber - 1;
			}
		}
		
		if (s1 == "bnez") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			int n2 = registerArray[registerMap[s2]];
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			if (n2 != 0) {
				z = labelMap[s3].lineNumber - 1;
			}
		}
		
		if (s1 == "blez") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			int n2 = registerArray[registerMap[s2]];
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			if (n2 <= 0) {
				z = labelMap[s3].lineNumber - 1;
			}
		}
		
		if (s1 == "bgez") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			int n2 = registerArray[registerMap[s2]];
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			if (n2 >= 0) {
				z = labelMap[s3].lineNumber - 1;
			}
		}
		
		if (s1 == "bgtz") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			int n2 = registerArray[registerMap[s2]];
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			if (n2 > 0) {
				z = labelMap[s3].lineNumber - 1;
			}
		}
		
		if (s1 == "bltz") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			int n2 = registerArray[registerMap[s2]];
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			if (n2 < 0) {
				z = labelMap[s3].lineNumber - 1;
			}
		}
		
		if (s1 == "j") {
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			z = labelMap[s2].lineNumber - 1;
		}
		
		if (s1 == "jr") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			z = registerArray[registerMap[s2]] - 1;
		}
		
		if (s1 == "jal") {
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			registerArray[registerMap["$ra"]] = z + 1;
			z = labelMap[s2].lineNumber - 1;
		}
		
		if (s1 == "jalr") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			registerArray[registerMap["$ra"]] = z + 1;
			z = registerArray[registerMap[s2]] - 1;
		}
		
		
		if (s1 == "la") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			bool isofst = false;
			int x = 0;
			for (; x < s3.length (); x++) {
				if (s3[x] == '(') {
					isofst = true;
					break;
				}
			}
			int addr;
			if (isofst) {
				int ofst = 0;
				x = 0;
				if (s3[0] == '-') x++;
				for (; x < s3.length() && s3[x] != '('; x++) {
					ofst *= 10;
					ofst += (int)s3[x] - (int)'0';
				}
				if (s3[0] == '-') ofst = -ofst;
				x++;
				int y = 0;
				while (x + y < prog[z].length() && s3[x + y] != ')') y++;
				string ss3 = s3.substr (x, y);
				addr = registerArray[registerMap[ss3]] + ofst;
			}
			else {
				addr = labelMap[s3].ramNumber;
			}
			registerArray[registerMap[s2]] = addr;
		}
		
		if (s1 == "lb") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			bool isofst = false;
			int x = 0;
			for (; x < s3.length (); x++) {
				if (s3[x] == '(') {
					isofst = true;
					break;
				}
			}
			int addr;
			if (isofst) {
				int ofst = 0;
				x = 0;
				if (s3[0] == '-') x++;
				for (; x < s3.length() && s3[x] != '('; x++) {
					ofst *= 10;
					ofst += (int)s3[x] - (int)'0';
				}
				if (s3[0] == '-') ofst = -ofst;
				x++;
				int y = 0;
				while (x + y < prog[z].length() && s3[x + y] != ')') y++;
				string ss3 = s3.substr (x, y);
				addr = registerArray[registerMap[ss3]] + ofst;
			}
			else {
				addr = labelMap[s3].ramNumber;
			}
			registerArray[registerMap[s2]] = (int)ram[addr];
		}
		
		if (s1 == "lh") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			bool isofst = false;
			int x = 0;
			for (; x < s3.length (); x++) {
				if (s3[x] == '(') {
					isofst = true;
					break;
				}
			}
			int addr;
			if (isofst) {
				int ofst = 0;
				x = 0;
				if (s3[0] == '-') x++;
				for (; x < s3.length() && s3[x] != '('; x++) {
					ofst *= 10;
					ofst += (int)s3[x] - (int)'0';
				}
				if (s3[0] == '-') ofst = -ofst;
				x++;
				int y = 0;
				while (x + y < prog[z].length() && s3[x + y] != ')') y++;
				string ss3 = s3.substr (x, y);
				addr = registerArray[registerMap[ss3]] + ofst;
			}
			else {
				addr = labelMap[s3].ramNumber;
			}
			registerArray[registerMap[s2]] = ((int)ram[addr] << 8) + (int)ram[addr + 1];
		}
		
		if (s1 == "lw") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			bool isofst = false;
			int x = 0;
			for (; x < s3.length (); x++) {
				if (s3[x] == '(') {
					isofst = true;
					break;
				}
			}
			int addr;
			if (isofst) {
				int ofst = 0;
				x = 0;
				if (s3[0] == '-') x++;
				for (; x < s3.length() && s3[x] != '('; x++) {
					ofst *= 10;
					ofst += (int)s3[x] - (int)'0';
				}
				if (s3[0] == '-') ofst = -ofst;
				x++;
				int y = 0;
				while (x + y < prog[z].length() && s3[x + y] != ')') y++;
				string ss3 = s3.substr (x, y);
				addr = registerArray[registerMap[ss3]] + ofst;
			}
			else {
				addr = labelMap[s3].ramNumber;
			}
			registerArray[registerMap[s2]] = ((int)ram[addr] << 24) + ((int)ram[addr + 1] << 16) + ((int)ram[addr + 2] << 8) + (int)ram[addr + 3];
		}
		
		if (s1 == "sb") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			bool isofst = false;
			int x = 0;
			for (; x < s3.length (); x++) {
				if (s3[x] == '(') {
					isofst = true;
					break;
				}
			}
			int addr;
			if (isofst) {
				int ofst = 0;
				x = 0;
				if (s3[0] == '-') x++;
				for (; x < s3.length() && s3[x] != '('; x++) {
					ofst *= 10;
					ofst += (int)s3[x] - (int)'0';
				}
				if (s3[0] == '-') ofst = -ofst;
				x++;
				int y = 0;
				while (x + y < prog[z].length() && s3[x + y] != ')') y++;
				string ss3 = s3.substr (x, y);
				addr = registerArray[registerMap[ss3]] + ofst;
			}
			else {
				addr = labelMap[s3].ramNumber;
			}
			ram[addr] = registerArray[registerMap[s2]] & (1 << 8) - 1;
			typ[addr] = 1;
		}
		
		if (s1 == "sh") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			bool isofst = false;
			int x = 0;
			for (; x < s3.length (); x++) {
				if (s3[x] == '(') {
					isofst = true;
					break;
				}
			}
			int addr;
			if (isofst) {
				int ofst = 0;
				x = 0;
				if (s3[0] == '-') x++;
				for (; x < s3.length() && s3[x] != '('; x++) {
					ofst *= 10;
					ofst += (int)s3[x] - (int)'0';
				}
				if (s3[0] == '-') ofst = -ofst;
				x++;
				int y = 0;
				while (x + y < prog[z].length() && s3[x + y] != ')') y++;
				string ss3 = s3.substr (x, y);
				addr = registerArray[registerMap[ss3]] + ofst;
			}
			else {
				addr = labelMap[s3].ramNumber;
			}
			ram[addr] = registerArray[registerMap[s2]] >> 8 & (1 << 8) - 1;
			ram[addr + 1] = registerArray[registerMap[s2]] & (1 << 8) - 1;
			typ[addr] = 2;
			typ[addr + 1] = 2;
		}
		
		if (s1 == "sw") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
			while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			bool isofst = false;
			int x = 0;
			for (; x < s3.length (); x++) {
				if (s3[x] == '(') {
					isofst = true;
					break;
				}
			}
			int addr;
			if (isofst) {
				int ofst = 0;
				x = 0;
				if (s3[0] == '-') x++;
				for (; x < s3.length() && s3[x] != '('; x++) {
					ofst *= 10;
					ofst += (int)s3[x] - (int)'0';
				}
				if (s3[0] == '-') ofst = -ofst;
				x++;
				int y = 0;
				while (x + y < prog[z].length() && s3[x + y] != ')') y++;
				string ss3 = s3.substr (x, y);
				addr = registerArray[registerMap[ss3]] + ofst;
			}
			else {
				addr = labelMap[s3].ramNumber;
			}
			ram[addr] = registerArray[registerMap[s2]] >> 24 & (1 << 8) - 1;
			ram[addr + 1] = registerArray[registerMap[s2]] >> 16 & (1 << 8) - 1;
			ram[addr + 2] = registerArray[registerMap[s2]] >> 8 & (1 << 8) - 1;
			ram[addr + 3] = registerArray[registerMap[s2]] & (1 << 8) - 1;
			typ[addr] = 3;
			typ[addr + 1] = 3;
			typ[addr + 2] = 3;
			typ[addr + 3] = 3;
			
		}
		
		if (s1 == "move") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s3 = prog[z].substr (i, j);
			i += j;
			registerArray[registerMap[s2]] = registerArray[registerMap[s3]];
		}
		
		if (s1 == "mfhi") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			registerArray[registerMap[s2]] = registerArray[registerMap["$hi"]];
		}
		
		if (s1 == "mflo") {
			j = 0;
			while (i < prog[z].length() && !isregi (prog[z][i])) i++;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s2 = prog[z].substr (i, j);
			i += j;
			registerArray[registerMap[s2]] = registerArray[registerMap["$lo"]];
		}
		
		if (s1 == "nop") {
		}
		
		if (s1 == "syscall") {
			if (registerArray[registerMap["$v0"]] == 1) {
				printf ("%d", registerArray[registerMap["$a0"]]);
				continue;
			}
			if (registerArray[registerMap["$v0"]] == 4) {
				int addr = registerArray[registerMap["$a0"]];
				while (ram[addr] != '\0') {
					printf ("%c", ram[addr]);
					addr++;
				}
				//cout << "              !!!           ";
				continue;
			}
			if (registerArray[registerMap["$v0"]] == 5) {
				scanf ("%d", &registerArray[registerMap["$v0"]]);
				continue;
			}
			if (registerArray[registerMap["$v0"]] == 8) {
				int leng = registerArray[registerMap["$a1"]];
				int addr = registerArray[registerMap["$a0"]];
				char c = getchar();
				while (c == '\n') c = getchar ();
				int x;
				for (x = 0; x < leng - 1 && !unshown (c); x++) {
					ram[addr + x] = c;
					typ[addr + x] = 1;
					c = getchar();
				}
				ram[addr + x] = '\0';
				typ[addr + x] = 1;
				continue;
			}
			if (registerArray[registerMap["$v0"]] == 9) {
				registerArray[registerMap["$v0"]] = heapTail;
				heapTail += registerArray[registerMap["$a0"]];
				continue;
			}
			if (registerArray[registerMap["$v0"]] == 10) {
				return 0;
			}
			if (registerArray[registerMap["$v0"]] == 17) {
				return registerArray[registerMap["$a0"]];
			}
		}
		//cout << z+1 << ' ' << heapTail << ' ' << registerArray[registerMap["$v0"]] << ' ' << registerArray[registerMap["$a0"]] << ' ';
		//cout << (int)ram[labelMap["_static_1"].ramNumber + 3] << ' ' << registerArray[registerMap["$sp"]] << ' ' << registerArray[registerMap["$fp"]] << endl;
		//cout <<" 1:" << registerArray[registerMap["$t1"]] << " 2:" << registerArray[registerMap["$t2"]]<< " 3:" << registerArray[registerMap["$t3"]]<< " 4:" << registerArray[registerMap["$t4"]]<< " 5:" << registerArray[registerMap["$t5"]] << endl;
	}
	return 0;
}




int main (int argc, char **argv) {
	
	
	//freopen ("test1.in", "r", stdin);
	//freopen ("superloop-5090379042-jiaxiao.s", "r", stdin);
	//freopen ("mips.out", "w", stdout);
	ifstream infile(argv[1]);
	pre ();
	progLines = 0;
	string s;
	getline (infile, s);
	while (!infile.eof()) {
		prog.push_back (s);
		progLines++;
		getline (infile, s);
	}
	heapTail = 0;
	stackTop = 4 * 1024 * 1024;
	memset (ram, 0, sizeof(ram));
	memset (typ, 0, sizeof(typ));
	
	//freopen ("superloop-5090379042-jiaxiao.in", "r", stdin);
	
	simulatePointData ();
	
	
	
	
	/*
	cout << heapTail << endl;
	for (int t = 0; t < heapTail; t++) {
		cout << (int)ram[t];
	}
	cout << endl;
	for (int t = 0; t < heapTail; t++) {
		cout << (int)typ[t];
	}
	cout << endl << endl;
	map <string, label> :: iterator it;
	it = labelMap.begin ();
	for (;it != labelMap.end (); ++it) {
		cout << (*it).first << ' ' << (*it).second.lineNumber << ' ' << (*it).second.ramNumber << endl;
	}
	*/
	int ans = simulatePointText ();
	
	return ans;
	
}
