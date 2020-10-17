#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
using namespace std;
struct StateInfo
{
	string nextState;
	string input;
	string output;
	bool operator<(const StateInfo& a) const
	{
		return input < a.input;
	}
	bool operator==(const StateInfo& a) const
	{
		return input == a.input;
	}
	bool operator!=(const StateInfo& a) const
	{
		return input != a.input;
	}
};
void loadFile(fstream& inFile, string& reset, int& input, int& output, vector< vector<StateInfo> >& states, map<string, int>& index);
void saveFile(fstream& outFile, string& reset, int& input, int& output, vector< vector<StateInfo> >& states, map<string, int>& index);
int turnToInt(char buffer[]);
void copy(const StateInfo a, StateInfo& b);
void extension(vector< vector<StateInfo> >& states);
void sortAll(vector< vector<StateInfo> >& states);
void replace(string& reset, vector<vector<StateInfo>>& states, string state, string replacedState);
bool equal(const StateInfo a, const StateInfo b);
void reduction(string& reset, vector< vector<StateInfo> >& states, map<string, int>& index);
void test(int& oriInput, int& redInput, int& oriOutput, int& redOutput, string& oriReset, string& redReset,
	vector< vector<StateInfo> >& oristates, vector< vector<StateInfo> >& redstates,
	map<string, int>& oriIndex, map<string, int>& redIndex, int seed);
