// CodingPractice.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <regex>
#include <cstdio>
#include <cinttypes>
#include <cmath>

class RadixLookup
{
private:

    std::vector<uint16_t> m_values;

public:

    RadixLookup(int biggestValue)
        : m_values(abs(biggestValue) + 1, 0)
    {}

    void Add(int value)
    {
        int absValue = abs(value);

        if (absValue < m_values.size())
            ++m_values[absValue];
        else
            throw std::out_of_range("ERROR! Radix cannot add out of range value");
    }

    bool FindAndRemove(int value)
    {
        int absValue = abs(value);

        uint16_t &count = m_values[absValue];

        if (count > 0)
        {
            --count;
            return true;
        }

        return false;
    }
};


int solution(std::vector<int> &A)
{
    if (A.size() == 1)
        return abs(A[0]);

    RadixLookup lookup(100);
    int sumOfAbs(0);

    for (int x : A)
    {
        sumOfAbs += abs(x);
        lookup.Add(x);
    }

    const int initTgtSum = (sumOfAbs / 2) + (sumOfAbs % 2);

    int targetSum = initTgtSum;
    int targetVal = std::min(targetSum, 100);

    while (targetSum > 0 && targetVal > 0)
    {
        if (!lookup.FindAndRemove(targetVal))
        {
            --targetVal;
            continue;
        }
        else
        {
            targetSum -= targetVal;
            targetVal = targetSum;
        }
    }

    return abs((sumOfAbs % 2) - 2 * targetSum);
}


int main()
{
    try
    {
        std::regex rgxCaptNumber("^((-?\\d+)\\s*,\\s*)", std::regex_constants::ECMAScript);
        std::regex rgxTestLine("\\[\\s*(.+)\\]", std::regex_constants::ECMAScript);
        
        std::string line;
        std::vector<int> numbers;
        
        do
        {
            std::getline(std::cin, line, '\n');

            std::cout << "line: " << line << std::endl;

            std::cmatch matchLine;

            if (!regex_match(line.c_str(), matchLine, rgxTestLine))
                break;

            std::cmatch matchNumber;
            const char *text = matchLine[1].first;

            std::cout << "bulk: " << text << std::endl;

            while (std::regex_search(text, matchNumber, rgxCaptNumber))
            {
                std::cout << atoi(matchNumber[2].first) << std::endl;
                text = matchNumber[1].second;
            }

        } while (false);
    }
    catch (std::exception &ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}