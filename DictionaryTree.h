#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "CryptogramPuzzle.h"
#include <string>
#include <cassert>
#include <vector>
#include <fstream>
#include <iostream>
#include <list>
#include <algorithm>

using namespace std;

class Dictionary;
struct CryptogramLetter;

class LetterNode {
public:
    friend class Dictionary;

    /*
     * Creates a vector of LetterNode Pointers the size of the alphabet to store the next letter
     */
    LetterNode();
    ~LetterNode();

    /*
     * Receives the remanding word that is trying to be stored,
     * if a LetterNode does not exist to store the next letter, then create it,
     * then calls the next LetterNode to store
     */
    void build(string word);

    /*
     * recives the remanding word that is trying to be found,
     * if a LetterNode does not exist for the next charater in the word,
     * it will return false, otherwise keep calling for the next letter,
     * until it reaches the end
     */
    bool const ifExists(string word);

    /*
     * recives a string of previous letters
     * if this is the last letter in the series, then it will print the word
     */
    void printDictionary(ostream &os = cout, string l = "") const;
    
    /*
     *
     *
     *
     *
     */
    bool possibleLetters(vector<CryptogramLetter*> word, vector<list<int>> *possibleLetters, unsigned int iteration, vector<list<int>> *letters) const;

protected:
    LetterNode(const LetterNode &t) { assert(false); }
    const LetterNode &operator=(const LetterNode &n) { assert(false); return *this; }

    bool endOfWord;
    vector<LetterNode*> letters;
};

class Dictionary {
public:

    ~Dictionary();
    /*
     * A public constant that is the known alphabet for encrypting/decrypting
     */
    static const vector<char> ALPHABET;

    /*
     * Takes a file name filled with words to add the the dictionary,
     * then reads each word, checks to make sure each letter is in the Alphabet,
     * then calls the root LetterNode for the word's size for it to be stored
     */
    void build(string file);

    /*
     * Takes a string and checks if it exists in the dictionary,
     * Calls the root LetterNode for checking
     */
    bool const ifExists(string word);

    /*
     * Takes a char and checks to see if it exists in the Alphabet
     */
    static int inAlphabet(char c);

    /*
     * Prints the Dictionary by size then by alphabetical order
     */
    void printDictionary(ostream &os = cout) const { for (LetterNode *i:root) i->printDictionary(os); }

    /*
     * Prints the words of a specified size
     */
    void printDictionary(unsigned int size, ostream &os = cout) const { if (size >= 0 && size < root.size()) root[size]->printDictionary(os); }

    /*
     * Takes a "word" made of cryptogram letters and 
     * a list of possible letters that each letter could be for that word,
     * then passes this on to the root of that size
     */
    vector<list<int>> possibleLetters(vector<CryptogramLetter*> word, vector<list<int>> *possibleLetters) const;

protected:
    vector<LetterNode *> root;
};



#endif // DICTIONARY_H
