#ifndef CRYTOGRAM_PUZZLE_H
#define CRYTOGRAM_PUZZLE_H

#include "DictionaryTree.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <time.h>

using namespace std;

class EnhancedDictionary;
class Dictionary;
struct CryptogramLetter;

enum STATUS { UNKNOWN, GUESS, KNOWN, PUNCTUATION }; //Unkown means no letter in key, guess means letter in key but not certain
                                                        //known means certain that is correct, Punctuation means ignore

class CryptogramPuzzle {
public:
    /*
     * Scrambles the alphabet in the dictionary then changes each letter
     * That exists in the alphabet to the scrambled one
     */
    static string encryptPhrase(string s);

    /*
     * Checksto make sure each word is in the dictionary before
     * calling the normal encryptPhrase
     */
    static string encryptPhrase(string s, Dictionary *d);

    /*
     * Solves the puzzle by calculating possible letters and elimating those that aren't
     */
    static string solvePuzzle(string s, Dictionary *dictionary);

    /*
     * the charater that seprates words
     */
    static const char DELIM = ' ';
    
private:
    /*
     * adds known letters to puzzle
     */
    static void replaceLetters(vector<vector<CryptogramLetter*>> *prevLevel, vector<list<int>> *l);

    /*
     * Converts a string into a puzzle
     */
    static void buildPuzzle(vector<vector<CryptogramLetter*>> *puzzle, string s);

    /*
     * Checks to see if the puzzle is soved
     */
    static bool isSolved(vector<vector<CryptogramLetter*>> *puzzle, Dictionary *dictionary);

    /*
     * Converts the puzzle back into a string
     */
    static string printPuzzle(vector<vector<CryptogramLetter*>> *puzzle);

    /*
     * Copys the previous level and returns a copy of it
     */
    static vector<vector<CryptogramLetter*>> copyPuzzleLevel(vector<vector<CryptogramLetter*>> *level);
};

struct CryptogramLetter {
    int encryptedLetter;    //index of encrypted letter in dictionary's alphabet
    int uncryptedLetter;    //index of uncrypted letter in dictionary's alphabet
    char punctuation;
    STATUS status;
};


#endif // CRYTOGRAM_PUZZLE_H
