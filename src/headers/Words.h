#pragma once
#include <vector>
#include <string>
using std::vector;
using std::string;

class Words
{
    private:
        vector<string> words;
    public:
        Words();
        string GetRandomWord() const;
};