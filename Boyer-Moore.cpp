#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

vector<int> badCharacterHeuristic(const string &pattern) {
    // tim vi tri xuat hien cuoi cung cua cac ki tu trong pattern
    const int ALPHABET_SIZE = 256;
    vector<int> badChar(ALPHABET_SIZE, -1);

    for (int i = 0; i < pattern.length(); i++) {
        badChar[pattern[i]] = i;
    }
    
    return badChar;
}

vector<int> goodSuffixHeuristic(const string &pattern) {
    int m = pattern.length();
    vector<int> suffix(m, -1);
    vector<int> borderPos(m + 1, 0);

    // Tim cac suffix cua pattern
    for (int i = m - 1; i >= 0; i--) {
        int j = i;
        while (j >= 0 && pattern[j] == pattern[m - 1 - (i - j)]) {
            j--;
        }
        suffix[i] = j + 1; 
    }

    for (int i = 0; i < m; i++) {
        cout << suffix[i] << " ";
        borderPos[suffix[i]] = i;
    }

    return borderPos;
}

// Hàm tìm kiếm Boyer-Moore
void boyerMooreSearch(const string &text, const string &pattern) {
    int n = text.length();
    int m = pattern.length();

    if (m > n) {
        cout << "Pattern is longer than text." << endl;
        return;
    }

    vector<int> badChar = badCharacterHeuristic(pattern);
    vector<int> goodSuffix = goodSuffixHeuristic(pattern);

    int s = 0;
    while (s <= (n - m)) {
        int j = m - 1;

        while (j >= 0 && pattern[j] == text[s + j]) {
            j--;
        }

        if (j < 0) {
            cout << "Pattern found at index " << s << endl;
            s += (s + m < n) ? m - goodSuffix[0] : 1;
        } else {
            s += max(1, j - badChar[text[s + j]]);
        }
    }
}

int main() {
    string text = "ABAAABCD";
    string pattern = "ABC";
    cout << "Text: " << text << endl;
    cout << "Pattern: " << pattern << endl;
    boyerMooreSearch(text, pattern);

    return 0;
}
