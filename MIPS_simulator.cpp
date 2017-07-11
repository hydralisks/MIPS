#include<map>
#include<string>
#include<cmath>
#include<cstdio>
#include<cstring>
#include<iostream>
#include<algorithm>
#include <vector>
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




int fetchData[10];
int preparationData[10];
int executionData[10];
int accessData[10];
int writeData[10];

string fetchCommand;
string preparationCommand;
string executionCommand;
string accessCommand;
string writeCommand;

string inp;

bool killed;
int remain;
bool hazard;
int in[233];

int z;

void fetch () {
	z++;
	//cout << z << endl;
	//cout << registerArray[registerMap["$10"]] << ' ' << registerArray[registerMap["$9"]] << ' ' << (int)ram[registerArray[registerMap["$9"]] + 3] << endl << endl;
	/*
	cout << "fetch  : " << z << endl;
	for (int i = 0; i < 34; i++) cout << in[i] << ' ';
	cout << endl;
	*/
	
	
	
	
	
	
	
	
	
	
	
	
	
}

void prepatation () {
	int i = 0, j = 0;
	while (i < prog[z].length() && unshown (prog[z][i])) i++;
	while (i + j < prog[z].length() && !unshown (prog[z][i + j])) j++;
	string s1 = prog[z].substr (i, j);
	i += j;
	
	/*
	cout << "prepatation  : " << prog[z] << endl;
	for (int i = 0; i < 34; i++) cout << in[i] << ' ';
	cout << endl << registerArray[31] << endl;
	*/
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
		
	if (s1 == "add" || s1 == "addu" || s1 == "addiu") {
		executionCommand = "add";
		executionData[0] = -1;
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
		if (in[registerMap[s3]]) {
			hazard = true;
			return;
		}
		int n3 = registerArray[registerMap[s3]];
		int n4;
		if (s4[0] == '$') {
			n4 = registerArray[registerMap[s4]];
			if (in[registerMap[s4]]) {
				hazard = true;
				return;
			}
		}
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
		executionData[1] = registerMap[s2];
		executionData[2] = n3;
		executionData[3] = n4;
		in[registerMap[s2]]++;
	}
		
	if (s1 == "sub" || s1 == "subu") {
		executionCommand = "sub";
		executionData[0] = -1;
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
		if (in[registerMap[s3]]) {
			hazard = true;
			return;
		}
		int n3 = registerArray[registerMap[s3]];
		int n4;
		if (s4[0] == '$') {
			if (in[registerMap[s4]]) {
				hazard = true;
				return;
			}
			n4 = registerArray[registerMap[s4]];
		}
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
		executionData[1] = registerMap[s2];
		executionData[2] = n3;
		executionData[3] = n4;
		in[registerMap[s2]]++;
	}
		
	if (s1 == "mul" || s1 == "mulu") {
		executionCommand = "mul";
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
			executionData[0] = 0;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			if (in[registerMap[s3]]) {
				hazard = true;
				return;
			}
			int n3 = registerArray[registerMap[s3]];
			int n4;
			if (s4[0] == '$') {
				if (in[registerMap[s4]]) {
					hazard = true;
					return;
				}
				n4 = registerArray[registerMap[s4]];
			}
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
			executionData[1] = registerMap[s2];
			executionData[2] = n3;
			executionData[3] = n4;
			in[registerMap[s2]]++;
		}
		else {
			executionData[0] = 1;
			if (in[registerMap[s2]]) {
				hazard = true;
				return;
			}
			int n2 = registerArray[registerMap[s2]];
			int n3;
			if (s3[0] == '$') {
				if (in[registerMap[s3]]) {
					hazard = true;
					return;
				}
				n3 = registerArray[registerMap[s3]];
			}
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
			executionData[2] = n2;
			executionData[3] = n3;
			in[registerMap["$hi"]]++;
			in[registerMap["$lo"]]++;
		}
	}
		
	if (s1 == "div" || s1 == "divu") {
		executionCommand = "div";
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
			executionData[0] = 0;
			while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
			string s4 = prog[z].substr (i, j);
			i += j;
			if (in[registerMap[s3]]) {
				hazard = true;
				return;
			}
			int n3 = registerArray[registerMap[s3]];
			int n4;
			if (s4[0] == '$') {
				if (in[registerMap[s4]]) {
					hazard = true;
					return;
				}
				n4 = registerArray[registerMap[s4]];
			}
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
			executionData[1] = registerMap[s2];
			executionData[2] = n3;
			executionData[3] = n4;
			in[registerMap[s2]]++;
		}
		else {
			executionData[0] = 1;
			if (in[registerMap[s2]]) {
				hazard = true;
				return;
			}
			int n2 = registerArray[registerMap[s2]];
			int n3;
			if (s3[0] == '$') {
				if (in[registerMap[s3]]) {
					hazard = true;
					return;
				}
				n3 = registerArray[registerMap[s3]];
			}
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
			executionData[2] = n2;
			executionData[3] = n3;
			in[registerMap["$hi"]]++;
			in[registerMap["$lo"]]++;
		}
	}
		
	if (s1 == "xor" || s1 == "xoru") {
		executionCommand = "xor";
		executionData[0] = -1;
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
		if (in[registerMap[s3]]) {
			hazard = true;
			return;
		}
		int n3 = registerArray[registerMap[s3]];
		int n4;
		if (s4[0] == '$') {
			if (in[registerMap[s4]]) {
				hazard = true;
				return;
			}
			n4 = registerArray[registerMap[s4]];
		}
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
		executionData[1] = registerMap[s2];
		executionData[2] = n3;
		executionData[3] = n4;
		in[registerMap[s2]]++;
	}
	
	if (s1 == "neg" || s1 == "negu") {
		executionCommand = "neg";
		executionData[0] = -1;
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
		if (in[registerMap[s3]]) {
			hazard = true;
			return;
		}
		int n3 = registerArray[registerMap[s3]];
		executionData[1] = registerMap[s2];
		executionData[2] = n3;
		in[registerMap[s2]]++;
	}
		
	if (s1 == "rem" || s1 == "remu") {
		executionCommand = "rem";
		executionData[0] = -1;
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
		if (in[registerMap[s3]]) {
			hazard = true;
			return;
		}
		int n3 = registerArray[registerMap[s3]];
		int n4;
		if (s4[0] == '$') {
			if (in[registerMap[s4]]) {
				hazard = true;
				return;
			}
			n4 = registerArray[registerMap[s4]];
		}
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
		executionData[1] = registerMap[s2];
		executionData[2] = n3;
		executionData[3] = n4;
		in[registerMap[s2]]++;
	}
		
	if (s1 == "li") {
		executionCommand = "li";
		executionData[0] = -1;
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
		executionData[1] = registerMap[s2];
		executionData[2] = n3;
		in[registerMap[s2]]++;
	}
	
		
		
		
	if (s1 == "seq") {
		executionCommand = "seq";
		executionData[0] = -1;
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
		if (in[registerMap[s3]]) {
			hazard = true;
			return;
		}
		int n3 = registerArray[registerMap[s3]];
		int n4;
		if (s4[0] == '$') {
			if (in[registerMap[s4]]) {
				hazard = true;
				return;
			}
			n4 = registerArray[registerMap[s4]];
		}
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
		executionData[1] = registerMap[s2];
		executionData[2] = n3;
		executionData[3] = n4;
		in[registerMap[s2]]++;
	}
	
	if (s1 == "sge") {
		executionCommand = "sge";
		executionData[0] = -1;
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
		if (in[registerMap[s3]]) {
			hazard = true;
			return;
		}
		int n3 = registerArray[registerMap[s3]];
		int n4;
		if (s4[0] == '$') {
			if (in[registerMap[s4]]) {
				hazard = true;
				return;
			}
			n4 = registerArray[registerMap[s4]];
		}
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
		executionData[1] = registerMap[s2];
		executionData[2] = n3;
		executionData[3] = n4;
		in[registerMap[s2]]++;
	}
		
	if (s1 == "sgt") {
		executionCommand = "sgt";
		executionData[0] = -1;
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
		if (in[registerMap[s3]]) {
			hazard = true;
			return;
		}
		int n3 = registerArray[registerMap[s3]];
		int n4;
		if (s4[0] == '$') {
			if (in[registerMap[s4]]) {
				hazard = true;
				return;
			}
			n4 = registerArray[registerMap[s4]];
		}
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
		executionData[1] = registerMap[s2];
		executionData[2] = n3;
		executionData[3] = n4;
		in[registerMap[s2]]++;
	}
		
	if (s1 == "sle") {
		executionCommand = "sle";
		executionData[0] = -1;
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
		if (in[registerMap[s3]]) {
			hazard = true;
			return;
		}
		int n3 = registerArray[registerMap[s3]];
		int n4;
		if (s4[0] == '$') {
			if (in[registerMap[s4]]) {
				hazard = true;
				return;
			}
			n4 = registerArray[registerMap[s4]];
		}
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
		executionData[1] = registerMap[s2];
		executionData[2] = n3;
		executionData[3] = n4;
		in[registerMap[s2]]++;
	}
	
	if (s1 == "slt") {
		executionCommand = "slt";
		executionData[0] = -1;
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
		if (in[registerMap[s3]]) {
			hazard = true;
			return;
		}
		int n3 = registerArray[registerMap[s3]];
		int n4;
		if (s4[0] == '$') {
			if (in[registerMap[s4]]) {
				hazard = true;
				return;
			}
			n4 = registerArray[registerMap[s4]];
		}
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
		executionData[1] = registerMap[s2];
		executionData[2] = n3;
		executionData[3] = n4;
		in[registerMap[s2]]++;
	}
		
	if (s1 == "sne") {
		executionCommand = "sne";
		executionData[0] = -1;
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
		if (in[registerMap[s3]]) {
			hazard = true;
			return;
		}
		int n3 = registerArray[registerMap[s3]];
		int n4;
		if (s4[0] == '$') {
			if (in[registerMap[s4]]) {
				hazard = true;
				return;
			}
			n4 = registerArray[registerMap[s4]];
		}
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
		executionData[1] = registerMap[s2];
		executionData[2] = n3;
		executionData[3] = n4;
		in[registerMap[s2]]++;
	}
		
	if (s1 == "b") {
		executionCommand = "b";
		executionData[0] = -1;
		j = 0;
		while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
		while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
		string s2 = prog[z].substr (i, j);
		i += j;
		executionData[4] = labelMap[s2].lineNumber - 1;
	}
	
	if (s1 == "beq") {
		executionCommand = "beq";
		executionData[0] = -1;
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
		if (in[registerMap[s2]]) {
			hazard = true;
			return;
		}
		int n2 = registerArray[registerMap[s2]];
		int n3;
		if (s3[0] == '$') {
			if (in[registerMap[s3]]) {
				hazard = true;
				return;
			}
			n3 = registerArray[registerMap[s3]];
		}
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
		executionData[2] = n2;
		executionData[3] = n3;
		executionData[4] = labelMap[s4].lineNumber - 1;
		executionData[9] = z;
	}
	
	if (s1 == "bne") {
		executionCommand = "bne";
		executionData[0] = -1;
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
		if (in[registerMap[s2]]) {
			hazard = true;
			return;
		}
		int n2 = registerArray[registerMap[s2]];
		int n3;
		if (s3[0] == '$') {
			if (in[registerMap[s3]]) {
				hazard = true;
				return;
			}
			n3 = registerArray[registerMap[s3]];
		}
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
		executionData[2] = n2;
		executionData[3] = n3;
		executionData[4] = labelMap[s4].lineNumber - 1;
		executionData[9] = z;
	}
		
	if (s1 == "bge") {
		executionCommand = "bge";
		executionData[0] = -1;
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
		if (in[registerMap[s2]]) {
			hazard = true;
			return;
		}
		int n2 = registerArray[registerMap[s2]];
		int n3;
		if (s3[0] == '$') {
			if (in[registerMap[s3]]) {
				hazard = true;
				return;
			}
			n3 = registerArray[registerMap[s3]];
		}
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
		executionData[2] = n2;
		executionData[3] = n3;
		executionData[4] = labelMap[s4].lineNumber - 1;
		executionData[9] = z;
	}
		
	if (s1 == "ble") {
		executionCommand = "ble";
		executionData[0] = -1;
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
		if (in[registerMap[s2]]) {
			hazard = true;
			return;
		}
		int n2 = registerArray[registerMap[s2]];
		int n3;
		if (s3[0] == '$') {
			if (in[registerMap[s3]]) {
				hazard = true;
				return;
			}
			n3 = registerArray[registerMap[s3]];
		}
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
		executionData[2] = n2;
		executionData[3] = n3;
		executionData[4] = labelMap[s4].lineNumber - 1;
		executionData[9] = z;
	}
		
	if (s1 == "bgt") {
		executionCommand = "bgt";
		executionData[0] = -1;
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
		if (in[registerMap[s2]]) {
			hazard = true;
			return;
		}
		int n2 = registerArray[registerMap[s2]];
		int n3;
		if (s3[0] == '$') {
			if (in[registerMap[s3]]) {
				hazard = true;
				return;
			}
			n3 = registerArray[registerMap[s3]];
		}
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
		executionData[2] = n2;
		executionData[3] = n3;
		executionData[4] = labelMap[s4].lineNumber - 1;
		executionData[9] = z;
	}
		
	if (s1 == "blt") {
		executionCommand = "blt";
		executionData[0] = -1;
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
		if (in[registerMap[s2]]) {
			hazard = true;
			return;
		}
		int n2 = registerArray[registerMap[s2]];
		int n3;
		if (s3[0] == '$') {
			if (in[registerMap[s3]]) {
				hazard = true;
				return;
			}
			n3 = registerArray[registerMap[s3]];
		}
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
		executionData[2] = n2;
		executionData[3] = n3;
		executionData[4] = labelMap[s4].lineNumber - 1;
		executionData[9] = z;
	}
		
	if (s1 == "beqz") {
		executionCommand = "beqz";
		executionData[0] = -1;
		j = 0;
		while (i < prog[z].length() && !isregi (prog[z][i])) i++;
		while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
		string s2 = prog[z].substr (i, j);
		i += j;
		if (in[registerMap[s2]]) {
			hazard = true;
			return;
		}
		int n2 = registerArray[registerMap[s2]];
		j = 0;
		while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
		while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
		string s3 = prog[z].substr (i, j);
		i += j;
		executionData[2] = n2;
		executionData[4] = labelMap[s3].lineNumber - 1;
		executionData[9] = z;
	}
		
	if (s1 == "bnez") {
		executionCommand = "bnez";
		executionData[0] = -1;
		j = 0;
		while (i < prog[z].length() && !isregi (prog[z][i])) i++;
		while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
		string s2 = prog[z].substr (i, j);
		i += j;
		if (in[registerMap[s2]]) {
			hazard = true;
			return;
		}
		int n2 = registerArray[registerMap[s2]];
		j = 0;
		while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
		while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
		string s3 = prog[z].substr (i, j);
		i += j;
		executionData[2] = n2;
		executionData[4] = labelMap[s3].lineNumber - 1;
		executionData[9] = z;
	}
	
	if (s1 == "blez") {
		executionCommand = "blez";
		executionData[0] = -1;
		j = 0;
		while (i < prog[z].length() && !isregi (prog[z][i])) i++;
		while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
		string s2 = prog[z].substr (i, j);
		i += j;
		if (in[registerMap[s2]]) {
			hazard = true;
			return;
		}
		int n2 = registerArray[registerMap[s2]];
		j = 0;
		while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
		while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
		string s3 = prog[z].substr (i, j);
		i += j;
		executionData[2] = n2;
		executionData[4] = labelMap[s3].lineNumber - 1;
		executionData[9] = z;
	}
	
	if (s1 == "bgez") {
		executionCommand = "bgez";
		executionData[0] = -1;
		j = 0;
		while (i < prog[z].length() && !isregi (prog[z][i])) i++;
		while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
		string s2 = prog[z].substr (i, j);
		i += j;
		if (in[registerMap[s2]]) {
			hazard = true;
			return;
		}
		int n2 = registerArray[registerMap[s2]];
		j = 0;
		while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
		while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
		string s3 = prog[z].substr (i, j);
		i += j;
		executionData[2] = n2;
		executionData[4] = labelMap[s3].lineNumber - 1;
		executionData[9] = z;
	}
		
	if (s1 == "bgtz") {
		executionCommand = "bgtz";
		executionData[0] = -1;
		j = 0;
		while (i < prog[z].length() && !isregi (prog[z][i])) i++;
		while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
		string s2 = prog[z].substr (i, j);
		i += j;
		if (in[registerMap[s2]]) {
			hazard = true;
			return;
		}
		int n2 = registerArray[registerMap[s2]];
		j = 0;
		while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
		while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
		string s3 = prog[z].substr (i, j);
		i += j;
		executionData[2] = n2;
		executionData[4] = labelMap[s3].lineNumber - 1;
		executionData[9] = z;
	}
		
	if (s1 == "bltz") {
		executionCommand = "bltz";
		executionData[0] = -1;
		j = 0;
		while (i < prog[z].length() && !isregi (prog[z][i])) i++;
		while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
		string s2 = prog[z].substr (i, j);
		i += j;
		if (in[registerMap[s2]]) {
			hazard = true;
			return;
		}
		int n2 = registerArray[registerMap[s2]];
		j = 0;
		while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
		while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
		string s3 = prog[z].substr (i, j);
		i += j;
		executionData[2] = n2;
		executionData[4] = labelMap[s3].lineNumber - 1;
		executionData[9] = z;
	}
		
	if (s1 == "j") {
		executionCommand = "j";
		executionData[0] = -1;
		j = 0;
		while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
		while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
		string s2 = prog[z].substr (i, j);
		i += j;
		executionData[4] = labelMap[s2].lineNumber - 1;
	}
		
	if (s1 == "jr") {
		executionCommand = "jr";
		executionData[0] = -1;
		j = 0;
		while (i < prog[z].length() && !isregi (prog[z][i])) i++;
		while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
		string s2 = prog[z].substr (i, j);
		i += j;
		if (in[registerMap[s2]]) {
			hazard = true;
			return;
		}
		executionData[4] = registerArray[registerMap[s2]] - 1;
	}
	
	if (s1 == "jal") {
		executionCommand = "jal";
		executionData[0] = -1;
		j = 0;
		while (i < prog[z].length() && (unshown (prog[z][i]) || prog[z][i] == ',')) i++;
		while (i + j < prog[z].length() && !unshown (prog[z][i + j]) && prog[z][i + j] != ',') j++;
		string s2 = prog[z].substr (i, j);
		i += j;
		executionData[2] = z + 1;
		executionData[4] = labelMap[s2].lineNumber - 1;
		in[registerMap["$ra"]]++;
	}
		
	if (s1 == "jalr") {
		executionCommand = "jalr";
		executionData[0] = -1;
		j = 0;
		while (i < prog[z].length() && !isregi (prog[z][i])) i++;
		while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
		string s2 = prog[z].substr (i, j);
		i += j;
		if (in[registerMap[s2]]) {
			hazard = true;
			return;
		}
		executionData[2] = z + 1;
		executionData[4] = registerArray[registerMap[s2]] - 1;
		in[registerMap["$ra"]]++;
	}
		
		
	if (s1 == "la" || s1 == "lb" || s1 == "lh" || s1 == "lw") {
		executionCommand = s1;
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
			if (in[registerMap[ss3]]) {
				hazard = true;
				return;
			}
			executionData[0] = 0;
			executionData[1] = registerMap[s2];
			executionData[2] = ofst;
			executionData[5] = registerArray[registerMap[ss3]];
			in[registerMap[s2]]++;
		}
		else {
			executionData[0] = 1;
			executionData[1] = registerMap[s2];
			executionData[5] = labelMap[s3].ramNumber;
			in[registerMap[s2]]++;
		}
	}
	
	if (s1 == "sb" || s1 == "sh" || s1 == "sw") {
		executionCommand = s1;
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
			if (in[registerMap[s2]]) {
				hazard = true;
				return;
			}
			if (in[registerMap[ss3]]) {
				hazard = true;
				return;
			}
			executionData[0] = 0;
			executionData[2] = registerArray[registerMap[s2]];
			executionData[3] = ofst;
			executionData[5] = registerArray[registerMap[ss3]];
		}
		else {
			if (in[registerMap[s2]]) {
				hazard = true;
				return;
			}
			executionData[0] = 1;
			executionData[2] = registerArray[registerMap[s2]];
			executionData[5] = labelMap[s3].ramNumber;
		}
	}
		
	
		
	if (s1 == "move") {
		executionCommand = "move";
		executionData[0] = -1;
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
		if (in[registerMap[s3]]) {
			hazard = true;
			return;
		}
		executionData[1] = registerMap[s2];
		executionData[2] = registerArray[registerMap[s3]];
		in[registerMap[s2]]++;
	}
		
	if (s1 == "mfhi") {
		executionCommand = "mfhi";
		executionData[0] = -1;
		j = 0;
		while (i < prog[z].length() && !isregi (prog[z][i])) i++;
		while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
		string s2 = prog[z].substr (i, j);
		i += j;
		if (in[registerMap["$hi"]]) {
			hazard = true;
			return;
		}
		executionData[1] = registerMap[s2];
		executionData[2] = registerArray[registerMap["$hi"]];
		in[registerMap[s2]]++;
	}
		
	if (s1 == "mflo") {
		executionCommand = "mflo";
		executionData[0] = -1;
		j = 0;
		while (i < prog[z].length() && !isregi (prog[z][i])) i++;
		while (i + j < prog[z].length() && isregi (prog[z][i + j])) j++;
		string s2 = prog[z].substr (i, j);
		i += j;
		if (in[registerMap["$lo"]]) {
			hazard = true;
			return;
		}
		executionData[1] = registerMap[s2];
		executionData[2] = registerArray[registerMap["$lo"]];
		in[registerMap[s2]]++;
	}
		
	if (s1 == "nop") {
		executionCommand = "nop";
		executionData[0] = -1;
	}
		
	if (s1 == "syscall") {
		executionCommand = "syscall";
		if (in[registerMap["$v0"]]) {
			hazard = true;
			return;
		}
		if (registerArray[registerMap["$v0"]] == 1) {
			if (in[registerMap["$a0"]]) {
				hazard = true;
				return;
			}
			executionData[0] = 1;
			executionData[2] = registerArray[registerMap["$a0"]];
		}
		if (registerArray[registerMap["$v0"]] == 4) {
			if (in[registerMap["$a0"]]) {
				hazard = true;
				return;
			}
			
			//cout << registerArray[registerMap["$a0"]] << endl;
			
			
			executionData[0] = 4;
			executionData[5] = registerArray[registerMap["$a0"]];
		}
		if (registerArray[registerMap["$v0"]] == 5) {
			int x;
			scanf ("%d", &x);
			executionData[0] = 5;
			executionData[1] = registerMap["$v0"];
			executionData[2] = x;
			in[registerMap["$v0"]]++;
		}
		if (registerArray[registerMap["$v0"]] == 8) {
			if (in[registerMap["$a0"]]) {
				hazard = true;
				return;
			}
			if (in[registerMap["$a1"]]) {
				hazard = true;
				return;
			}
			int leng = registerArray[registerMap["$a1"]];
			inp = "";
			char c = getchar();
			while (c == '\n') c = getchar ();
			int x;
			for (x = 0; x < leng - 1 && !unshown (c); x++) {
				inp += c;
				c = getchar();
			}
			executionData[0] = 8;
			executionData[5] = registerArray[registerMap["$a0"]];
		}
		if (registerArray[registerMap["$v0"]] == 9) {
			if (in[registerMap["$a0"]]) {
				hazard = true;
				return;
			}
			executionData[0] = 9;
			executionData[1] = registerMap["$v0"];
			executionData[2] = heapTail;
			heapTail += registerArray[registerMap["$a0"]];
			in[registerMap["$v0"]]++;
		}
		if (registerArray[registerMap["$v0"]] == 10) {
			executionData[0] = 10;
		}
		if (registerArray[registerMap["$v0"]] == 17) {
			if (in[registerMap["$a0"]]) {
				hazard = true;
				return;
			}
			executionData[0] = 17;
			executionData[6] = registerArray[registerMap["$a0"]];
		}
	}
	
} 