bool testEqual(const string original, const string reduction);
int main(int argc, char* argv[])
{
	fstream readFile(argv[1],ios::in);
	fstream writeFile("answer.KISS", ios::out);

	int input = 0, output = 0;
	string reset;
	map<string, int> index;
	vector< vector<StateInfo> > states;

	loadFile(readFile, reset, input, output, states, index);

	extension(states);

	sortAll(states);

	reduction(reset, states, index);

	saveFile(writeFile, reset, input, output, states, index);
}
void loadFile(fstream& inFile, string& reset, int& input, int& output, vector< vector<StateInfo> >& states, map<string, int>& index)
{
	if (!inFile) {
		cout << "File could not be opened" << endl;
		return;
	}
	char buffer[30];
	int state = 0, number = 0, newIndex = 0;
	string stateName;
	while (inFile.getline(buffer, 30)) {
		if (buffer[0] == '#')
			continue;
		if (buffer[1] == 'i') {
			input = turnToInt(buffer);
			continue;
		}
		if (buffer[1] == 'o') {
			output = turnToInt(buffer);
			continue;
		}
		if (buffer[1] == 'p') {
			number = turnToInt(buffer);
			continue;
		}
		if (buffer[1] == 's') {
			state = turnToInt(buffer);
			continue;
		}
		if (buffer[1] == 'r') {
			for (int i = 3; buffer[i] != '\0'; i++)
				reset += buffer[i];
			break;
		}
	}
	states.resize(state);
	map<string, int>::iterator it;
	for (int i = 0; i < number; i++) {
		inFile.getline(buffer, 30);
		StateInfo stateInfo;
		int next, state, j;
		stateInfo.input.assign(buffer, 0, input);
		next = input + 1;
		for (; buffer[next] != ' '; next++)
			stateName += buffer[next];
		next++;
		it = index.find(stateName);
		if (it == index.end())
			index[stateName] = newIndex++;
		state = index[stateName];
		stateName.clear();
		for (; buffer[next] != ' '; next++)
			stateName += buffer[next];
		next++;
		stateInfo.nextState = stateName;
		stateName.clear();
		stateInfo.output.assign(buffer, next, output);
		states[state].push_back(stateInfo);
	}
	inFile.close();
}
void saveFile(fstream& outFile, string& reset, int& input, int& output, vector<vector<StateInfo>>& states, map<string, int>& index)
{
	int cnt;
	map<string, int>::iterator it;
	outFile << ".i " << input << endl;
	outFile << ".o " << output << endl;
	for (cnt = 0, it = index.begin(); it != index.end(); ++it)
		if (!states[(*it).second].empty())
			cnt++;
	outFile << ".s " << cnt << endl;
	cnt = 0;
	for (int i = 0; i < states.size(); i++)
		for (int j = 0; j < states[i].size(); j++)
			cnt++;
	outFile << ".p " << cnt << endl;
	outFile << ".r " << reset << endl;
	for (it = index.begin(); it != index.end(); ++it) {
		for (int j = 0; j < states[(*it).second].size(); j++) {
			outFile << states[(*it).second][j].input << " "
				<< (*it).first << " "
				<< states[(*it).second][j].nextState << " "
				<< states[(*it).second][j].output << endl;
		}
	}
	outFile << ".e" << endl;
	outFile.close();
}
int turnToInt(char buffer[])
{
	int toIntPos = 0;
	char toInt[10];
	for (int i = 0; buffer[i] != '\0'; i++) {
		if (buffer[i] >= '0' && buffer[i] <= '9') {
			toInt[toIntPos++] = buffer[i];
			if (!(buffer[i + 1] >= '0' && buffer[i + 1] <= '9'))
				break;
		}
	}
	toInt[toIntPos] = '\0';
	return (int)atoi(toInt);
}
void copy(const StateInfo a, StateInfo& b)
{
	b.nextState = a.nextState;
	b.input = a.input;
	b.output = a.output;
}
void extension(vector<vector<StateInfo>>& states)
{
	StateInfo buffer;
	for (int i = 0; i < states.size(); i++) {
		for (int j = 0; j < states[i].size(); j++) {
			for (int k = 0; states[i][j].input[k] != '\0'; k++) {
				if (states[i][j].input[k] == '-') {
					states[i][j].input[k] = '0';
					copy(states[i][j], buffer);
					buffer.input[k] = '1';
					states[i].push_back(buffer);
				}
			}
		}
	}
}
void sortAll(vector<vector<StateInfo>>& states)
{
	for (int i = 0; i < states.size(); i++)
		sort(states[i].begin(), states[i].end());
}
void replace(string& reset, vector<vector<StateInfo>>& states, string state, string replacedState)
{
	if (reset == replacedState)
		reset = state;
	for (int i = 0; i < states.size(); i++)
		for (int j = 0; j < states[i].size(); j++)
			if (states[i][j].nextState == replacedState)
				states[i][j].nextState = state;
}
bool equal(const StateInfo a, const StateInfo b)
{
	return a.nextState == b.nextState && a.input == b.input && a.output == b.output;
}
void reduction(string& reset, vector<vector<StateInfo>>& states, map<string, int>& index)
{
	int i = 0, j = 1;
	map<string, int>::iterator it1, it2;
	while (i != states.size()) {
		while (j != states.size()) {
			if (states[i].size() == states[j].size() && states[i].size()) {
				for (int k = 0; k < states[i].size(); k++) {
					if (equal(states[i][k], states[j][k]) && k == states[i].size() - 1) {
						for (it1 = it2 = index.begin(); it1 != index.end() && it2 != index.end();) {
							if ((*it1).second != i)
								++it1;
							if ((*it2).second != j)
								++it2;
							if ((*it1).second == i && (*it2).second == j)
								break;
						}
						replace(reset, states, (*it1).first, (*it2).first);
						states[j].clear();
						i = 0; j = 1;
					}
					else if (equal(states[i][k], states[j][k]));
					else {
						j++;
						break;
					}
				}
			}
			else {
				j++;
			}
		}
		i++; j = i + 1;
	}
	i = 0; j = 1;
	while (i != states.size()) {
		while (j != states.size()) {
			int k = 0, l = 0;
			while (states[i].size() && states[j].size()) {
				if (states[i][k] != states[j][l]) {
					if (states[i][k] < states[j][l])
						k++;
					else
						l++;
					if (k == states[i].size() || l == states[j].size()) {
						for (int n = 0; n < states[j].size(); n++)
							states[i].push_back(states[j][n]);
						for (it1 = it2 = index.begin(); it1 != index.end() && it2 != index.end();) {
							if ((*it1).second != i)
								++it1;
							if ((*it2).second != j)
								++it2;
							if ((*it1).second == i && (*it2).second == j)
								break;
						}
						replace(reset, states, (*it1).first, (*it2).first);
						states[j].clear();
						break;
					}
				}
				else {
					break;
				}
			}
			j++;
		}
		i++; j = i + 1;
	}
}
void test(int& oriInput, int& redInput, int& oriOutput, int& redOutput, string& oriReset, string& redReset,
	vector< vector<StateInfo> >& oristates, vector< vector<StateInfo> >& redstates,
	map<string, int>& oriIndex, map<string, int>& redIndex, int seed)
{
	srand(seed);
	int r;
	string oriState, redState;
	if (oriReset == "" || redReset == "") {
		cout << "Error Reset" << endl;
		return;
	}
	oriState = oriReset; redState = redReset;
	if (oriInput != redInput) {
		cout << "Error Input" << endl;
		return;
	}
	if (oriOutput != redOutput) {
		cout << "Error Output" << endl;
		return;
	}
	for (int i = 0; i < 1000000; i++) {
		r = rand() % oristates[oriIndex[oriState]].size();
		for (int j = 0; j < redstates[redIndex[redState]].size(); j++) {
			if (testEqual(oristates[oriIndex[oriState]][r].input, redstates[redIndex[redState]][j].input)) {
				if (oristates[oriIndex[oriState]][r].output == redstates[redIndex[redState]][j].output) {
					oriState = oristates[oriIndex[oriState]][r].nextState;
					redState = redstates[redIndex[redState]][j].nextState;
					break;
				}
				else {
					cout << "Error Output Not Equal" << endl;
					cout << "Original" << endl;
					cout << "State : " << oriState << endl;
					cout << "Input : " << oristates[oriIndex[oriState]][r].input << endl;
					cout << "Reduction" << endl;
					cout << "State : " << redState << endl;
					cout << "Input : " << redstates[redIndex[redState]][r].input << endl;
					return;
				}
			}
			else if (!testEqual(oristates[oriIndex[oriState]][r].input, redstates[redIndex[redState]][j].input)
				&& j == redstates[redIndex[redState]].size() - 1) {
				cout << "Error No Input" << endl;
				cout << "Original" << endl;
				cout << "State : " << oriState << endl;
				cout << "Input : " << oristates[oriIndex[oriState]][r].input << endl;
				cout << "Reduction" << endl;
				cout << "State : " << redState << endl;
				cout << "Input : " << redstates[redIndex[redState]][r].input << endl;
				return;
			}
		}
	}
}
bool testEqual(const string original, const string reduction)
{
	if(original.length()==reduction.length())
		for (int i = 0; i < original.length(); i++) {
			if (original[i] == reduction[i] && i == original.size() - 1)
				return true;
			else if (original[i] != reduction[i] && original[i] == '-' && i == original.size() - 1)
				return true;
			else if (original[i] == reduction[i]);
			else if (original[i] != reduction[i] && original[i] == '-');
			else
				return false;
		}
	return false;
}