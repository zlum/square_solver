#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <queue>

using namespace std;

// Struct with coefficients and roots of quadratic equation
struct QuadEquation
{
    array<int, 3> coeff{}; // TODO: Check if I need to init it
    pair<double, double> root;
};

// Helper function to calculate one root
double calcRoot(double sqrtDsc, bool isNegative, int a, int b)
{
    return (-b + (isNegative ? -sqrtDsc : sqrtDsc)) / (2.0 * a);
}

// Calculates roots of quadratic equation
// Using NaN as no root mark
pair<double, double> calcRoots(int a, int b, int c)
{
    // Check if it is a quadratic equation
    if(a == 0)
    {
        // Calc as simple bx + c = 0 equation
        return {double(-c) / double(b),
                numeric_limits<double>::quiet_NaN()};
    }

    int discriminant = (b * b) - (4 * a * c);

    // Determines root count by discriminant sign
    if(discriminant > 0)
    {
        // Two roots
        double sqrtDsc = sqrt(discriminant);

        return {calcRoot(sqrtDsc, false, a, b),
                calcRoot(sqrtDsc, true, a, b)};
    }

    if(discriminant == 0)
    {
        // One root
        double sqrtDsc = sqrt(discriminant);

        return {calcRoot(sqrtDsc, false, a, b),
                numeric_limits<double>::quiet_NaN()};
    }

    // No roots
    return {numeric_limits<double>::quiet_NaN(),
            numeric_limits<double>::quiet_NaN()};
}

// Prints quadratic equation in format:
// (a b c) => (x1, x2)
void printQuadEquation(const QuadEquation& equation)
{
    cout << '('
         << equation.coeff.at(0) << ' '
         << equation.coeff.at(1) << ' '
         << equation.coeff.at(2) << ") => ";

    if(isnan(equation.root.first))
    {
        cout << "no roots" << endl;
    }
    else
    if(isnan(equation.root.second))
    {
        // One root
        cout << '('
             << equation.root.first << ')'
             << endl;
    }
    else
    {
        // Two roots
        cout << '('
             << equation.root.first << ", "
             << equation.root.second << ')'
             << endl;
    }
}

// Reads coefficients of quadratic equation from cli arguments
queue<array<int, 3>> readCoeffs(int argc, char* argv[])
{
    // Validator. Allows digits and '-' sign only
    auto isNotValidChar = [](char c){ return !bool(isdigit(c)) && (c != '-'); };
    queue<array<int, 3>> coeffQueue;

    // 3 values by step. Skips argv[0] cause it is working directory
    size_t i = 1;

    while(i + 2 < size_t(argc))
    {
        size_t j = 0; // Counter of correctly read symbols
        array<int, 3> coeff{};

        while(j < 3)
        {
            string str(argv[j + i]); // Get next argument

            // Remove junk symbols
            str.erase(remove_if(str.begin(), str.end(), isNotValidChar),
                      str.end());

            try
            {
                coeff.at(j) = stoi(str);
            }
            catch(...)
            {
                ++i; // Skip current
                if((i + j) >= size_t(argc)) break; // Boundary check
                continue;
            }

            ++j;
        }

        // Push coefficients to the queue if all of them had been read correctly
        if(j == 3)
        {
            coeffQueue.emplace(coeff);
        }

        i += 3;
    }

    return coeffQueue;
}

int main(int argc, char* argv[])
{
    QuadEquation equation;

    queue<array<int, 3>> coeffs = readCoeffs(argc, argv);

    while(!coeffs.empty())
    {
        equation.coeff = coeffs.front();
        coeffs.pop();

        equation.root = calcRoots(equation.coeff.at(0),
                                  equation.coeff.at(1),
                                  equation.coeff.at(2));

        printQuadEquation(equation);
    }

    return 0;
}