void execution () {
	/*
	cout << "execution  : " << executionCommand << endl;
	for (int i = 0; i < 34; i++) cout << in[i] << ' ';
	cout << endl << registerArray[31] << endl;
	*/
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	if (executionCommand == "add") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = executionData[2] + executionData[3];
	}
	
	if (executionCommand == "sub") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = executionData[2] - executionData[3];
	}
		
	if (executionCommand == "mul") {
		if (executionData[0] == 0) {
			accessCommand = "s";
			accessData[1] = executionData[1];
			accessData[2] = executionData[2] * executionData[3];
		}
		if (executionData[0] == 1) {
			accessCommand = "ts";
			long long nn = (long long)executionData[2] * (long long)executionData[3];
			accessData[2] = nn >> 32 & (1LL << 32) - 1LL;
			accessData[3] = nn & (1LL << 32) - 1LL;
		}
	}
		
	if (executionCommand == "div") {
		if (executionData[0] == 0) {
			accessCommand = "s";
			accessData[1] = executionData[1];
			accessData[2] = executionData[2] / executionData[3];
		}
		if (executionData[0] == 1) {
			accessCommand = "ts";
			accessData[2] = executionData[2] % executionData[3];
			accessData[3] = executionData[2] / executionData[3];
		}
	}
	
	if (executionCommand == "xor") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = executionData[2] ^ executionData[3];
	}
		
	if (executionCommand == "neg") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = -executionData[2];
	}
	
	if (executionCommand == "rem") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = executionData[2] % executionData[3];
	}
		
	if (executionCommand == "li") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = executionData[2];
	}
		
	
	
		
	if (executionCommand == "seq") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = executionData[2] == executionData[3];
	}
		
	if (executionCommand == "sge") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = executionData[2] >= executionData[3];
	}
	
	if (executionCommand == "sgt") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = executionData[2] > executionData[3];
	}
		
	if (executionCommand == "sle") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = executionData[2] <= executionData[3];
	}
		
	if (executionCommand == "slt") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = executionData[2] < executionData[3];
	}
	
	if (executionCommand == "sne") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = executionData[2] != executionData[3];
	}
		
	if (executionCommand == "b") {
		accessCommand = "b";
		accessData[4] = executionData[4];
	}
		
	if (executionCommand == "beq") {
		accessCommand = "b";
		if (executionData[2] == executionData[3]) accessData[4] = executionData[4];
		else accessData[4] = executionData[9];
	}
	
	if (executionCommand == "bne") {
		accessCommand = "b";
		if (executionData[2] != executionData[3]) accessData[4] = executionData[4];
		else accessData[4] = executionData[9];
	}
	
	if (executionCommand == "bge") {
		accessCommand = "b";
		if (executionData[2] >= executionData[3]) accessData[4] = executionData[4];
		else accessData[4] = executionData[9];
	}
		
	if (executionCommand == "ble") {
		accessCommand = "b";
		if (executionData[2] <= executionData[3]) accessData[4] = executionData[4];
		else accessData[4] = executionData[9];
	}
		
	if (executionCommand == "bgt") {
		accessCommand = "b";
		if (executionData[2] > executionData[3]) accessData[4] = executionData[4];
		else accessData[4] = executionData[9];
	}
		
	if (executionCommand == "blt") {
		accessCommand = "b";
		if (executionData[2] < executionData[3]) accessData[4] = executionData[4];
		else accessData[4] = executionData[9];
	}
		
	if (executionCommand == "beqz") {
		accessCommand = "b";
		if (executionData[2] == 0) accessData[4] = executionData[4];
		else accessData[4] = executionData[9];
	}
		
	if (executionCommand == "bnez") {
		accessCommand = "b";
		if (executionData[2] != 0) accessData[4] = executionData[4];
		else accessData[4] = executionData[9];
	}
		
	if (executionCommand == "blez") {
		accessCommand = "b";
		if (executionData[2] <= 0) accessData[4] = executionData[4];
		else accessData[4] = executionData[9];
	}
	
	if (executionCommand == "bgez") {
		accessCommand = "b";
		if (executionData[2] >= 0) accessData[4] = executionData[4];
		else accessData[4] = executionData[9];
	}
	
	if (executionCommand == "bgtz") {
		accessCommand = "b";
		if (executionData[2] > 0) accessData[4] = executionData[4];
		else accessData[4] = executionData[9];
	}
		
	if (executionCommand == "bltz") {
		accessCommand = "b";
		if (executionData[2] < 0) accessData[4] = executionData[4];
		else accessData[4] = executionData[9];
	}
		
	if (executionCommand == "j") {
		accessCommand = "b";
		accessData[4] = executionData[4];
	}
		
	if (executionCommand == "jr") {
		accessCommand = "b";
		accessData[4] = executionData[4];
	}
		
	if (executionCommand == "jal") {
		accessCommand = "ja";
		accessData[2] = executionData[2];
		accessData[4] = executionData[4];
	}
	
	if (executionCommand == "jalr") {
		accessCommand = "ja";
		accessData[2] = executionData[2];
		accessData[4] = executionData[4];
	}
	
	if (executionCommand == "la" || executionCommand == "lb" || executionCommand == "lh" || executionCommand == "lw") {
		accessCommand = executionCommand;
		if (executionData[0] == 0) {
			accessData[1] = executionData[1];
			accessData[5] = executionData[5] + executionData[2];
		}
		if (executionData[0] == 1) {
			accessData[1] = executionData[1];
			accessData[5] = executionData[5];
		}
	}
	
	if (executionCommand == "sb") {
		accessCommand = "sb";
		if (executionData[0] == 0) {
			accessData[2] = executionData[2] >> 0 & (1 << 8) - 1;
			accessData[5] = executionData[5] + executionData[3];
		}
		if (executionData[0] == 1) {
			accessData[2] = executionData[2] >> 0 & (1 << 8) - 1;
			accessData[5] = executionData[5];
		}
	}
	
	if (executionCommand == "sh") {
		accessCommand = "sh";
		if (executionData[0] == 0) {
			accessData[2] = executionData[2] >> 8 & (1 << 8) - 1;
			accessData[3] = executionData[2] >> 0 & (1 << 8) - 1;
			accessData[5] = executionData[5] + executionData[3];
		}
		if (executionData[0] == 1) {
			accessData[2] = executionData[2] >> 8 & (1 << 8) - 1;
			accessData[3] = executionData[2] >> 0 & (1 << 8) - 1;
			accessData[5] = executionData[5];
		}
	}
	
	if (executionCommand == "sw") {
		accessCommand = "sw";
		if (executionData[0] == 0) {
			accessData[2] = executionData[2] >> 24 & (1 << 8) - 1;
			accessData[3] = executionData[2] >> 16 & (1 << 8) - 1;
			accessData[7] = executionData[2] >> 8 & (1 << 8) - 1;
			accessData[8] = executionData[2] >> 0 & (1 << 8) - 1;
			accessData[5] = executionData[5] + executionData[3];
		}
		if (executionData[0] == 1) {
			accessData[2] = executionData[2] >> 24 & (1 << 8) - 1;
			accessData[3] = executionData[2] >> 16 & (1 << 8) - 1;
			accessData[7] = executionData[2] >> 8 & (1 << 8) - 1;
			accessData[8] = executionData[2] >> 0 & (1 << 8) - 1;
			accessData[5] = executionData[5];
		}
	}
	
	
		
	if (executionCommand == "move") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = executionData[2];
	}
		
	if (executionCommand == "mfhi") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = executionData[2];
	}
		
	if (executionCommand == "mflo") {
		accessCommand = "s";
		accessData[1] = executionData[1];
		accessData[2] = executionData[2];
	}
		
	if (executionCommand == "nop") {
		accessCommand = "nop";
	}
		
	if (executionCommand == "syscall") {
		if (executionData[0] == 1) {
			accessCommand = "pn";
			accessData[2] = executionData[2];
		}
		if (executionData[0] == 4) {
			accessCommand = "ps";
			accessData[5] = executionData[5];
		}
		if (executionData[0] == 5) {
			accessCommand = "s";
			accessData[1] = executionData[1];
			accessData[2] = executionData[2];
		}
		if (executionData[0] == 8) {
			accessCommand = "is";
			accessData[5] = executionData[5];
		}
		if (executionData[0] == 9) {
			accessCommand = "s";
			accessData[1] = executionData[1];
			accessData[2] = executionData[2];
		}
		if (executionData[0] == 10) {
			accessCommand = "ku";
		}
		if (executionData[0] == 17) {
			accessCommand = "kn";
			accessData[6] = executionData[6];
		}
	}
	
	executionCommand = "nop";
	
}

