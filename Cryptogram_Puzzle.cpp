#include "CryptogramPuzzle.h"

using namespace std;

string CryptogramPuzzle::encryptPhrase(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);

    srand((unsigned int)time(NULL));

    vector<char> key = Dictionary::ALPHABET;
    for (unsigned int i = 0; i < key.size() * 50; i++) {
        swap(key[rand()%key.size()], key[rand()%key.size()]);
    }
    int ndx;
    for (unsigned int i = 0; i < s.size(); i++) {
        ndx = Dictionary::inAlphabet(s[i]);
        if (ndx != -1) {
            //letter found in Alphabet
            s[i] = key[ndx];
        }
    }

    return s;
}

string CryptogramPuzzle::encryptPhrase(string s, Dictionary *d) {
    //check to see if words in string is in dictionary
    string word = "";
    string newS = "";

    for (char c : s) {
        if (Dictionary::inAlphabet(c) != -1) {
            word += c;
        } else if (c == DELIM) {
            if (d->ifExists(word)) {
                newS += word + DELIM;
                word = "";
            } else {
                cout << "Invalid word: " << word << endl;
                return "";
            }
        }
    }

    if (d->ifExists(word)) {
        newS += word + DELIM;
    } else {
        cout << "Invalid word: " << word << endl;
        return "";
    }

    //call normal createPuzzle
    return encryptPhrase(s);
}

string CryptogramPuzzle::printPuzzle(vector<vector<CryptogramLetter*>> *puzzle) {
    string s;
    for (vector<vector<CryptogramLetter*>>::const_iterator i = (*puzzle).begin(); i != (*puzzle).end(); i++) {
        for (vector<CryptogramLetter*>::const_iterator j = i->begin(); j != i->end(); j++) {
            if ((*j)->status == UNKNOWN) {
                s += "_";
            } else if ((*j)->status == PUNCTUATION) {
                s += (*j)->punctuation;
            } else {
                s += Dictionary::ALPHABET[(*j)->uncryptedLetter];
            }
        }
        s += " ";
    }
    return s;
}

