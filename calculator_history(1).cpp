#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <cctype>

using namespace std;

// Class to store calculation history
class History {
private:
    vector<pair<string, double>> records;  // Store expression and result
    const string filename = "calculator_history.txt";  // File for saving history
    
public:
    // Add new record
    void addRecord(const string& expression, double result) {
        records.push_back({expression, result});
        saveToFile(expression, result);
    }
    
    // Show all history
    void displayAll() {
        if (records.empty()) {
            cout << "History is empty." << endl;
            return;
        }
        
        cout << "\n====== Calculation History ======" << endl;
        for (size_t i = 0; i < records.size(); ++i) {
            cout << i + 1 << ". " << records[i].first 
                 << " = " << records[i].second << endl;
        }
        cout << "================================\n" << endl;
    }
    
    // Clear history
    void clear() {
        records.clear();
        ofstream file(filename, ios::trunc);
        file.close();
        cout << "History cleared." << endl;
    }
    
    // Get last result
    double getLastResult() {
        if (records.empty()) return 0.0;
        return records.back().second;
    }
    
    // Save to file
    void saveToFile(const string& expression, double result) {
        ofstream file(filename, ios::app);
        if (file.is_open()) {
            file << expression << " = " << result << endl;
            file.close();
        }
    }
    
    // Load from file
    void loadFromFile() {
        ifstream file(filename);
        if (file.is_open()) {
            records.clear();
            string line;
            while (getline(file, line)) {
                size_t equalPos = line.find('=');
                if (equalPos != string::npos) {
                    string expr = line.substr(0, equalPos - 1);
                    string resultStr = line.substr(equalPos + 2);
                    try {
                        double result = stod(resultStr);
                        records.push_back({expr, result});
                    } catch (...) {
                        // Skip conversion errors
                    }
                }
            }
            file.close();
        }
    }
};

// Class for expression calculation
class ExpressionCalculator {
private:
    // Operator priority
    int precedence(char op) {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        if (op == '^') return 3;
        if (op == 's' || op == 'c' || op == 't' || op == 'l' || op == 'e' || op == 'r') return 4;
        return 0;
    }
    
    // Check if character is operator
    bool isOperator(char c) {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || 
               c == 's' || c == 'c' || c == 't' || c == 'l' || c == 'e' || c == 'r';
    }
    
    // Perform math operation
    double applyOperation(double a, double b, char op) {
        switch(op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': 
                if (b == 0) {
                    throw runtime_error("Error: Division by zero!");
                }
                return a / b;
            case '^': return pow(a, b);
            default: return 0;
        }
    }
    
    // Calculate functions (sin, cos, etc.)
    double applyFunction(char func, double value) {
        switch(func) {
            case 's': return sin(value);  // sin
            case 'c': return cos(value);  // cos
            case 't': return tan(value);  // tan
            case 'l': 
                if (value <= 0) {
                    throw runtime_error("Error: Logarithm of non-positive number!");
                }
                return log10(value);      // log base 10
            case 'e': return exp(value);  // exp
            case 'r': 
                if (value < 0) {
                    throw runtime_error("Error: Square root of negative number!");
                }
                return sqrt(value);       // sqrt
            default: return value;
        }
    }
    
    // Convert infix to postfix notation
    string infixToPostfix(const string& expression) {
        stack<char> operators;
        string postfix;
        
        for (size_t i = 0; i < expression.length(); ++i) {
            char c = expression[i];
            
            // If digit or decimal point
            if (isdigit(c) || c == '.') {
                while (i < expression.length() && (isdigit(expression[i]) || expression[i] == '.')) {
                    postfix += expression[i++];
                }
                postfix += ' ';
                --i;
            }
            // If function (sin, cos, etc.)
            else if (isalpha(c)) {
                // Detect function type
                if (expression.substr(i, 3) == "sin") {
                    operators.push('s');
                    i += 2;
                } else if (expression.substr(i, 3) == "cos") {
                    operators.push('c');
                    i += 2;
                } else if (expression.substr(i, 3) == "tan") {
                    operators.push('t');
                    i += 2;
                } else if (expression.substr(i, 3) == "log") {
                    operators.push('l');
                    i += 2;
                } else if (expression.substr(i, 3) == "exp") {
                    operators.push('e');
                    i += 2;
                } else if (expression.substr(i, 4) == "sqrt") {
                    operators.push('r');
                    i += 3;
                }
            }
            // If opening parenthesis
            else if (c == '(') {
                operators.push(c);
            }
            // If closing parenthesis
            else if (c == ')') {
                while (!operators.empty() && operators.top() != '(') {
                    postfix += operators.top();
                    postfix += ' ';
                    operators.pop();
                }
                if (!operators.empty()) operators.pop();  // Remove '('
                
                // If there was a function before parenthesis
                if (!operators.empty() && (operators.top() == 's' || operators.top() == 'c' || 
                    operators.top() == 't' || operators.top() == 'l' || operators.top() == 'e' || operators.top() == 'r')) {
                    postfix += operators.top();
                    postfix += ' ';
                    operators.pop();
                }
            }
            // If operator
            else if (isOperator(c)) {
                while (!operators.empty() && precedence(operators.top()) >= precedence(c)) {
                    postfix += operators.top();
                    postfix += ' ';
                    operators.pop();
                }
                operators.push(c);
            }
            // Skip spaces
            else if (c == ' ') {
                continue;
            }
        }
        
        // Add remaining operators
        while (!operators.empty()) {
            postfix += operators.top();
            postfix += ' ';
            operators.pop();
        }
        
        return postfix;
    }
    
