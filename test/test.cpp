#include <vector>
#include <iostream>
#include <tuple>
#include <limits>

#include "yac.hpp"

int main()
{   
    std::vector<std::tuple<std::string, 
                           double, 
                           std::error_code>
                           > tests {
        {"0", 0.0, yac::make_error_code(yac::yac_error::no_error)},
        {"1", 1.0, yac::make_error_code(yac::yac_error::no_error)},
        {"12340", 12340.0, yac::make_error_code(yac::yac_error::no_error)},
        {"+12340", 12340.0, yac::make_error_code(yac::yac_error::no_error)},
        {"-12340", -12340.0, yac::make_error_code(yac::yac_error::no_error)},
        {"1.243", 1.24, yac::make_error_code(yac::yac_error::no_error)},
        {"1.246", 1.25, yac::make_error_code(yac::yac_error::no_error)},
        {"+1.24", 1.24, yac::make_error_code(yac::yac_error::no_error)},
        {"-1.24", -1.24, yac::make_error_code(yac::yac_error::no_error)},
        {"-1.", 0, yac::make_error_code(yac::yac_error::floating_point_number_end_dot)},
        {".0", 0, yac::make_error_code(yac::yac_error::floating_point_number_start_dot)},
        {"1 + .1", 0, yac::make_error_code(yac::yac_error::floating_point_number_start_dot)},
        {"(-1.24)", -1.24, yac::make_error_code(yac::yac_error::no_error)},
        {"(-1.24", 0.0, yac::make_error_code(yac::yac_error::end_bracket)},
        {"(-1.24 )", -1.24, yac::make_error_code(yac::yac_error::no_error)},
        {"(-1.24  )", -1.24, yac::make_error_code(yac::yac_error::no_error)},
        {"(( -1.24  ))", -1.24, yac::make_error_code(yac::yac_error::no_error)},
        {"(( -1.24  )", 0.0, yac::make_error_code(yac::yac_error::end_bracket)},
        {" -1.24 ", -1.24, yac::make_error_code(yac::yac_error::no_error)},
        {" -1. 24 ", 0.0, yac::make_error_code(yac::yac_error::floating_point_number_end_dot)},
        {" --1. 24 ", 0.0, yac::make_error_code(yac::yac_error::too_many_sign)},
        {"1 + 2", 3.0, yac::make_error_code(yac::yac_error::no_error)},
        {"5 +    5", 10.0, yac::make_error_code(yac::yac_error::no_error)},
        {"1/2", 0.5, yac::make_error_code(yac::yac_error::no_error)},
        {"15-(2-3)+5", 21, yac::make_error_code(yac::yac_error::no_error)},
        {"1/2 + 3 + 3 * (23) + 3", 75.5, yac::make_error_code(yac::yac_error::no_error)},
        {"1/2 + ((9)) + 1", 10.5, yac::make_error_code(yac::yac_error::no_error)},
        {"1/2 + 1.3 + 3", 4.8, yac::make_error_code(yac::yac_error::no_error)},
        {"1/2 + 1.3 + 3)", 0.0, yac::make_error_code(yac::yac_error::not_complete_expression)},
        {"(((((((((0,0)))))))))", 0.0, yac::make_error_code(yac::yac_error::no_error)},
        {"(((((((((0,0))))))))", 0.0, yac::make_error_code(yac::yac_error::end_bracket)},
        {"((((((((()))))))))", 0.0, yac::make_error_code(yac::yac_error::wrong_token)},
        {"((((((((())))))))))", 0.0, yac::make_error_code(yac::yac_error::wrong_token)},
        {"1 + (3 +2.1*3) + 2*3*4*7/3*2 +7", 129.3, yac::make_error_code(yac::yac_error::no_error)},
        {"1()", 0.0, yac::make_error_code(yac::yac_error::not_complete_expression)},
        {"1)", 0.0, yac::make_error_code(yac::yac_error::not_complete_expression)},
        {"1)", 0.0, yac::make_error_code(yac::yac_error::not_complete_expression)},
        {"- -1", 0.0, yac::make_error_code(yac::yac_error::too_many_sign)},
        {"1 - -1", 0.0, yac::make_error_code(yac::yac_error::too_many_sign)},
        {"1 - + 1", 0.0, yac::make_error_code(yac::yac_error::too_many_sign)},
        {"1 - (+ 1)", 0.0, yac::make_error_code(yac::yac_error::no_error)},
        {"1 - (+ 1.0)", 0.0, yac::make_error_code(yac::yac_error::no_error)},
        {"1 - (+ 1.00.)", 0.0, yac::make_error_code(yac::yac_error::floating_point_many_dot)},
        {"1/0", 0.0, yac::make_error_code(yac::yac_error::devide_by_zero)},
        {"1/(2 - 2)", 0.0, yac::make_error_code(yac::yac_error::devide_by_zero)},
        {"aefwef", 0.0, yac::make_error_code(yac::yac_error::wrong_token)},
        {"99999*9999", 9.9989e+08, yac::make_error_code(yac::yac_error::no_error)},
    };

    for (const auto& expected: tests)
    {
        const std::string& checkExp = std::get<0>(expected);
        double checkResult = std::get<1>(expected);
        std::error_code checkError = std::get<2>(expected);
        std::error_code error;
        double result = yac::Solve(checkExp, error);
        if (error != checkError)
        {
            std::cerr << "fail to check exp '" << checkExp 
                      << "': return error code is wrong (expected '" 
                      << checkError.message() << "' but return '"
                      << error.message() << "')" << std::endl;
            return 1;
        }

        if (error)
        {
            continue;
        }

        if (std::abs(result - result) >= std::numeric_limits<double>::epsilon())
        {
            std::cerr << "fail to check exp '" << checkExp 
                      << "': return result is wrong (expected '" 
                      << checkResult << "' but return '"
                      << result << "')" << std::endl;   
            return 2;    
        }
    }

    return 0;
}