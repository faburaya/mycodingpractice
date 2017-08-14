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

#define TRACE(X)

/// <sumary>
/// A graph represented by an array of edges in contiguous
/// memory, that allows radix-like lookup for its edges, each
/// one identified by its index.
/// </sumary>
class Graph
{
private:

    struct Edge
    {
        uint16_t count;
        uint16_t available;

        Edge()
            : count(0), available(0) {}
    };

    std::vector<Edge> m_edges;
    
public:

    Graph(int biggestValue)
        : m_edges(abs(biggestValue) + 1)
    {}

    // add value to graph as an edge identified by the absolute
    void AddEdge(int value)
    {
        value = abs(value);

        if (value < m_edges.size())
        {
            auto &edge = m_edges[value];
            edge.available = ++edge.count;
        }
        else
            throw std::out_of_range("ERROR! Radix cannot add out of range value");
    }
    
    // DFS to find a path whose sum of edges reaches the total
    int FindSumOfEdges(int total)
    {
        TRACE(std::cout << "Calling FindSumOfEdges(" << total << ")" << std::endl);

        if (total < 0)
            throw std::logic_error("ERROR! Sum of edges cannot be negative");

        int maxAchieved(0);

        /* starting from the biggest available edge
        that does not exceed the total up front */
        int target = std::min(static_cast<int> (m_edges.size() - 1), total);

        while (target > 0)
        {
            auto &edge = m_edges[target];

            if (edge.available > 0)
            {
                --edge.available;

                int achieved = FindSumOfEdges(total - target) + target;

                if (achieved == total)
                {
                    TRACE(std::cout << "Rewinding recursion: achieved " << total << std::endl);
                    return total;
                }
                
                if (achieved > maxAchieved)
                    maxAchieved = achieved;

                ++edge.available;
            }

            --target;
        }

        TRACE(std::cout << "Rewinding recursion: achieved " << maxAchieved << std::endl);
        return maxAchieved;
    }
};


// Proposed algorithm
int solution(std::vector<int> &A)
{
    if (A.size() == 1)
        return abs(A[0]);

    Graph lookup(100);

    int sumOfAbs(0);

    for (int x : A)
    {
        int absVal = abs(x);
        sumOfAbs += absVal;
        
        lookup.AddEdge(x); // load numbers as edges of the graph
    }

    int targetSum = sumOfAbs / 2;

    return sumOfAbs - 2 * lookup.FindSumOfEdges(targetSum);
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