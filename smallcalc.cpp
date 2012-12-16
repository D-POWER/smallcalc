#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <sstream>

#define MAX_TOKEN_LENGTH 20
#define MAX_EXPRESSION_LENGTH 1000

// A utility function for converting a number to a string
namespace std
{
std::string to_string(float n)
{
    stringstream ss;
    ss << n;
    return ss.str();
}
}

namespace smallcalc
{
enum TOKEN_TYPE
{
  UNDEFINED,
	VARIABLE,
	NUMBER,
	ADDITION_OP,
	SUBSTRACTION_OP,
	DIVISION_OP,
	MULTIPLICATION_OP,
	OPEN_PARENTHESIS,
	CLOSE_PARENTHESIS
};

/**
 * 	A simple recursive descent parser
 *	inspired from the book "The Art of C++" by Herbert Schildt
 */
class Parser
{
public:
	//! Takes an expression and returns the result as a float
	float parse(char *expr);

private:
	//! Returns the value of the current token as a float if it is of type NUMBER otherwise an exception is thrown
	float get_number();
	//! Evaluates an arithmetic expression based on the operator's id
	float eval_operator(TOKEN_TYPE op_id, float lhs, float rhs);
	//! Finds the next token in the expression and stores its type in token_type and its value in token_value
	void next_token();
	//! Parses addition and substraction
	float level1();
	//! Parses division and multiplication
	float level2();
    //! Unary minus
	float level3();
	//! Parses parentheses
	float level4();

	TOKEN_TYPE token_type;
	char token_value[MAX_TOKEN_LENGTH];
	char m_expr[MAX_EXPRESSION_LENGTH];
	char *m_cursor; // It points to the beginning of a token at each step
};
}

float smallcalc::Parser::parse(char *expr)
{
	try
	{
		strcpy(m_expr, expr);
		m_cursor = m_expr;
		strcpy(token_value, "");
		token_type = UNDEFINED;
		next_token();
		return level1();
	}
	catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0.0f;
}

float smallcalc::Parser::get_number()
{
	if (token_type == NUMBER)
    {
        float n = strtod(token_value, NULL);
        next_token();
        return n;
    }
	else
		throw std::domain_error("get_number() was called while the current token is not of type NUMBER");
}

float smallcalc::Parser::eval_operator(TOKEN_TYPE op_id, float lhs, float rhs)
{
	switch (op_id)
	{
		case ADDITION_OP:
			return lhs + rhs;
		case SUBSTRACTION_OP:
			return lhs - rhs;
		case DIVISION_OP:
			return lhs / rhs;
		case MULTIPLICATION_OP:
			return lhs * rhs;
		default:
			throw std::domain_error("eval_operator() was called with an unrecognized operator id");
	}
}

void smallcalc::Parser::next_token()
{
	// Are we out of bounds?
	if (m_cursor == '\0')
	{
		token_type = UNDEFINED;
		return;
	}
	// Skip whitespace
	while (isspace(*m_cursor))
		++m_cursor;
	// Check against an operator
	if (*m_cursor == '+')
	{
		token_type = ADDITION_OP;
		strcpy(token_value, "+");
		++m_cursor;
	}
	else if (*m_cursor == '-')
	{
		token_type = SUBSTRACTION_OP;
		strcpy(token_value, "-");
		++m_cursor;
	}
	else if (*m_cursor == '/')
	{
		token_type = DIVISION_OP;
		strcpy(token_value, "/");
		++m_cursor;
	}
	else if (*m_cursor == '*')
	{
		token_type = MULTIPLICATION_OP;
		strcpy(token_value, "*");
		++m_cursor;
	}
	else if (*m_cursor == '(')
	{
		token_type = OPEN_PARENTHESIS;
		strcpy(token_value, "(");
		++m_cursor;
	}
	else if (*m_cursor == ')')
	{
		token_type = CLOSE_PARENTHESIS;
		strcpy(token_value, ")");
		++m_cursor;
	}
	else if (isdigit(*m_cursor)) // Check against a number (or a variable?)
	{
		int i = 0;
		while ((isdigit(*m_cursor) && m_cursor != '\0'))
		{
			token_value[i++] = *m_cursor;
			++m_cursor;
			if (*m_cursor == '.')
            {
                token_value[i++] = *m_cursor;
                ++m_cursor;
            }
		}
		token_value[i] = '\0';
		token_type = NUMBER;
	}
	else if (*m_cursor != '\0') // Token is not recognized
	{
		throw std::domain_error("Error occured at column " + std::to_string(m_cursor - m_expr + 1));
		token_type = UNDEFINED;
	}
}

// Addition and substraction

float smallcalc::Parser::level1()
{
	float ans = level2();
	TOKEN_TYPE operator_id = token_type;
	while (operator_id == ADDITION_OP || operator_id == SUBSTRACTION_OP)
	{
		next_token();
		ans = eval_operator(operator_id, ans, level2());
		operator_id = token_type;
	}
	return ans;
}

// Multiplication and division

float smallcalc::Parser::level2()
{
	float ans = level3();
	TOKEN_TYPE operator_id = token_type;
	while (operator_id == DIVISION_OP || operator_id == MULTIPLICATION_OP)
	{
		next_token();
		ans = eval_operator(operator_id, ans, level3());
		operator_id = token_type;
	}
	return ans;
}

// Unary minus

float smallcalc::Parser::level3()
{
	if (token_type == SUBSTRACTION_OP)
	{
		next_token();
		float ans = -level4();
		return ans;
	}
	else
        return level4();
}

// Parentheses

float smallcalc::Parser::level4()
{
	float ans;
	if (token_type == OPEN_PARENTHESIS)
	{
		next_token();
		ans = level1();
		if (token_type != CLOSE_PARENTHESIS)
			throw std::domain_error("Missing close parenthesis");
        else
            next_token();
	}
	else
		return get_number();
	return ans;
}

int main(int argc, char *argv[])
{
    // Create a fresh Parser object
    smallcalc::Parser parser;
    // User expressions should not exceed MAX_EXPRESSION_LENGTH in length
    char expr[MAX_EXPRESSION_LENGTH];
    std::cout << "Enter an expression (empty string to exit)" << std::endl;
    do
    {
        std::cout << "> ";
        std::cin.getline(expr, MAX_EXPRESSION_LENGTH);
        if (strcmp(expr, ""))
        {
            float ans = parser.parse(expr);
            std::cout << "\tAns = " << ans << std::endl;
        }
        std::cout << std::endl;
    }
    while (strcmp(expr, ""));
    return 0;
}