string CryptogramPuzzle::solvePuzzle(string s, Dictionary *dictionary) {
    //Build Puzzle
    vector<vector<vector<CryptogramLetter*>>> puzzle;   //Iteration, Phrase, Word, Letter Info
    vector<vector<CryptogramLetter*>> base;   // Phrase, Word, Letter Info:  this is push onto puzzle 
    
    vector<list<int>> PLreturn;    //char, list of possible letters
    vector<vector<list<int>>> possibleLetters;  //iteration, alphabet, list of possible letters

    vector<pair<int, list<int>::const_iterator>> guessNDX;   //iteration, (guess letter, iterator to possible letter;
    vector<pair<int, int>> guessOrder; //most used letters to least, (count, letter);

    //build Possible Letters Base;
    possibleLetters.push_back(vector<list<int>>(Dictionary::ALPHABET.size()));
    for (vector<list<int>>::iterator i = possibleLetters.back().begin(); i != possibleLetters.back().end(); i++) {
        for (unsigned int j = 0; j < Dictionary::ALPHABET.size(); j++) {
            i->push_back(j);
        }
    }

    string trash;

    bool flag;
    bool possibleLettersChange;
    bool invalidKey = false;
    char uncrypt;
    char crypt;

    char uncryptNDX;
    char cryptNDX;

    int ndx;

    list<int> replacement;
    list<int> knownLetters;

    buildPuzzle(&base, s);
    puzzle.push_back(base);

    //set up guess order
    //Add to vector
    for (vector<vector<CryptogramLetter*>>::const_iterator i = puzzle.back().begin(); i != puzzle.back().end(); i++) {
        for (vector<CryptogramLetter*>::const_iterator j = i->begin(); j != i->end(); j++) {
            flag = true;
            for (vector<pair<int, int>>::iterator k = guessOrder.begin(); k != guessOrder.end(); k++) {
                if ((*j)->encryptedLetter == k->second) {
                    k->first++;
                    flag = false;
                }
            }
            if (flag) {
                guessOrder.push_back(pair<int, int>(1, (*j)->encryptedLetter));
            }
        }
    }
    //Sort Greatest to least
    sort(guessOrder.begin(), guessOrder.end());
    reverse(guessOrder.begin(), guessOrder.end());

    
    //solve puzzle
    while (!isSolved(&puzzle.back(), dictionary)) {
        cout << "Puzzle: " << endl;
        cout << "Depth: " << puzzle.size() << endl;
        cout << printPuzzle(&puzzle.back()) << endl;

        //input type
        if (true) {
            //auto
            if (invalidKey) {
                //update guess ndx
                guessNDX.back().second++;
                while (guessNDX.back().second == possibleLetters.back()[guessNDX.back().first].end()) {
                    guessNDX.pop_back();
                    guessNDX.back().second++;
                }

                if (guessNDX.size() == 0) {
                    return "Not Solveable";
                }
            } else {
                //guess new letter
                ndx = 0;
                for (pair<int, int> i : guessOrder) {
                    if (i.second != -1 && possibleLetters.back()[i.second].size() > 1) {
                        ndx = i.second;
                        break;
                    }
                }
                guessNDX.push_back(pair<int, list<int>::const_iterator>(ndx, possibleLetters.back()[ndx].begin()));
            }
            cryptNDX = guessNDX.back().first;
            uncryptNDX = *guessNDX.back().second;

            cout << endl << "Guess Deapth:" << guessNDX.size() << " " << Dictionary::ALPHABET[cryptNDX] << " -> " << Dictionary::ALPHABET[uncryptNDX] << endl;
        } else {
            //manual
            cout << endl << "Enter crypt letter to uncrypt letter: ";
            cin >> crypt >> uncrypt;

            cryptNDX = Dictionary::inAlphabet(crypt);
            uncryptNDX = Dictionary::inAlphabet(uncrypt);
        }

        possibleLetters.push_back(possibleLetters.back());

        possibleLetters.back()[cryptNDX].clear();
        possibleLetters.back()[cryptNDX].push_back(uncryptNDX);

        puzzle.push_back(copyPuzzleLevel(&puzzle.back()));
        replaceLetters(&puzzle.back(), &possibleLetters.back());

        do {
            invalidKey = false;
            possibleLettersChange = false;
            
            for (vector<vector<CryptogramLetter *>>::iterator i = (puzzle.end() - 1)->begin(); i != (puzzle.end() - 1)->end(); i++) {
                PLreturn = dictionary->possibleLetters(*i , &possibleLetters.back());

                //Add to list
                for (unsigned int j = 0; j < PLreturn.size(); j++) {
                    //if letter has 0 possible add
                    if (possibleLetters.back()[(*i)[j]->encryptedLetter].size() == 0) {
                        for (int k : PLreturn[j]) {
                            possibleLetters.back()[(*i)[j]->encryptedLetter].push_back(k);
                        }
                    } else {
                        //merge
                        replacement.clear();
                        for (list<int>::iterator k = possibleLetters.back()[(*i)[j]->encryptedLetter].begin(); k != possibleLetters.back()[(*i)[j]->encryptedLetter].end(); k++) {
                            flag = false;
                            for (list<int>::iterator l = PLreturn[j].begin(); l != PLreturn[j].end(); l++) {
                                if (*k == *l) {
                                    replacement.push_back(*k);
                                    break;
                                }
                            }
                        }
                        //replace
                        if (possibleLetters.back()[(*i)[j]->encryptedLetter].size() != replacement.size()) {
                            possibleLettersChange = true;
                        }
                        possibleLetters.back()[(*i)[j]->encryptedLetter] = replacement;

                        if (possibleLetters.back()[(*i)[j]->encryptedLetter].size() == 0) {
                            invalidKey = true;
                            break;
                        }
                    }
                }
                if (invalidKey) break;
            }
            if (invalidKey) break;

            //remove letters that are already known/guessed from other possible letters
            knownLetters.clear();
            for (vector<list<int>>::const_iterator i = possibleLetters.back().begin(); i != possibleLetters.back().end(); i++) {
                if (i->size() == 1) {
                    knownLetters.push_back(i->front());
                }
            }
            for (vector<list<int>>::iterator i = possibleLetters.back().begin(); i != possibleLetters.back().end(); i++) {
                if (i->size() != 1) {
                    for (list<int>::const_iterator k = knownLetters.begin(); k != knownLetters.end(); k++) {
                        for (list<int>::iterator j = i->begin(); j != i->end();) {
                            if (*j == *k) {
                                j = i->erase(j);
                                possibleLettersChange = true;
                            } else {
                                j++;
                            }
                        }
                    }
                }
            }
            
            //print known letters
            cout << "Known letters: ";
            for (vector<list<int>>::const_iterator i = possibleLetters.back().begin(); i != possibleLetters.back().end(); i++) {
                if (i->size() == 1) {
                    cout << Dictionary::ALPHABET[i->front()] << " ";
                }
            }
            cout << endl;

            //update puzzle if new letters are found
            if (possibleLettersChange && !invalidKey) {
                cout << "Run Again!" << endl;
                replaceLetters(&puzzle.back(), &possibleLetters.back());
            }
        } while (possibleLettersChange && !invalidKey);

        if (invalidKey) {
            cout << "Invalid key" << endl;
            for (vector<vector<CryptogramLetter*>>::iterator i = puzzle.back().begin(); i != puzzle.back().end(); i++) {
                for (vector<CryptogramLetter*>::iterator j = i->begin(); j != i->end(); j++) {
                    delete *j;
                }
            }
            puzzle.pop_back();
            possibleLetters.pop_back();
        }
     }

    //Rebuild
    s = printPuzzle(&puzzle.back());

    //delete used memory
    for (vector<vector<vector<CryptogramLetter*>>>::iterator k = puzzle.begin(); k != puzzle.end(); k++) {
        for (vector<vector<CryptogramLetter*>>::iterator i = k->begin(); i != k->end(); i++) {
            for (vector<CryptogramLetter*>::iterator j = i->begin(); j != i->end(); j++) {
                delete *j;
            }
        }
    }
    return s;
}

