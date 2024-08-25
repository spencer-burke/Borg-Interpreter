#include <iostream>
#include <sstream>
#include <fstream>

#define TABLESIZE 12

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::stringstream;
using std::ifstream;

class VNode
{
    public:
        int _lscope;
        double _value;
        string _name;
        VNode* _next;
        void print_info();
        VNode(int lscope, double value, string name, VNode* next): _lscope(lscope), _value(value), _name(name), _next(next) {}
        VNode(int lscope, double value, string name): _lscope(lscope), _value(value), _name(name), _next(NULL) {}
};

void VNode::print_info()
{
    cout << "_lscope --> " << _lscope << endl;
    cout << "_value --> " << _value << endl;
    cout << "_name --> " << _name << endl;
}

class Stack
{
    public:
        VNode* top;
        Stack(): top(NULL) {}
        void push(VNode* arg);
        void pop();
        VNode* peek();
        void dump_stack();
        VNode* lookup(string arg);
};

void Stack::push(VNode* arg)
{
    if (top == NULL)
    {
        top = arg;
    }
    else
    {
        arg->_next = top;
        top = arg;
    }
}

void Stack::pop()
{
    if (top == NULL)
        return;
    top = top->_next;
}

VNode* Stack::peek() { return top; }

void Stack::dump_stack()
{
    VNode* curr = top;
    if (curr == NULL)
    {
        cout << "[]" << endl;
        return;
    }
    while (curr != NULL)
    {
        cout << "["<< curr->_lscope << ", " << curr->_value << ", " << curr->_name << "]" << " ";
        curr = curr->_next;
    }
    cout << endl;
}

VNode* Stack::lookup(string arg)
{
    VNode* curr = top;
    while(curr != NULL)
    {
        if (curr->_name.compare(arg) == 0)
            return curr;
        curr = curr->_next;
    }
    return NULL;
}

class HTable
{
    public:
        Stack* table[TABLESIZE];
        int hash(string arg, int length);
        void insert(VNode* arg);
        void remove(VNode* arg);
        void dump_table();
        VNode* lookup(string arg);
        HTable() 
        {
            for (int i = 0; i < TABLESIZE; i++)
                table[i] = new Stack();
        }
};

int HTable::hash(string arg, int length)
{
    int result = 0;
    for (int i = 0; i < length; i++)
        result += int(arg[i]) * (i+1);
    return result % TABLESIZE;
}

void HTable::insert(VNode* arg)
{
    int i = hash(arg->_name, arg->_name.length());
    table[i]->push(arg);
}

void HTable::remove(VNode* arg)
{
    int i = hash(arg->_name, arg->_name.length());
    table[i]->pop();
}

void HTable::dump_table()
{
    for (int i = 0; i < TABLESIZE; i++)
        table[i]->dump_stack();
}

VNode* HTable::lookup(string arg)
{
    int i = hash(arg, arg.length());
    return table[i]->lookup(arg);
}

class Runtime
{
    public:
        int glscope;
        HTable* symbol_table;
        Stack* scope_list;
        void interpret_file(string filename);
        void interpret_line(string line);
        void handle_start();
        void handle_finish();
        void handle_var(string line);
        void handle_print(string line);
        bool check_number(string arg);
        int count_words(string line);
        void handle_expression(string line);
        double evaluate_expression(double val1, double val2, string exp_operator);
        Runtime(): glscope(-1) 
        { 
            symbol_table = new HTable(); 
            scope_list = new Stack();
        }
};

void Runtime::handle_start()
{
    glscope++;
    # ifdef DEBUG
        cout << "[DEBUG]: glscope --> " << glscope << endl;
    # endif
}

void Runtime::handle_finish()
{
    // get rid of everything on the scope list with the same scope and then decrement it
    VNode* curr = scope_list->peek();
    while (curr != NULL && curr->_lscope == glscope)
    {
        // remove current node from the table, and then get the next one
        symbol_table->remove(curr);
        scope_list->pop();
        curr = scope_list->peek();
    }
    glscope--;
    return;
}

void Runtime::handle_var(string line)
{
    string var_name;
    double var_value;
    int var_scope = glscope;
    stringstream s(line);
    string curr;
    s >> curr;
    s >> var_name;
    s >> curr;
    s >> var_value;
    // two of them are created, because the next pointer is modified when pushed to the scope stack
    VNode* table_node = new VNode(var_scope, var_value, var_name);
    VNode* list_node = new VNode(var_scope, var_value, var_name);
    symbol_table->insert(table_node);
    scope_list->push(list_node);
}

bool Runtime::check_number(string arg)
{
    if (arg[0] == '-')
        return (isdigit(arg[1]));
    else
        return (isdigit(arg[0]));
}

int Runtime::count_words(string line)
{
    stringstream s(line);
    string word;
    int count = 0;
    while (s >> word)
        count++;
    return count;
}

double Runtime::evaluate_expression(double val1, double val2, string exp_operator)
{
    if (exp_operator.compare("+") == 0)
        return val1 + val2;
    else if (exp_operator.compare("-") == 0)
        return val1 - val2;
    else if (exp_operator.compare("/") == 0)
        return val1 / val2;
    else if (exp_operator.compare("*") == 0)
        return val1 * val2;
    else if (exp_operator.compare("%") == 0)
        return double(int(val1) % int(val2));
    else 
        return double(int(val1) ^ int(val2));
}

