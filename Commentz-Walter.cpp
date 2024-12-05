#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <algorithm>
#include <memory>

const int NOT_SET = -1;

class CWNode {
public:
    char character;
    int depth;
    std::string word;
    CWNode* parent;
    CWNode* ACsuffix_link;
    CWNode* ACoutput_link;
    std::map<char, std::unique_ptr<CWNode>> children;

    int min_suffix_difference;
    int min_output_difference;
    CWNode* CWsuffix_link;
    CWNode* CWoutput_link;
    int suffix_shift;
    int output_shift;

    CWNode(char c, int d, CWNode* p) : 
        character(c), 
        depth(d), 
        word(""), 
        parent(p), 
        ACsuffix_link(nullptr), 
        ACoutput_link(nullptr),
        min_suffix_difference(NOT_SET),
        min_output_difference(NOT_SET),
        CWsuffix_link(nullptr),
        CWoutput_link(nullptr),
        suffix_shift(0),
        output_shift(0) {}
};

class Trie {
protected:
    std::unique_ptr<CWNode> root;

    virtual std::unique_ptr<CWNode> createNode(char character, int depth, CWNode* parent) {
        return std::make_unique<CWNode>(character, depth, parent);
    }

public:
    Trie(void) : root(createNode('\0', 0, nullptr)) {}

    void addWord(const std::string word) {
        CWNode* currentNode = root.get();
        int currentDepth = 1;

        for (char c : word) {
            auto& nextNode = currentNode->children[c];
            if (!nextNode) {
                nextNode = createNode(c, currentDepth, currentNode);
            }
            currentNode = nextNode.get();
            currentDepth++;
        }

        currentNode->word = word;
    }

    bool lookup(const std::string word) {
        CWNode* currentNode = root.get();
        for (char c : word) {
            auto it = currentNode->children.find(c);
            if (it == currentNode->children.end()) return false;    // Not found
            currentNode = it->second.get();
        }
        return true;
    }

    bool isRoot(CWNode* node) {
        return node->character == '\0';
    }

    bool nodeHasChild(CWNode* node, char character) {
        return node->children.find(character) != node->children.end();
    }

    CWNode* getACSuffixLink(CWNode* node) {
        CWNode* searcher = node->parent->ACsuffix_link;
        while (!isRoot(searcher) && !nodeHasChild(searcher, node->character)) {
            searcher = searcher->ACsuffix_link;
        }

        if (nodeHasChild(searcher, node->character)) {
            return searcher->children[node->character].get();
        }

        return searcher;
    }
};

class CWAuto : public Trie {
private:
    int minDepth; // wmin
    std::map<char, int> charLookupTable;

    std::unique_ptr<CWNode> createNode(char character, int depth, CWNode* parent) override {
        return std::make_unique<CWNode>(character, depth, parent);
    }

public:
    CWAuto(void) : minDepth(0) {}

    void addWord(const std::string& word) {
        std::string reversedWord = word;
        std::reverse(reversedWord.begin(), reversedWord.end());
        Trie::addWord(reversedWord);

        int pos = 1;
        for (char c : reversedWord) {
            std::map<char, int>::iterator it = charLookupTable.find(c);
            if (it == charLookupTable.end() || it->second > pos) {
                charLookupTable[c] = pos;
            }
            pos++;
        }

        if (minDepth == 0 || reversedWord.length() < minDepth) {
            minDepth = reversedWord.length();
        }
    }

    void initializeShiftValues() {
        std::queue<CWNode*> bfsQueue;
        root->suffix_shift = 1;
        root->output_shift = minDepth;

        for (auto& child : root->children) {
            bfsQueue.push(child.second.get());
        }

        while (!bfsQueue.empty()) {
            CWNode* currentNode = bfsQueue.front();
            bfsQueue.pop();

            currentNode->suffix_shift = (currentNode->CWsuffix_link == nullptr) ? minDepth : currentNode->min_suffix_difference;
            currentNode->output_shift = (currentNode->CWoutput_link == nullptr) ? 
                (currentNode->parent)->output_shift : 
                currentNode->min_output_difference;

            for (auto& child : currentNode->children) {
                bfsQueue.push(child.second.get());
            }
        }
    }

