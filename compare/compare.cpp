#include <bits/stdc++.h>
using namespace std;

#define NO_OF_CHARS 256

// Preprocessing for the strong good suffix rule
void preprocess_strong_suffix(vector<int>& shift, vector<int>& bpos, const string& pat, int len_pat)
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
void preprocess_case_2_3(vector<int>& shift, vector<int>& bpos, const string& pat, int m)
{
    int j = bpos[0];
    for (int i = 0; i <= m; i++) {
        if (shift[i] == 0)
            shift[i] = j;
        if (i == j)
            j = bpos[j]; 
    }
}

// Boyer-Moore Bad Character Heuristic
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
    preprocess_strong_suffix(shift, bpos, pat, m);
    preprocess_case_2_3(shift, bpos, pat, m);
    int s = 0;
    while (s <= (n - m)) {
        int j = m - 1;
        while (j >= 0 && pat[j] == txt[s + j]) j--;
        if (j < 0) {
            result.push_back(s);  
            s += max(shift[0], (s + m < n) ? m - badchar[txt[s + m]] : 1);
        } else {
            s += max(shift[j + 1], max(1, j - badchar[txt[s + j]]));
        }
    }
    return result;
}

// Function to build the longest prefix suffix (LPS) array
vector<int> buildLPSArray(const string &pattern) {
    int m = pattern.length();
    vector<int> lps(m, 0); // LPS array to hold the longest prefix suffix values
    int length = 0; // length of the previous longest prefix suffix

    int i = 1;
    while (i < m) {
        if (pattern[i] == pattern[length]) {
            length++;
            lps[i] = length;
            i++;
        } else {
            if (length != 0) {
                length = lps[length - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

// KMP algorithm to find all occurrences of the pattern in the text
vector<int> KMPSearch(const string &text, const string &pattern) {
    int n = text.length();
    int m = pattern.length();

    vector<int> lps = buildLPSArray(pattern);
    vector<int> result; // to store the indices where pattern is found

    int i = 0; // index for text
    int j = 0; // index for pattern
    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }

        if (j == m) {
            result.push_back(i - j); // pattern found at index i - j
            j = lps[j - 1];
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }
    return result;
}

// Naive Search Function
vector<int> naiveSearch(const string& txt, const string& pat) {
    int n = txt.size();
    int m = pat.size();
    vector<int> result;

    for (int i = 0; i <= n - m; i++) {
        int j = 0;
        while (j < m && txt[i + j] == pat[j]) {
            j++;
        }
        if (j == m) {
            result.push_back(i);  // Pattern found at index i
        }
    }
    return result;
}

// Function to measure and compare the execution time of all three algorithms
void measureSearchTimes(int K, const string& txt, const string& pat) {
    vector<pair<string, double>> results;

    // Measure average time for each approach
    double naiveTime = 0.0, boyerMooreTime = 0.0, kmpTime = 0.0;

    for (int i = 0; i < K; i++) {
        // Measure time for Naive Search
        auto start = chrono::high_resolution_clock::now();
        int a = naiveSearch(txt, pat).size();
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> naiveDuration = end - start;
        naiveTime += naiveDuration.count();

        // Measure time for Boyer-Moore Search
        start = chrono::high_resolution_clock::now();
        int b = boyerMooreSearch(txt, pat).size();
        end = chrono::high_resolution_clock::now();
        chrono::duration<double> boyerMooreDuration = end - start;
        boyerMooreTime += boyerMooreDuration.count();

        // Measure time for KMP Search
        start = chrono::high_resolution_clock::now();
        int c = KMPSearch(txt, pat).size();
        end = chrono::high_resolution_clock::now();
        chrono::duration<double> kmpDuration = end - start;
        kmpTime += kmpDuration.count();
        if (a != b || b != c) {
            cout << "Error: Results mismatch between Naive, Boyer-Moore, and KMP approaches!" << endl;
        }
    }

    // Calculate mean times
    results.push_back({"Naive Search", naiveTime / K});
    results.push_back({"Boyer-Moore Search", boyerMooreTime / K});
    results.push_back({"KMP Search", kmpTime / K});

    // Sort results by time (ascending)
    sort(results.begin(), results.end(), [](const pair<string, double>& a, const pair<string, double>& b) {
        return a.second < b.second;
    });

    // Display ranking
    cout << "Search Algorithm Ranking (Fastest to Slowest):\n";
    for (const auto& res : results) {
        cout << res.first << " - Average Time: " << res.second << " seconds\n";
    }
}

int main() {
    // Read the DNA sequence from file
    ifstream file("dna_sequence.txt");
    string txt;
    if (file.is_open()) {
        getline(file, txt, '\0');  // Read the whole file into txt
        file.close();
    } else {
        cerr << "Failed to open the file!" << endl;
        return 1;
    }

    string pat = "CACAC";  // The pattern to search for
    int K = 100;  // Number of iterations to measure

    measureSearchTimes(K, txt, pat);

    return 0;
}