void Runtime::handle_expression(string line)
{
    stringstream s(line);
    string curr;
    string exp_operator;
    double val1;
    double val2;
    double result;
    s >> curr;
    s >> curr;
    if (check_number(curr))
        val1 = stod(curr);
    else
    {
        // get the first value for the expression
        VNode* result = symbol_table->lookup(curr);
        if (result == NULL)
        {
            cout << curr << " IS UNDEFINED" << endl;
            return;
        }
        val1 = result->_value;
    }
    s >> exp_operator;
    s >> curr;
    if(check_number(curr))
        val2 = stod(curr);
    else
    {
        // get the second value for the expression
        VNode* result = symbol_table->lookup(curr);
        if (result == NULL)
        {
            cout << curr << " IS UNDEFINED" << endl;
            return;
        }
        val2 = result->_value;
    }
    result = evaluate_expression(val1, val2, exp_operator);
    stringstream again(line);
    string final_result = "";
    // get rid of print again and use string stream to get the rest of the expression for output
    again >> curr;
    again >> curr;
    final_result += curr;
    while (again >> curr)
        final_result += " " + curr;
    // print the actual result
    cout << final_result << " IS " << result << endl;
}

void Runtime::handle_print(string line)
{
    int num_words = count_words(line);
    if (num_words == 2)
    {
        string curr;
        stringstream s(line);
        s >> curr;
        s >> curr;
        VNode* result = symbol_table->lookup(curr);
        if (result == NULL)
        {
            cout << curr << " IS UNDEFINED" << endl;
            return;
        }
        cout << curr << " IS " << result->_value << endl;
    }
    else
       handle_expression(line); 
}

void Runtime::interpret_line(string line)
{
    stringstream s(line);
    string first_word;
    s >> first_word;
    if (first_word.compare("COM") == 0)
    {
        #ifdef DEBUG
            cout << "[DEBUG]: COM statement found" << endl;
        #endif
        return;
    }
    else if(first_word.compare("START") == 0)
    {
        #ifdef DEBUG
            cout << "[DEBUG]: START statement found" << endl;
        #endif 
        handle_start();
        return;
    }
    else if(first_word.compare("VAR") == 0)
    {
        #ifdef DEBUG
            cout << "[DEBUG]: VAR statement found" << endl;
        #endif 
        handle_var(line); 
        return;
    }
    else if(first_word.compare("FINISH") == 0)
    {
        #ifdef DEBUG
            cout << "[DEBUG]: FINISH statement found" << endl;
        #endif 
        handle_finish();
        return;
    }
    else if(first_word.compare("PRINT") == 0)
    {
        #ifdef DEBUG
            cout << "[DEBUG]: PRINT statement found" << endl;
        #endif 
        handle_print(line);
        return;
    }
    else
    {
        // handle assignment expression and incremental operators
        // check if the first word contains an incremental operator
        if (first_word.find("+") != std::string::npos) 
        {
            #ifdef DEBUG
                cout << "[DEBUG]: INCREMENT OPERATOR found" << endl;
            #endif 
            string find = first_word.substr(0, first_word.find("+"));
            VNode* result = symbol_table->lookup(find);
            result->_value += 1;
            return;
        }
        if (first_word.find("-") != std::string::npos) 
        {
            #ifdef DEBUG
                cout << "[DEBUG]: DECREMENT OPERATOR found" << endl;
            #endif 
            string find = first_word.substr(0, first_word.find("-"));
            VNode* result = symbol_table->lookup(find);
            result->_value -= 1;
            return;
        }
        // check for assignment
        if (first_word.compare("") != 0 && symbol_table->lookup(first_word) == NULL) 
        {
            cout << first_word << " IS UNDEFINED" << endl;
            return;
        }
        if (first_word.compare("") != 0)
        {
            #ifdef DEBUG
                cout << "[DEBUG]: ASSIGNMENT found" << endl;
            #endif 
            VNode* modify = symbol_table->lookup(first_word);
            // get to the value to assign 
            double assign;
            s >> first_word;
            s >>  assign;
            modify->_value = assign;
        }
    }
}

void Runtime::interpret_file(string filename)
{
    string line;
    int num_words;
    ifstream io(filename);
    while (getline(io, line))
        interpret_line(line);
    io.close();
}

int main()
{
    Runtime* runtime = new Runtime();
    runtime->interpret_file("Program.borg");
    return 0;
}

/*PROGRAM.BORG SOURCE
COM VALID BORG COMMENT
COM ANOTHER COMMENT

COM THIS COM IS A NEWLINE AWAY

START
    VAR ABC = 25
    VAR DEF = 13
    VAR GHI = 17
    VAR JKL = 10
    START
        VAR ABC = 12
        PRINT GHI
        PRINT 2 * GHI
        PRINT ABC / JKL
        PRINT MNO
        VAR MNO = 15
        VAR QRS = 2
        VAR TUV = 15.5
        MNO++
        PRINT MNO
        MNO--
        PRINT MNO
        DEF = 26
        PRINT DEF
    FINISH
FINISH
*/

/* OUTPUT:
GHI IS 17
2 * GHI IS 34
ABC / JKL IS 1.2
MNO IS UNDEFINED
MNO IS 16
MNO IS 15
DEF IS 26
*/