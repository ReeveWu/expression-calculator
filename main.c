#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

/*
Exception handling:
1. Check if there is "/0" or "%0"
2. Check the use of parentheses
3. Check if input is an integer
4. Check the use of operators
*/
bool ErrorExpression;

typedef struct {
    char stk[100000];
    int top;
} stack;

typedef struct {
    double values[1000000];
    int front;
    int rear;
} queue;

typedef struct {
    double stk[100000];
    int top;
} ansStack;

// Define the stack for storing operators
stack operatorStack = {.top = -1};
// Define the queue for storing postfix results
queue postfixQueue = {.front = -1, .rear = -1};
// Define the stack for answers of calculations
ansStack answer = {.top = -1};

// Calculation functions
int infixToPostfix(char *infix, int infixPointer, int n);
int postfixToAns(int n);
double calculate(double op1, double op2, char operator);

// Functions for manipulating stacks and queues
void postfixEnqueue(double data);
double postfixDequeue();
int postfixQueueIsEmpty();
void operatorStackPush(char num);
char operatorStackPop();
void AnsStackPush(double num);
double AnsStackPop();

// Memory reset
void resetMemory();

// Operator priority
int precedence(char operator);

// Exception handling
void parentheses_error(int *count, int unit);

int main() {
// User input expression
    int T;
    printf("Please enter the number of expressions: ");
    scanf("%d", &T);
    printf("Please enter each of expressions: \n");
    char infix[1000000+T], single_str[1000000];
    for (int n = 0 ; n < T; n++) {
        printf("%d -> ", n + 1);
        scanf("%s", single_str);
        int len1 = strlen(infix);
        int len2 = strlen(single_str);
        // The length of the string resulting from adding expressions should be less than 10^6
        if ((len1+len2) > (1000000+T)) {
            printf("Error: Total expression length is bigger than 1000,000. ");
            return 0;
        }
        for (int i = 0; i < len2; i++) {
            infix[len1 + i] = single_str[i];
        }
        infix[len1 + len2] = ',';
    }

    printf("========================= Answer =========================\n");
    // Calculate the answer for each expression
    int infixPointer = 0;
    for (int n = 0; n < T; n++) {
        resetMemory();
        infixPointer = infixToPostfix(infix, infixPointer, n + 1);
        if (!ErrorExpression) postfixToAns(n + 1);
    }
    return 0;
}

void operatorStackPush(char num) { operatorStack.stk[++operatorStack.top] = num; }

char operatorStackPop() { return operatorStack.stk[operatorStack.top--]; }

void AnsStackPush(double num) { answer.stk[++answer.top] = num; }

double AnsStackPop() { return answer.stk[answer.top--]; }

void postfixEnqueue(double data) { postfixQueue.values[++postfixQueue.rear] = data; }

double postfixDequeue() { return postfixQueue.values[++postfixQueue.front]; }

int postfixQueueIsEmpty() { return postfixQueue.front == postfixQueue.rear; }

void resetMemory() {
    ErrorExpression = true;
    operatorStack.top = postfixQueue.front = postfixQueue.rear = answer.top = -1;
}

// Calculate the result of an operation
void parentheses_error(int *parenthesesCount, int unit) { *parenthesesCount += unit; }

int precedence(char operator) {
    switch (operator) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        case '^':
        case '%':
            return 3;
        default:
            return 0;
    }
}

double calculate(double op1, double op2, char operator) {
    switch (operator) {
        case '+':
            return op1 + op2;
        case '-':
            return op1 - op2;
        case '*':
            return op1 * op2;
        case '/':
            return op1 / op2;
        case '^':
            return pow(op1, op2);
        case '%':
            return fmod(op1, op2);
        default:
            return 0;
    }
}

// Convert infix expression to postfix and store it in the queue
int infixToPostfix(char *infix, int infixPointer, int n) {
    bool isDivByZero = false;
    bool isInteger = true;
    int parenthesesCount = 0;
    char ch;
    // Read each character
    while ((ch = infix[infixPointer++]) != ',') {
        if (ch == '.') isInteger = false;
        // Check if the character is a digit
        if (isdigit(ch)) {
            char num[100];
            int k = 0;
            // Handle multiple digits
            while (isdigit(ch)) {
                num[k++] = ch;
                ch = infix[infixPointer++];
            }
            num[k] = '\0';
            postfixEnqueue((double) atoi(num));
            infixPointer--;
        } else if (ch == '(') {
            parentheses_error(&parenthesesCount, 1);
            operatorStackPush(ch);
        } else if (ch == ')') {
            parentheses_error(&parenthesesCount, -1);
            // The number of right parentheses should never be greater than the number of left
            if (parenthesesCount < 0) {
                while ((ch = infix[infixPointer++]) != ',') continue;
                break;
            }
            // Extract operators from the operator stack until a left parenthesis is encountered
            while (operatorStack.stk[operatorStack.top] != '(') {
                // Convert the operator to ASCII and store it in the queue
                postfixEnqueue((double) ((int) operatorStackPop() * (-1)));
            }
            operatorStackPop();
        } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^' || ch == '%') {
            if (ch == '/' || ch == '%') {
                // The expression cannot be "x/0" or "x%0"
                if (infix[infixPointer] == '0') isDivByZero = true;
            }
            // Push the operator onto the operator stack
            while (operatorStack.top >= 0 && precedence(ch) <= precedence(operatorStack.stk[operatorStack.top])) {
                postfixEnqueue((double) ((int) operatorStackPop() * (-1)));
            }
            operatorStackPush(ch);
        }
    }
    // Pop all operators from the operator stack
    while (operatorStack.top >= 0) {
        postfixEnqueue((double) ((int) operatorStackPop() * (-1)));
    }

    // Exception handling
    if (parenthesesCount) {
        printf("Expression %d: <Error: Wrong use of parentheses>\n", n);
    } else if (!isInteger) {
        printf("Expression %d: <Error: Input is not an integer>\n", n);
    } else if (isDivByZero) {
        printf("Expression %d: <Error: Cannot be divided by zero>\n", n);
    } else {
        ErrorExpression = false;
    }
    // Return the current index of infix string
    return infixPointer;
}

// Calculate the result of postfix
int postfixToAns(int n) {
    // Check if there are more operators than operands
    int operatorCount = 0;
    while (!postfixQueueIsEmpty()) {
        // Read data from the queue in order
        double data = postfixDequeue();
        if (data >= 0) {
            operatorCount -= 1;
            // Take the operand and push it into the AnsStack
            AnsStackPush(data);
        } else {
            operatorCount += 1;
            // Take the operator and two elements from the AnsStack to perform the calculation
            double op2 = AnsStackPop();
            double op1 = AnsStackPop();
            // Push the result back into the AnsStack
            AnsStackPush(calculate(op1, op2, (char) (data * (-1))));
        }
    }
    // Exception handling
    if (operatorCount >= 0) {
        printf("Expression %d: <Error: Incorrect number of operator>\n", n);
        return 0;
    }
    // Print the calculation result
    printf("Expression %d: %.13f\n", n, AnsStackPop());
    return 0;
}