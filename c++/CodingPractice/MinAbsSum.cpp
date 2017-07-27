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


/// <sumary>
/// A container of contiguous memory that enables "radix lookup"
/// (where elements are indexed by their absolute values).
/// </sumary>
class PartitionedRadix
{
private:

    struct Element
    {
        uint16_t unmarkedCount;
        uint16_t markedCount;

        Element()
            : unmarkedCount(0), markedCount(0) {}
    };

    std::vector<Element> m_values;

public:

    PartitionedRadix(int biggestValue)
        : m_values(abs(biggestValue) + 1)
    {}

    // add value to radix in UNmarked partition
    void AddUnmarked(int value)
    {
        value = abs(value);

        if (value < m_values.size())
            ++m_values[value].unmarkedCount;
        else
            throw std::out_of_range("ERROR! Radix cannot add out of range value");
    }

    // find value in radix UNmarked partition and move to the marked one
    bool MoveToMarkedPart(int unmarkedValue)
    {
        unmarkedValue = abs(unmarkedValue);

        if (unmarkedValue >= m_values.size())
            throw std::out_of_range("ERROR! Radix cannot move out of range value");

        auto &entry = m_values[unmarkedValue];

        if (entry.unmarkedCount > 0)
        {
            --entry.unmarkedCount;
            ++entry.markedCount;
            return true;
        }

        return false;
    }

    // swap a value between UNmarked and marked partitions, given the delta from one to another
    bool SwapBetweenParts(int delta)
    {
        if (abs(delta) >= m_values.size())
            throw std::out_of_range("ERROR! Radix cannot swap out of range values");

        // calculate boundaries:

        int absValUnmarked, endAbsValUnmarked;

        if (delta > 0) // value in UNmarked partition is inferior
        {
            absValUnmarked = 0;
            endAbsValUnmarked = m_values.size() - delta;
        }
        else // value in UNmarked partition is superior
        {
            absValUnmarked = -delta;
            endAbsValUnmarked = m_values.size();
        }

        // scan the array looking for the pair to swap:
        while (absValUnmarked < endAbsValUnmarked)
        {
            auto &entry2 = m_values[absValUnmarked + delta];
            auto &entry1 = m_values[absValUnmarked];

            if (entry1.unmarkedCount == 0 || entry2.markedCount == 0)
                ++absValUnmarked;
            else
            {// swap between partitions:
                --entry1.unmarkedCount;
                ++entry1.markedCount;
                --entry2.markedCount;
                ++entry2.unmarkedCount;
                return true;
            }
        }

        return false;
    }
};


// Proposed algorithm
int solution(std::vector<int> &A)
{
    if (A.size() == 1)
        return abs(A[0]);

    PartitionedRadix lookup(100);

    // Start using greedy logic to find a almost optimal solution:

    int maxAbsVal(0);
    int sumOfAbs(0);

    for (int x : A)
    {
        int absVal = abs(x);
        sumOfAbs += absVal;
        
        lookup.AddUnmarked(x); // at first, add to unmarked partition

        /* keep track of maximum, so as to
        save some iterations further ahead: */

        if (absVal <= maxAbsVal)
            continue;
        else
            maxAbsVal = absVal;
    }

    int targetSum = (sumOfAbs / 2) + (sumOfAbs % 2);
    int targetVal = std::min(targetSum, maxAbsVal);

    while (targetSum > 0 && targetVal > 0)
    {
        if (!lookup.MoveToMarkedPart(targetVal))
        {
            --targetVal;
            continue;
        }
        else
        {
            targetSum -= targetVal;
            targetVal = std::min(targetSum, maxAbsVal);
        }
    }

    int greedyResult = (sumOfAbs % 2) - 2 * targetSum;

    // finish here if greedy logic was enough to find optimal solution
    if (abs(greedyResult) <= 1)
        return abs(greedyResult);

    std::cout << "greedy result is " << greedyResult << std::endl;

    /* Now we can try permutations between marked and unmarked partitions
    in order to eliminate the remaining difference obtained in the greedy
    solution. This can be achieved by moving half the amount of the difference
    from one partition to another. The greedy result is positive when the
    sum in the UNmarked partition is greater than the sum in the marked
    partition, hence an UNmarked value must be swapped with a superior value
    in the marked partition. When the greedy result is negative, the opposite
    must be done. */

    int equalizationTarget = greedyResult / 2;

    int increment = -equalizationTarget / abs(equalizationTarget);

    do
    {
        if (lookup.SwapBetweenParts(equalizationTarget))
            return abs(greedyResult - 2 * equalizationTarget);
        else
            equalizationTarget += increment;

    } while (equalizationTarget > 0);

    return abs(greedyResult);
}


// entry point with regex-based-parser
int main()
{
    try
    {
        std::regex rgxCaptNumber(
            "^([+|-]?\\d+(?:\\s*,\\s*)?)",
            std::regex_constants::ECMAScript | std::regex_constants::optimize
        );

        std::regex rgxTestLine("\\[\\s*(.+)\\]", std::regex_constants::ECMAScript);
        
        std::string line;
        std::vector<int> numbers;
        
        do
        {
            std::getline(std::cin, line, '\n');

            std::cmatch matchLine;

            if (!regex_match(line.c_str(), matchLine, rgxTestLine))
                break;

            numbers.clear();

            std::cmatch matchNumber;
            const char *text = matchLine[1].first;

            while (std::regex_search(text, matchNumber, rgxCaptNumber))
            {
                numbers.push_back(atoi(matchNumber[1].first));
                text = matchNumber[1].second;
            }

            std::cout << "minimum absolute sum is " << solution(numbers) << std::endl;

        } while (!numbers.empty());

        return EXIT_SUCCESS;
    }
    catch (std::exception &ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}