void access () {
	/*
	cout << "access  : " << accessCommand << endl;
	for (int i = 0; i < 34; i++) cout << in[i] << ' ';
	cout << endl << registerArray[31] << endl;
	*/
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	if (accessCommand == "s") {
		writeCommand = "s";
		writeData[1] = accessData[1];
		writeData[2] = accessData[2];
	}
	
	if (accessCommand == "ts") {
		writeCommand = "ts";
		writeData[2] = accessData[2];
		writeData[3] = accessData[3];
	}
		
	if (accessCommand == "b") {
		writeCommand = "b";
		writeData[4] = accessData[4];
	}
		
	if (accessCommand == "ja") {
		writeCommand = "ja";
		writeData[2] = accessData[2];
		writeData[4] = accessData[4];
	}
		
	if (accessCommand == "la") {
		writeCommand = "s";
		writeData[1] = accessData[1];
		writeData[2] = accessData[5];
	}
	
	if (accessCommand == "lb") {
		int addr = accessData[5];
		writeCommand = "s";
		writeData[1] = accessData[1];
		writeData[2] = ((int)ram[addr] << 0);
	}
	
	if (accessCommand == "lh") {
		int addr = accessData[5];
		writeCommand = "s";
		writeData[1] = accessData[1];
		writeData[2] = ((int)ram[addr] << 8) + ((int)ram[addr + 1] << 0);
	}
	
	if (accessCommand == "lw") {
		int addr = accessData[5];
		writeCommand = "s";
		writeData[1] = accessData[1];
		writeData[2] = ((int)ram[addr] << 24) + ((int)ram[addr + 1] << 16) + ((int)ram[addr + 2] << 8) + ((int)ram[addr + 3] << 0);
	}
	
	if (accessCommand == "sb") {
		int addr = accessData[5];
		writeCommand = "nop";
		ram[addr + 0] = accessData[2];
	}
	
	if (accessCommand == "sh") {
		int addr = accessData[5];
		writeCommand = "nop";
		ram[addr + 0] = accessData[2];
		ram[addr + 1] = accessData[3];
	}
	
	if (accessCommand == "sw") {
		int addr = accessData[5];
		writeCommand = "nop";
		ram[addr + 0] = accessData[2];
		ram[addr + 1] = accessData[3];
		ram[addr + 2] = accessData[7];
		ram[addr + 3] = accessData[8];
	}
	
	if (accessCommand == "nop") {
		writeCommand = "nop";
	}
	
	if (accessCommand == "pn") {
		writeCommand = "nop";
		printf ("%d\n", accessData[2]);
	}
	
	if (accessCommand == "ps") {
		writeCommand = "nop";
		int addr = accessData[5];
		while (ram[addr] != '\0') {
			printf ("%c", ram[addr]);
			addr++;
		}
	}
	
	if (accessCommand == "is") {
		writeCommand = "nop";
		int addr = accessData[5];
		for (int x = 0; x < inp.length(); x++) {
			ram[addr + x] = inp[x];
		}
		ram[addr + inp.length()] = '\0'; 
	}
	
	if (accessCommand == "ku") {
		writeCommand = "ku";
	}
	
	if (accessCommand == "kn") {
		writeCommand = "kn";
		writeData[6] = accessData[6];
	}
	
}

