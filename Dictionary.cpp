#include "DictionaryTree.h"

const vector<char> Dictionary::ALPHABET({ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '\'' });

Dictionary::~Dictionary() {
    for (int i = 0; i < root.size(); i++) {
        delete root[i];
    }
}

void Dictionary::build(string file) {
    ifstream myfile;
    string word;
    unsigned int size;
    bool flag;
    myfile.open(file, ios::in);
    if (myfile.is_open()) {
        while (getline(myfile, word)) {
            //convert to lowercase
            transform(word.begin(), word.end(), word.begin(), ::tolower);

            //make sure word can be represented with Alphabet
            flag = false;
            for (char c : word) {
                //test for good letter
                if (inAlphabet(c) == -1) {
                    //letter does not exist
                    flag = true;
                    cout << "Error the charater " << c << " does not exist the the alphabet " << endl;
                    break;
                }
            }
            if (!flag) {
                size = word.size();
                if (size >= 0 && size < root.size()) {
                    root[size]->build(word);
                } else {
                    while (root.size() <= size) {
                        root.push_back(new LetterNode);
                    }
                    if (size >= 0) {
                        root[size]->build(word);
                    } else {
                        cout << "Did not add " << word << endl;
                    }
                }
            } else {
            cout << "Did not add " << word << endl;
            }
        }
        myfile.close();
    } else {
    cout << "File did not open: " << file << endl;
    }
}

bool const Dictionary::ifExists(string word) {
    transform(word.begin(), word.end(), word.begin(), ::tolower);
    unsigned int size = word.size();

    if (size >= 0 && size < root.size()) {
        return root[size]->ifExists(word);
    } else {
        return false;
    }
}

int Dictionary::inAlphabet(char c) {
    c = tolower(c);
    vector<char>::const_iterator f = find(Dictionary::ALPHABET.begin(), Dictionary::ALPHABET.end(), c);
    if (f != Dictionary::ALPHABET.end()) {
        //letter found in Alphabet
        return distance(Dictionary::ALPHABET.begin(), f);
    }
    return -1;
}

vector<list<int>> Dictionary::possibleLetters(vector<CryptogramLetter*> word, vector<list<int>> *possibleLetters) const {
    bool deadEnd = true;
    //remove punctuation
    for (unsigned int i = 0; i < word.size(); i++) {
        if (word[i]->status == PUNCTUATION) {
            if (word.size() - 1 == i) {
                word.pop_back();
            } else {
                cout << "ERROR: Can't handle punctuation " << word[i]->punctuation << endl;
            }
        }
    }

    vector<list<int>> l(word.size());
    if (word.size() >= 0 && word.size() < root.size()) {
        deadEnd = root[word.size()]->possibleLetters(word, possibleLetters, 0, &l);
    }

    return l;
}

//***************************************************************************************************************************************************************************************************************************
//****************************************************************************************************LetterNode*************************************************************************************************************
//***************************************************************************************************************************************************************************************************************************

LetterNode::LetterNode() {
    endOfWord = false;
    for (unsigned int i = 0; i < Dictionary::ALPHABET.size(); i++) {
        letters.push_back(NULL);
    }
}

LetterNode::~LetterNode() {
    for (unsigned int i = 0; i < Dictionary::ALPHABET.size(); i++) {
        delete letters[i];
    }
}

void LetterNode::build(string word) {
    if (word.length() == 0) {
        endOfWord = true;
        return;
    }

    //test for good letter
    int ndx = Dictionary::inAlphabet(word[0]);
    if (ndx != -1) {
        //letter found in Alphabet
        if (letters[ndx] == NULL) {
            letters[ndx] = new LetterNode();
        }
        letters[ndx]->build(word.substr(1));
    } else {
        cout << "Error the charater " << word[0] << " does not exist the the alphabet " << endl;
        return;
    }

}

void LetterNode::printDictionary(ostream &os, string l) const {
    if (endOfWord) {
        cout << l << endl;
    }
    for (unsigned int i = 0; i < letters.size(); i++) {
        if (letters[i] != NULL) {
            letters[i]->printDictionary(os, l + Dictionary::ALPHABET[i]);
        }
    }
}

bool const LetterNode::ifExists(string word) {
    if (word.length() == 0) {
        if (endOfWord) {
            return true;
        } else {
            return false;
        }
    }
    char l = word[0];

    //test for good letter
    int ndx = Dictionary::inAlphabet(word[0]);
    if (ndx != -1) {
        //letter found in Alphabet
        if (letters[ndx] == NULL) {
            letters[ndx] = new LetterNode();
        }
        return letters[ndx]->ifExists(word.substr(1));
    } else {
        return false;
    }
}

bool LetterNode::possibleLetters(vector<CryptogramLetter *> word, vector<list<int>> *possibleLetters, unsigned int iteration, vector<list<int>> *l) const {
    //exit clause
    if (iteration == word.size()) {
        return false;
    }

    bool flag;
    bool deadEnd = true;
    bool r;

    if (word[iteration]->status == UNKNOWN) {
        //cycle through letters
        for (list<int>::const_iterator i = (*possibleLetters)[word[iteration]->encryptedLetter].begin(); i != (*possibleLetters)[word[iteration]->encryptedLetter].end(); i++) {
            if (letters[*i] != NULL) {
                if (iteration < word.size()) {
                    r = letters[*i]->possibleLetters(word, possibleLetters, iteration + 1, l);
                    if (deadEnd) {
                        deadEnd = r;
                    }
                    if (!r) {
                        //add letter
                        flag = false;
                        for (int j : (*l)[iteration]) {
                            if (j == *i) {
                                flag = true;
                                break;
                            }
                        }
                        if (!flag) {
                            (*l)[iteration].push_back(*i);
                        }
                    }
                }
            }
        }
    } else if (word[iteration]->status == KNOWN || word[iteration]->status == GUESS) {

        if (letters[word[iteration]->uncryptedLetter] == NULL) {
            return true;
        }

        r = letters[word[iteration]->uncryptedLetter]->possibleLetters(word, possibleLetters, iteration + 1, l);
        if (deadEnd) {
            deadEnd = r;
        }

        if (!r) {
            //add letter
            flag = false;
            for (int j : (*l)[iteration]) {
                if (j == word[iteration]->uncryptedLetter) {
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                (*l)[iteration].push_back(word[iteration]->uncryptedLetter);
            }
        }
        
    }

    return deadEnd;
}