#include <fstream>
#include <iostream>
#include "../headers/Words.h"

using std::endl;
using std::getline;
using std::ifstream;

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
    ifstream file("../assets/words.txt");
    while (getline(file, line))
    {
        words.push_back(line);
    }
    file.close();
}
array<string, 3> Words::GetRandomWords() const
{
    array<string, 3> three_words;
    for (int i = 0; i < 3; i++)
    {
        three_words[i] = words[rand() % words.size()];
    }
    return three_words;
}