#ifndef YAC_HPP
#define YAC_HPP

#include <string>
#include <vector>
#include <math.h>
#include <system_error>

namespace yac
{
    enum class yac_error
    {
        no_error = 0,
        wrong_token,
        not_complete_expression,
        not_supported_operator,
        wrong_value,
        devide_by_zero,
        too_many_sign,
        floating_point_many_dot,
        floating_point_number_start_dot,
        floating_point_number_end_dot,
        end_bracket,
    };
}

namespace std
{
  template <>
  struct is_error_code_enum<yac::yac_error>
    : public true_type {};
}


namespace yac
{
    class yac_category_impl: public std::error_category
    {
        public:
        virtual const char* name() const noexcept override
        {
            return "yac";
        }
        virtual std::string message(int ev) const override
        {
            static char const* msgs[] = {
                "no error",
                "parser error, wrong token",
                "expression is not complete",
                "containt not supported operator",
                "value is wrong",
                "devision by zero",
                "too many sign before number",
                "floating point contain too many dot",
                "floating point couldn't start with dot",
                "floating point couldn't end with dot",
                "couldn't find end bracket",
            };
            if (ev < 0 || ev >= int(sizeof(msgs)/sizeof(msgs[0])))
            {
                return "Unknown error";
            }

            return msgs[ev];
        }
    };

    inline const std::error_category& yac_category()
    {
        static yac_category_impl instance;
        return instance;
    }    
    
    inline std::error_code make_error_code(yac_error e)
    {
        return std::error_code(static_cast<int>(e), yac_category());
    }

    class YAC 
    {
    public:
        YAC(std::string expr, std::error_code& error): m_expr(std::move(expr)),
                                                       m_error(error)

        {
            m_error = make_error_code(yac_error::no_error);
        }

        double Solve()
        {
            double result = Parse();
            if (m_error)
            {
                return 0.0;
            }

            if (m_position != m_expr.size())
            {
                m_error = make_error_code(yac_error::not_complete_expression);
                return 0.0;
            }
            return std::round(result * 100) / 100;
        }

    private:


        struct OperatorToken 
        {
            char    Operator;
            uint8_t Priority; 
            double  Value;
        };

        std::string      m_expr;
        std::error_code& m_error;
        size_t           m_position = 0;
        std::vector<OperatorToken> m_parser;


        void SkipSpace()
        {
            while (std::isspace(m_expr[m_position]))
            {
                ++m_position;
            }
        }
        char GetChar()
        {
            if (m_position >= m_expr.size())
            {
                //m_error = make_error_code(yac_error::not_complete_expression);
                return 0;
            }
            return m_expr[m_position];
        }

        double ParseNumer()
        {
            size_t start = m_position;
            ++m_position;
            bool dot = false;
            size_t dotPozition = 0;
            char symbol = 0;
            while ((symbol = GetChar()) != 0)  
            {
                if (symbol == '.' || symbol == ',')
                {
                    if (dot)
                    {
                        m_error = make_error_code(yac_error::floating_point_many_dot);
                        return 0.0;
                    }
                    dot = true;
                    dotPozition = m_position;
                }else if (!std::isdigit(symbol))
                {
                    break;
                }
                ++m_position;
            }
            if (dot &&
                (dotPozition == start ||
                 dotPozition == m_position - 1))
            {
                if (dotPozition == start)
                {
                    m_error = make_error_code(yac_error::floating_point_number_start_dot);
                    return 0.0;

                } else if (dotPozition == m_position - 1)
                {
                    m_error = make_error_code(yac_error::floating_point_number_end_dot);
                    return 0.0;
                }
            }
            std::string number = m_expr.substr(start, m_position - start);

            return dot ? stod(number) : static_cast<double>(stoi(number));
        }

        double Compute(double lhs, double rhs, char oper)
        {
            switch (oper)
            {
                case '*': return lhs * rhs;
                case '-': return lhs - rhs;
                case '+': return lhs + rhs;
                case '/': 
                    if (rhs == 0.0) 
                    {
                        m_error = make_error_code(yac_error::devide_by_zero);
                        return 0.0;
                    }
                    return lhs / rhs;
                default:
                    return 0.0;
            }
        }

        double ParseValueToken(bool signPresent, bool firstValueToken)
        {
            SkipSpace();
            switch (GetChar())
            {
                case '0': case '1': case '2':
                case '3': case '4': case '5':
                case '6': case '7': case '8':
                case '9': 
                    return ParseNumer();
                case '-':
                {
                    if (!firstValueToken || signPresent)
                    {
                        m_error = make_error_code(yac_error::too_many_sign);
                        return 0.0;
                    }
                    ++m_position;
                    return ParseValueToken(true, false) * -1;
                }
                case '+':
                {
                    if (!firstValueToken || signPresent)
                    {
                        m_error = make_error_code(yac_error::too_many_sign);
                        return 0.0;
                    }
                    ++m_position;
                    return ParseValueToken(true, false);
                }
                case '(':
                {
                     ++m_position;
                    double value = Parse();
                    if (m_error)
                    {
                        return 0.0;
                    }
                    SkipSpace();
                    char symbol = GetChar();
                    if (symbol != ')')
                    {
                        m_error = make_error_code(yac_error::end_bracket);
                        return 0.0;                         
                    }
                    ++m_position;
                    return value;
                }
                // just in case
                case '.':
                    m_error = make_error_code(yac_error::floating_point_number_start_dot);
                    return 0.0;
                default:
                    m_error = make_error_code(yac_error::wrong_token);
                    return 0.0;                    
            }
        }

        OperatorToken ParseOperatorToken()
        {
            SkipSpace();
            switch (GetChar())
            {
                case '*':
                {
                    ++m_position;
                    return {'*', 10};
                }
                case '-':
                {
                    ++m_position;
                    return {'-', 5};
                }
                case '+':
                {
                    ++m_position;
                    return {'+', 5};
                }
                case '/':
                {
                    ++m_position;
                    return {'/', 10};
                }
                default:
                {
                    return {0, 0, 0.0};     
                }
            }
            ++m_position;
        }

        double Parse()
        {
            m_parser.push_back({0, 0, 0.0});
            double value = ParseValueToken(false, true);
            if (m_error)
            {
                return 0.0;
            }

            while (!m_parser.empty())
            {
                OperatorToken operToken = ParseOperatorToken();
                if (m_error)
                {
                    return 0.0;
                }

                while (!m_parser.empty() && m_parser.back().Priority >= operToken.Priority)
                {
                    if (m_parser.back().Operator == 0)
                    {
                        m_parser.pop_back();
                        return value;
                    }
                    value = Compute(m_parser.back().Value, value, m_parser.back().Operator);
                    if (m_error)
                    {
                        return 0.0;
                    }
                    m_parser.pop_back();
                }
                operToken.Value = value;
                m_parser.push_back(std::move(operToken));
                value = ParseValueToken(false, false);
                if (m_error)
                {
                    return 0.0;
                }
            }
            return value;
        }

    };

    inline double Solve(std::string expr, std::error_code& error)
    {
        YAC yac(std::move(expr), error);
        return yac.Solve(); 
    }
}

#endif