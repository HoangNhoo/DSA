#include <bits/stdc++.h>
using namespace std;

#define NO_OF_CHARS 256
void preprocess_good_suffix_case_1(vector<int>& shift, vector<int>& bpos, const string& pat, int len_pat)
{
    int id_current_prefix = len_pat, id_current_suffix = len_pat + 1;
    bpos[id_current_prefix] = id_current_suffix;

    while (id_current_prefix > 0) {
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
void preprocess_good_suffix_case_2_3(vector<int>& shift, vector<int>& bpos, const string& pat, int m)
{
    int j = bpos[0];
    for (int i = 0; i <= m; i++) {
        if (shift[i] == 0)
            shift[i] = j;
        if (i == j)
            j = bpos[j]; 
    }
}


void badCharHeuristic(string str, int size, int badchar[NO_OF_CHARS])
{
    int i;
 
    for (i = 0; i < NO_OF_CHARS; i++)
        badchar[i] = -1;
 
    for (i = 0; i < size; i++)
        badchar[(int)str[i]] = i;
}


// Boyer-Moore Search Algorithm
vector<int> boyerMooreSearch(string txt, string pat) {
    int m = pat.size();
    int n = txt.size();
    int badchar[NO_OF_CHARS];
    vector<int> bpos(m + 1), shift(m + 1, 0);
	vector<int> result;

    badCharHeuristic(pat, m, badchar);

	preprocess_good_suffix_case_1(shift, bpos, pat, m);
    preprocess_good_suffix_case_2_3(shift, bpos, pat, m);
    int s = 0;
    while (s <= (n - m)) {
        int j = m - 1;
        while (j >= 0 && pat[j] == txt[s + j]) j--;
        if (j < 0) {
            result.push_back(s);  // Pattern found at index s
            s += max(shift[0], (s + m < n) ? m - badchar[txt[s + m]] : 1);
        } else {
            s += max(shift[j + 1],max(1, j - badchar[txt[s + j]]));
        }
    }

    return result;
}
int main() {
    int option; 
    cout << "Boyer-Moore Search Algorithm" << endl;
    cout << "Please README.md for a clear guide on how to use this program" << endl;
    cout << "Enter 1 to enter your own text and pattern" << endl;
    cout << "Enter 2 to read text and pattern from a file" << endl;
    cout << "Enter your choice: ";
    cin >> option;

    string txt;
    string pat;
    if (option == 1) {
        cout << "Enter text: ";
        cin >> txt;
        cout << "Enter pattern: ";
        cin >> pat;
    } else if (option == 2) {
        string filename;
        cout << "Enter file name (input.txt): ";
        cin >> filename;

        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error opening file!" << endl;
            return 1;
        }
        getline(file, txt);
        getline(file, pat);
        file.close();

        cout << "Text from file: " << txt << endl;
        cout << "Pattern from file: " << pat << endl;
    } else {
        cout << "Invalid choice!" << endl;
        return 1;
    }
    vector<int> bmResult = boyerMooreSearch(txt, pat);
    if (bmResult.empty()) {
        cout << "Pattern not found in text using Boyer-Moore!" << endl;
    } else {
        cout << "Boyer-Moore Result: ";
        for (int idx : bmResult) {
            cout << idx << " ";
        }
        cout << endl;
    }

    return 0;
}