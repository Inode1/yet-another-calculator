#include <iostream>

#include "yac.hpp"

int main()
{
    std::string expr;
    while (std::getline(std::cin, expr))
    {
        std::error_code error;
        double result = yac::Solve(expr, error);
        if (error)
        {
            std::cout << "Smth bad happened while parsing command '" 
                      << expr  << "': " << error.message() << std::endl;
            continue;
        }
        std::cout << "Result : " << result << std::endl;
    }
}