    // Calculate postfix expression
    double evaluatePostfix(const string& postfix) {
        stack<double> values;
        stringstream ss(postfix);
        string token;
        
        while (ss >> token) {
            // If number
            if (isdigit(token[0]) || (token[0] == '-' && token.length() > 1 && isdigit(token[1]))) {
                values.push(stod(token));
            }
            // If function (one operand)
            else if (token.length() == 1 && (token[0] == 's' || token[0] == 'c' || token[0] == 't' || 
                     token[0] == 'l' || token[0] == 'e' || token[0] == 'r')) {
                if (values.empty()) {
                    throw runtime_error("Error: Invalid expression!");
                }
                double value = values.top();
                values.pop();
                values.push(applyFunction(token[0], value));
            }
            // If two-operand operator
            else if (token.length() == 1 && isOperator(token[0])) {
                if (values.size() < 2) {
                    throw runtime_error("Error: Invalid expression!");
                }
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();
                values.push(applyOperation(a, b, token[0]));
            }
        }
        
        if (values.size() != 1) {
            throw runtime_error("Error: Invalid expression!");
        }
        
        return values.top();
    }
    
public:
    // Calculate main expression
    double calculate(const string& expression) {
        try {
            string postfix = infixToPostfix(expression);
            return evaluatePostfix(postfix);
        } catch (const exception& e) {
            throw;
        }
    }
};

// Main calculator class
class AdvancedCalculator {
private:
    History history;
    ExpressionCalculator calculator;
    double lastResult;
    bool hasLastResult;
    
public:
    AdvancedCalculator() : lastResult(0.0), hasLastResult(false) {
        history.loadFromFile();
    }
    
    // Show menu
    void displayMenu() {
        cout << "\n====== Advanced Algebraic Calculator ======" << endl;
        cout << "1. Enter new expression" << endl;
        cout << "2. Continue with previous result (+/-/*//)" << endl;
        cout << "3. Show calculation history" << endl;
        cout << "4. Clear history" << endl;
        cout << "5. Clear current result (C)" << endl;
        cout << "6. Exit" << endl;
        cout << "===========================================" << endl;
        cout << "Your choice: ";
    }
    
    // Process input expression
    void processExpression(const string& input) {
        try {
            if (input == "C" || input == "c") {
                clearResult();
                return;
            }
            
            double result;
            string expressionToCalculate;
            
            // If expression starts with operator, use previous result
            if (hasLastResult && (input[0] == '+' || input[0] == '-' || input[0] == '*' || input[0] == '/')) {
                expressionToCalculate = to_string(lastResult) + " " + input;
            } else {
                expressionToCalculate = input;
            }
            
            result = calculator.calculate(expressionToCalculate);
            
            cout << "Result: " << fixed << setprecision(6) << result << endl;
            
            // Save to history
            history.addRecord(expressionToCalculate, result);
            
            // Update last result
            lastResult = result;
            hasLastResult = true;
            
        } catch (const exception& e) {
            cout << e.what() << endl;
        }
    }
    
    // Clear result
    void clearResult() {
        lastResult = 0.0;
        hasLastResult = false;
        cout << "Current result cleared." << endl;
    }
    
    // Run calculator
    void run() {
        cout << "Welcome to Advanced Algebraic Calculator!" << endl;
        cout << "Features: + - * / ^ () sin cos tan log exp sqrt" << endl;
  
        
        int choice;
        string input;
        
        do {
            displayMenu();
            cin >> choice;
            cin.ignore();  // Clear buffer
            
            switch(choice) {
                case 1:
                    cout << "Enter expression: ";
                    getline(cin, input);
                    processExpression(input);
                    break;
                    
                case 2:
                    if (!hasLastResult) {
                        cout << "No previous result. Enter an expression first." << endl;
                    } else {cout << "Current result: " << lastResult << endl;
                        cout << "Enter expression (example: +5 or *2): ";
                        getline(cin, input);
                        processExpression(input);
                    }
                    break;
                    
                case 3:
                    history.displayAll();
                    break;
                    
                case 4:
                    history.clear();
                    break;
                    
                case 5:
                    clearResult();
                    break;
                    
                case 6:
                    cout << "Exiting program. Goodbye!" << endl;
                    break;
                    
                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
            
        } while (choice != 6);
    }
};

// Main function
int main() {
    // Console setup for proper display
    system("cls");
    
    AdvancedCalculator calculator;
    calculator.run();
    
    return 0;
}