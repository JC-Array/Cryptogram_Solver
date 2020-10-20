#include <iostream>
#include "DictionaryTree.h"
#include "CryptogramPuzzle.h"

using namespace std;

/*
 To do:
    Dictionary Upgrades:
        Allow quick load of dictionary by storing nodes
    Build UI
        Allow messages to be stored
        Allow input of encrypted or uncryped messages
        Allow auto decrypt or manual
    Cryptogram_Puzzle Upgrades:
        Allow known letters to be entered at start
        Allow auto and manual mode
        Speed up
        Keep double spaces and Captilization
*/

int main()
{
    string phrase = "Today's weather report states clear sky's with a slight breeze.  Remember to bring a jacket because it will by chilly.";
    string encryptedPhrase;
    string dictionaryFile = "refWordsSmall.txt";
    string answer;

    cout << "Start" << endl;

    Dictionary dictionary;

    cout << "Building" << endl;

    dictionary.build(dictionaryFile);

    cout << "OG Phrase: " << phrase << endl;
    encryptedPhrase = CryptogramPuzzle::encryptPhrase(phrase, &dictionary);
    cout << "Encryted : " << encryptedPhrase << endl;

    cout << "Start Puzzle" << endl;

    answer = CryptogramPuzzle::solvePuzzle(encryptedPhrase, &dictionary);

    cout << "Finish" << endl;

    cout << answer << endl;

    return 0;
}