void CryptogramPuzzle::replaceLetters(vector<vector<CryptogramLetter*>> *prevLevel, vector<list<int>> *l) {
    for (vector<vector<CryptogramLetter*>>::iterator i = prevLevel->begin(); i != prevLevel->end(); i++) {
        for (vector<CryptogramLetter*>::iterator j = i->begin(); j != i->end(); j++) {
            for (unsigned int k = 0; k < l->size(); k++) {
                if ((*l)[k].size() == 1) {
                    if ((*j)->encryptedLetter == k) {
                        (*j)->uncryptedLetter = (*l)[k].front();
                        (*j)->status = GUESS;
                    }
                }
            }
        }
    }
}

void CryptogramPuzzle::buildPuzzle(vector<vector<CryptogramLetter*>> *puzzle, string s) {
    CryptogramLetter *temp;
    vector<CryptogramLetter*> wordTemp;
    for (char c : s) {
        if (Dictionary::inAlphabet(c) != -1) {
            temp = new CryptogramLetter;

            temp->encryptedLetter = Dictionary::inAlphabet(c);
            temp->status = UNKNOWN;
            temp->uncryptedLetter = Dictionary::inAlphabet(c);

            wordTemp.push_back(temp);
        } else if (wordTemp.size() != 0) {
            if (c == DELIM) {
                puzzle->push_back(wordTemp);
                wordTemp.clear();
            } else {
                //punctuation
                temp = new CryptogramLetter;

                temp->encryptedLetter = -1;
                temp->status = PUNCTUATION;
                temp->uncryptedLetter = -1;
                temp->punctuation = c;

                wordTemp.push_back(temp);
            }
        }
    }
    puzzle->push_back(wordTemp);
}

bool CryptogramPuzzle::isSolved(vector<vector<CryptogramLetter*>> *puzzle, Dictionary *dictionary) {
    string word;
    for (vector<CryptogramLetter*> i : *puzzle) {
        word = "";
        for (CryptogramLetter* j : i) {
            if (j->status == UNKNOWN) return false;
            if (j->status != PUNCTUATION) word += Dictionary::ALPHABET[j->uncryptedLetter];
        }
        if (!dictionary->ifExists(word))return false;
    }
    return true;
}

vector<vector<CryptogramLetter*>> CryptogramPuzzle::copyPuzzleLevel(vector<vector<CryptogramLetter*>> *level) {
    vector<vector<CryptogramLetter*>> newLevel;
    CryptogramLetter *temp;
    for (vector<vector<CryptogramLetter*>>::const_iterator i = level->begin(); i != level->end(); i++) {
        newLevel.push_back(vector<CryptogramLetter*>());
        for (vector<CryptogramLetter*>::const_iterator j = i->begin(); j != i->end(); j++) {
            temp = new CryptogramLetter;
            temp->encryptedLetter = (*j)->encryptedLetter;
            temp->uncryptedLetter = (*j)->uncryptedLetter;
            temp->punctuation = (*j)->punctuation;
            temp->status = (*j)->status;

            newLevel.back().push_back(temp);
        }
    }

    return newLevel;
}