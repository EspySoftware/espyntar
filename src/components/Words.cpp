#include <fstream>
#include <iostream>
#include "../headers/Words.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::getline;

// Words::Words()
// {
//     string line;
//     ifstream file("assets/words.txt");
//     while(getline(file, line))
//     {
//         words.push_back(line);
//         cout << line << endl;
//     }
// }
Words::Words()
{
    string line;
    ifstream file("assets/words.txt");
    while(getline(file, line))
    {
        words.push_back(line);
        cout << line << endl;
    }
}