    void createFailureLinks() {
        std::queue<CWNode*> bfsQueue;

        for (auto& child : root->children) {
            CWNode* childNode = child.second.get();
            childNode->ACsuffix_link = root.get();

            for (auto& grandchild : childNode->children) {
                bfsQueue.push(grandchild.second.get());
            }
        }

        while (!bfsQueue.empty()) {
            CWNode* currentNode = bfsQueue.front();
            bfsQueue.pop();

            for (auto& child : currentNode->children) {
                bfsQueue.push(child.second.get());
            }

            CWNode* acSuffixNode = getACSuffixLink(currentNode);
            currentNode->ACsuffix_link = acSuffixNode;

            bool suffixIsWord = !acSuffixNode->word.empty();
            currentNode->ACoutput_link = suffixIsWord ? 
                acSuffixNode : 
                (acSuffixNode->ACoutput_link ? acSuffixNode->ACoutput_link : nullptr);

            bool isSet2 = !currentNode->word.empty();
            CWNode* acCWNode = (acSuffixNode);

            if (acCWNode->min_suffix_difference == -1 || 
                acCWNode->min_suffix_difference > currentNode->depth - acCWNode->depth) {
                acCWNode->min_suffix_difference = currentNode->depth - acCWNode->depth;
                acCWNode->CWsuffix_link = currentNode;
            }

            if (isSet2) {
                if (acCWNode->min_output_difference == -1 || 
                    acCWNode->min_output_difference > currentNode->depth - acCWNode->depth) {
                    acCWNode->min_output_difference = currentNode->depth - acCWNode->depth;
                    acCWNode->CWoutput_link = currentNode;
                }
            }
        }

        initializeShiftValues();
    }

    int charFunc(char character) {
        std::map<char, int>::iterator it = charLookupTable.find(character);
        return (it != charLookupTable.end()) ? it->second : (minDepth + 1);
    }

    int shiftFunc(CWNode* node, int j) {
        int maxOfS1AndChar = (node->character == '\0') ? 
            node->suffix_shift : 
            std::max(charFunc(node->character) - j - 1, node->suffix_shift);

        return std::min(maxOfS1AndChar, node->output_shift);
    }

    std::vector<std::pair<std::string, int>> reportAllMatches(const std::string& text) {
        int i = minDepth - 1;
        std::vector<std::pair<std::string, int>> matches;

        while (i < text.length()) {
            CWNode* v = root.get();
            int j = 0;
            char charToFind = text[i - j];

            while (nodeHasChild(v, charToFind)) {
                v = (v->children[charToFind].get());
                j++;

                if (!v->word.empty()) {
                    std::string originalWord = v->word;
                    std::reverse(originalWord.begin(), originalWord.end());
                    matches.push_back({originalWord, i - j + 1});
                }

                if (i < j)
                    break;
                charToFind = text[i - j]; 
            }

            if (j > i) j = i;
            i += shiftFunc(v, j);
        }

        return matches;
    }
};

int main() {
    CWAuto automaton;
    
    // Add dictionary
    std::fstream dictionary("dictionary.txt");
    for (std::string keyword; std::getline(dictionary, keyword); automaton.addWord(keyword));
    dictionary.close();

    // Create failure links
    automaton.createFailureLinks();
    
    // Match against text
    std::fstream document("document.txt");
    std::string text((std::istreambuf_iterator<char>(document)), std::istreambuf_iterator<char>());
    document.close();
    std::vector<std::pair<std::string, int>> matches = automaton.reportAllMatches(text);

    // Print result
    std::ofstream output("output.txt");
    output << "Total matches: " << matches.size() << std::endl;
    for(std::pair<std::string, int> match : matches) {
        output << "Match: " << match.first << " at position " << match.second << "\n";
    }
    output.close();
    return 0;
}
