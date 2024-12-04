#include <bits/stdc++.h>
using namespace std;

#define NO_OF_CHARS 256
// Naive Pattern Matching Algorithm
vector<int> naiveSearch(string txt, string pat) {
    int n = txt.size();
    int m = pat.size();
    vector<int> result;
    
    // Traverse the text and compare with the pattern
    for (int i = 0; i <= n - m; i++) {
        int j = 0;
        while (j < m && txt[i + j] == pat[j])
            j++;
        if (j == m) {
            result.push_back(i);  // Pattern found at index i
        }
    }
    return result;
}

// Preprocessing for the strong good suffix rule
void preprocess_strong_suffix(vector<int>& shift, vector<int>& bpos, const string& pat, int len_pat)
{
    int id_current_prefix = len_pat, id_current_suffix = len_pat + 1;
    bpos[id_current_prefix] = id_current_suffix;

    while (id_current_prefix > 0) {
        // If characters at positions i-1 and j-1 are different, continue searching to the right
        while (id_current_suffix <= len_pat && pat[id_current_prefix - 1] != pat[id_current_suffix - 1]) {
            if (shift[id_current_suffix] == 0) {
                shift[id_current_suffix] = id_current_suffix - id_current_prefix; 
            }
            id_current_suffix = bpos[id_current_suffix];
        }
        id_current_prefix--;
        id_current_suffix--;
        bpos[id_current_prefix] = id_current_suffix; 
    }
}

// Preprocessing for case 2 and case 3
void preprocess_case_2_3(vector<int>& shift, vector<int>& bpos, const string& pat, int m)
{
    int j = bpos[0];
    for (int i = 0; i <= m; i++) {
        // If shift[i] is 0, we need to apply case 2 or case 3
        if (shift[i] == 0)
            shift[i] = j;
        if (i == j)
            j = bpos[j]; // If i surpasses j, we find the next bounding box
    }
}


// Boyer-Moore Bad Character Heuristic
void badCharHeuristic(string str, int size, int badchar[NO_OF_CHARS]) {
    int i;
    // Initializing all occurrences as -1
    for (i = 0; i < NO_OF_CHARS; i++)
        badchar[i] = size;

    // Fill the actual value of last occurrence of a character
    for (i = 0; i < size; i++)
        badchar[(int)str[i]] = size - i - 1;
}

// Boyer-Moore Search Algorithm
vector<int> boyerMooreSearch(string txt, string pat) {
    int m = pat.size();
    int n = txt.size();
    int badchar[NO_OF_CHARS];
    vector<int> bpos(m + 1), shift(m + 1, 0);
	vector<int> result;

    // Fill the bad character array by calling the preprocessing function
    badCharHeuristic(pat, m, badchar);
	// Fill the good suffix array
	preprocess_strong_suffix(shift, bpos, pat, m);
    preprocess_case_2_3(shift, bpos, pat, m);
    int s = 0;
    while (s <= (n - m)) {
        int j = m - 1;
        while (j >= 0 && pat[j] == txt[s + j]) j--;
        if (j < 0) {
            result.push_back(s);  // Pattern found at index s
            s += max(shift[0],(s + m < n) ? max(1, badchar[txt[s + m]]) : 1);
        } else {
            s += max(shift[j + 1],max(1, badchar[txt[s + j]]));
        }
    }

    return result;
}

// Generate random text and pattern
void generateRandomTest(string &txt, string &pat, int n, int m) {
    string chars = "ABCDEFG";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, chars.size() - 1);

    // Generate random text of length n
    txt.clear();
    for (int i = 0; i < n; i++) {
        txt += chars[dis(gen)];
    }

    // Generate random pattern of length m
    pat.clear();
    for (int i = 0; i < m; i++) {
        pat += chars[dis(gen)];
    }
}

int main() {
	while(true){
		// Set random test parameters
		int n = 1000;  // Length of text
		int m = 20;     // Length of pattern

		// Generate random test case
		string txt, pat;
		generateRandomTest(txt, pat, n, m);
		cout <<pat << endl;
		// Perform search using both approaches
		vector<int> naiveResult = naiveSearch(txt, pat);
		vector<int> bmResult = boyerMooreSearch(txt, pat);
		// Check if the results match
		if (naiveResult != bmResult) {
			cout << "Error: Results mismatch between Naive and Boyer-Moore approaches!" << endl;
			cout << "Naive Result: ";
			for (int idx : naiveResult) {
				cout << idx << " ";
			}
			cout << endl;
			cout << "Boyer-Moore Result: ";
			for (int idx : bmResult) {
				cout << idx << " ";
			}
			cout << endl;
			return 1;
		} 
	}

    return 0;
}