void write () {
	
	/*
	cout << "write  : " << writeCommand << endl;
	for (int i = 0; i < 34; i++) cout << in[i] << ' ';
	cout << endl << registerArray[31] << ' ' << (int)ram[registerArray[29] + 3] << endl;
	*/
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	if (writeCommand == "s") {
		registerArray[writeData[1]] = writeData[2];
		in[writeData[1]]--;
	}
	
	if (writeCommand == "ts") {
		registerArray[registerMap["$hi"]] = writeData[2];
		registerArray[registerMap["$lo"]] = writeData[3];
		in[registerMap["$hi"]]--;
		in[registerMap["$lo"]]--;
	}
	
	if (writeCommand == "b") {
		z = writeData[4];
	}
		
	if (writeCommand == "ja") {
		registerArray[registerMap["$ra"]] = writeData[2];
		z = writeData[4];
		in[registerMap["$ra"]]--;
	}
		
	if (writeCommand == "nop") {
	}
	
	if (writeCommand == "ku") {
		killed = true;
		remain = 0;
	}
	
	if (writeCommand == "kn") {
		killed = true;
		remain = writeData[6];
	}
	
}
	

int simulatePointText () {
	memset (in, 0, sizeof (in));
	killed = false;
	remain = 0;
	executionCommand = "nop";
	accessCommand = "nop";
	writeCommand = "nop";
	z = 0;
	while (prog[z][0] != 'm' || prog[z][1] != 'a' || prog[z][2] != 'i' || prog[z][3] != 'n' || prog[z][4] != ':') z++;
	fetch ();
	int uu = 0;
	
	while (!killed) {
		uu++;
		//if (uu > 150) break;
		hazard = false;
		write ();
		access ();
		execution ();
		prepatation ();
		if (hazard) {
			executionCommand = "nop";
			continue;
		}
		if (executionCommand[0] == 'b' || executionCommand[0] == 'j') {
			write ();
			access ();
			write ();
			execution ();
			access ();
			write ();
			executionCommand = "nop";
			accessCommand = "nop";
			writeCommand = "nop";
		}
		fetch ();
	}
	return remain;
}








int main () {
	
	
	//freopen ("test1.in", "r", stdin);
	freopen ("builtin-5140519064-youyurong.s", "r", stdin);
	freopen ("mips.out", "w", stdout);
	
	pre ();
	progLines = 0;
	string s;
	getline (cin, s);
	while (!cin.eof()) {
		prog.push_back (s);
		progLines++;
		getline (cin, s);
	}
	heapTail = 0;
	stackTop = 4 * 1024 * 1024;
	memset (ram, 0, sizeof(ram));
	memset (typ, 0, sizeof(typ));
	
	freopen ("builtin-5140519064-youyurong.in", "r", stdin);
	
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
