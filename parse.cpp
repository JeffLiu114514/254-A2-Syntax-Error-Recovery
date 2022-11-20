/* Complete recursive descent parser for the calculator language.
   Builds on figure 2.16 in the text.  Prints a trace of productions
   predicted and tokens matched.  Does no error recovery: prints
   "syntax error" and dies on invalid input.
   Michael L. Scott, 2008-2022.
*/

#include <iostream>
#include <tuple>
#include <list>
#include <map>
#include <algorithm>
#include "scan.hpp"
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::tie;
using std::map;
using std::list;
using std::initializer_list;

/*
    several things to do today:
    1. write the contain function
    2. write the function for all parse steps
    3. error check functions
    4. 
*/
// enum token {t_read, t_write, t_id, t_gets, t_add, t_sub, t_mul, t_div, t_lparen, t_rparen, t_if, t_then, t_end, t_while, t_do, t_inum, t_rnum, t_neq, t_lt, t_gt, t_le, t_ge, t_trunc, t_real, t_int, t_float, t_semi, t_eof};
const char* names[] = {"read", "write", "id", "gets", "add", 
                       "sub", "mul", "div", "lparen", "rparen", "if", "then", 
                       "end", "while", "do", "inum", "rnum", "==", "<>", "<", ">", 
                       "<=", ">=", "trunc", "real", "int", "float", "semi", "eof"};

class parser {
    token next_token;
    string token_image;
    scanner s;
    map<string, bool> whether_epsilon;
    map<string, list<token>> FIRST;
    map<string, list<token>> FOLLOW;
    bool print_tree = true; // do not print tree when there is an error


    // We need to report the error instead of exist the program
    void error (string sym) {
        cout << "found syntax error at " << sym << " for the current token " << token_image << endl;
        print_tree = false;
    }


    template <class T, class I >
    bool contains(const std::list<T>& list, const I & val){
        return std::find(list.begin(), list.end(), val) != list.end();
    }

    void match (token expected) {
        if (next_token == expected) {
            // cout << "matched " << names[next_token];
            // if (next_token == t_id || next_token == t_inum || next_token == t_rnum)
            //     cout << ": " << token_image;
            // cout << endl;
            tie(next_token, token_image) = s.scan (); // if matched, scan next token
        }
        else{
            error ("match");
            // cout << "should be " << names[expected] << endl;
        }
    }

public:

    void build_eps(){ // use bool for epsilon checking
        whether_epsilon.insert({"P", false});
        whether_epsilon.insert({"SL", true});
        whether_epsilon.insert({"S", false});
        whether_epsilon.insert({"E", false});
        whether_epsilon.insert({"T", false});
        whether_epsilon.insert({"F", false});
        whether_epsilon.insert({"C", false});
        whether_epsilon.insert({"TP", true});
        whether_epsilon.insert({"TT", true});
        whether_epsilon.insert({"FT", true});
        whether_epsilon.insert({"RO", false});
        whether_epsilon.insert({"AO", false});
        whether_epsilon.insert({"MO", false});
    }

    void build_first(){
        FIRST.insert({"SL", {t_int, t_real, t_id, t_read, t_write, t_if, t_while}});
        FIRST.insert({"S", {t_int, t_real, t_id, t_read, t_write, t_if, t_while}});
        FIRST.insert({"TP", {t_int, t_real}});
        FIRST.insert({"TT", {t_add, t_sub}});
        FIRST.insert({"FT", {t_mul, t_div}});
        FIRST.insert({"F", {t_lparen, t_id, t_inum, t_rnum, t_trunc, t_float}});
        FIRST.insert({"RO", {t_eq, t_neq, t_lt, t_gt, t_le, t_gt}});
        FIRST.insert({"AO", {t_add, t_sub}});
        FIRST.insert({"MO", {t_mul, t_div}});
        FIRST.insert({"T", {t_lparen, t_id, t_inum, t_rnum, t_trunc, t_float}});
        FIRST.insert({"P", {t_int, t_real, t_id, t_read, t_write, t_if, t_while, t_eof}});
        FIRST.insert({"E", {t_lparen, t_id, t_inum, t_rnum, t_trunc, t_float}});
        FIRST.insert({"C", {t_lparen, t_id, t_inum, t_rnum, t_trunc, t_float}});
    }

    void build_follow(){
        FOLLOW.insert({"P", {t_eof}});
        FOLLOW.insert({"SL",  {t_end, t_eof}});
        FOLLOW.insert({"S",  {t_semi}});
        FOLLOW.insert({"TP",  {t_id}});
        FOLLOW.insert({"C",  {t_then, t_do}});

        FOLLOW.insert({"E",  {t_rparen, t_eq, t_neq, t_lt, t_gt, t_le, t_ge, t_then, t_do, t_semi}});
        
        // = FOLLOW(E)
        FOLLOW.insert({"TT",  {t_rparen, t_eq, t_neq, t_lt, t_gt, t_le, t_ge, t_then, t_do, t_semi}});

        // FIRST(TT) - epsilon union FOLLOW(TT)
        FOLLOW.insert({"T",  {t_add, t_sub, t_rparen, t_eq, t_neq, t_lt, t_gt, t_le, t_ge, t_then, t_do, t_semi}});

        // = FOLLOW(T)
        FOLLOW.insert({"FT",  {t_add, t_sub, t_rparen, t_eq, t_neq, t_lt, t_gt, t_le, t_ge, t_then, t_do, t_semi}});

        // FIRST(FT) - epsilon union FOLLOW(FT)
        FOLLOW.insert({"F",  {t_mul, t_div, t_add, t_sub, t_rparen, t_eq, t_neq, t_lt, t_gt, t_le, t_ge, t_then, t_do, t_semi}});

        // = FIRST of (E)
        FOLLOW.insert({"RO",  {t_lparen, t_id, t_inum, t_rnum, t_trunc, t_float}});

        // = FIRST(T)
        FOLLOW.insert({"AO",  {t_lparen, t_id, t_inum, t_rnum, t_trunc, t_float}});

        // = FIRST(F)
        FOLLOW.insert({"MO",  {t_lparen, t_id, t_inum, t_rnum, t_trunc, t_float}});
    }

    //Implementation of Wirths algorithm
    void check_for_error(string sym)
    {
        bool eps = whether_epsilon[sym];
        list<token> first = FIRST[sym];
        list<token> follow = FOLLOW[sym];

        if (!(contains(first, next_token) || (contains(follow, next_token) && eps))) // immediate error detection
        {
            error(sym);
            do{
                tie(next_token, token_image) = s.scan();
            }
            while(!(contains(first, next_token) ||
                    contains(follow, next_token) ||
                    next_token == t_eof));
        }
    }

    parser() {
        tie(next_token, token_image) = s.scan ();
    }

    string program () {
        string current_str = "";
        check_for_error("P");
        switch (next_token) {
            case t_int:
            case t_real:
            case t_id:
            case t_read:
            case t_write:
            case t_if:
            case t_while:
            case t_eof:
                // cout << "predict program --> stmt_list eof" << endl;
                // predict P -> SL $$
                current_str = "[ " + stmt_list() + " ]";
                stmt_list ();
                match (t_eof);
                break;
            default: error("P");
        }
        if (print_tree == false){ // do not print tree when there is an error
            return "";
        }
        return current_str;
    }

private:
    string stmt_list () {
        string current_str = "";
        check_for_error("SL");
        switch (next_token) {
            case t_int:
            case t_real:
            case t_id:
            case t_read:
            case t_write:
            case t_if:
            case t_while:
                // cout << "predict stmt_list --> stmt stmt_list" << endl;
                //predict SL --> S ; SL
                current_str = current_str + stmt();
                match (t_semi);
                current_str = current_str + stmt_list();
                break;
            case t_end:
            case t_eof:
                // cout << "predict stmt_list --> epsilon" << endl;
                break;          // epsilon production
            default: error ("SL");
        }
        return current_str;
    }

    string stmt () {
        string current_str = "";
        check_for_error("S");
        switch (next_token) {
            case t_int:
                // predict S --> int id := E
                match (t_int);
                current_str = current_str + "(int" + " \"" + token_image + "\"" + ")\n";
                current_str = current_str + "(:= " + "\"" + token_image + "\"";
                match (t_id);
                match (t_gets);
                current_str = current_str + expr() + ")\n";
                break;
            case t_real:
                // predict S --> real id := E
                match (t_real);
                match (t_id);
                match (t_gets);
                current_str = current_str + "(real " + token_image + " " + expr() + ")\n";
                break;
            case t_id:
                // cout << "predict stmt --> type id = expr" << endl;
                // predict S --> id ：= E
                current_str = current_str + "(:= \"" + token_image + "\"";
                match (t_id);
                match (t_gets);
                current_str = current_str + expr() + ")";
                break;
            case t_read:
                // predict S -->  read TP id
                // cout << "predict stmt --> read id" << endl;
                match (t_read);
                current_str = current_str + TP();
                current_str = current_str + " \"" + token_image + "\"" + ")\n";
                current_str = current_str + "(read";
                current_str = current_str + " \"" + token_image + "\")" + "\n";
                match (t_id);
                // cout << current_str << endl;
                break;
            case t_write:
                // predict S --> write E
                // cout << "predict stmt --> write expr" << endl;
                match (t_write);
                current_str = current_str + "(write" + expr () + ")";
                break;
            case t_if:
                // predict S --> if C then SL end
                // cout << "predict stmt --> if expr then stmt_list end" << endl;
                match (t_if);
                current_str = current_str + "(if (" + C() + ")\n";
                match (t_then);
                current_str = current_str + "[" + stmt_list();
                match (t_end);
                current_str = current_str + "\n ])";
                break;
            case t_while:
                // predict S --> while C do SL end
                // cout << "predict stmt --> while expr do stmt_list end" << endl;
                match (t_while);
                current_str = current_str + "(" + "while (" + C() + ")\n";
                match (t_do);
                current_str = current_str + "[ " + stmt_list();
                match (t_end);
                current_str = current_str + "\n ])";
                break;
            case t_semi:
                // cout << "predict stmt --> epsilon" << endl;
                break;          // epsilon production
            default: error ("S");
        }
        return current_str;
    }

    string expr () {
        string current_str = "";
        check_for_error("E");
        switch (next_token) {
            case t_id:
            case t_inum:
            case t_rnum:
            case t_lparen:
            case t_trunc:
            case t_float:
                // cout << "predict expr --> term term_tail" << endl;
                current_str = current_str + term_tail (term());
                break;
            // t_rparen, t_eq, t_neq, t_lt, t_gt, t_le, t_ge, t_then, t_do, t_semi
            case t_rparen:
            case t_eq:
            case t_neq:
            case t_lt:
            case t_gt:
            case t_le:
            case t_ge:
            case t_then:
            case t_do:
            case t_semi:
            case t_eof:
                // cout << "predict expr --> epsilon" << endl;
                break;          // epsilon production
            default: error ("E");
        }
        return current_str;
    }

    string term_tail (string inner_str) { // inner_str from term
        // t_rparen, t_eq, t_neq, t_lt, t_gt, t_le, t_gt, t_then, t_do, t_semi
        string current_str = "";
        check_for_error("TT");
        switch (next_token) {
            case t_add:
            case t_sub:
                // cout << "predict term_tail --> add_op term term_tail" << endl;
                //predict TT --> ao T TT
                current_str = current_str + " (" + add_op();
                current_str = current_str + inner_str;
                current_str = current_str + term_tail(term()) + ")";
                break;
            case t_rparen:
            case t_eq:
            case t_neq:
            case t_lt:
            case t_gt:
            case t_le:
            case t_ge:
            case t_then:
            case t_do:
            case t_semi:
            case t_eof:
                // cout << "predict term_tail --> epsilon" << endl;
                current_str = current_str + inner_str;
                break;          // epsilon production
            default: error ("TT");
        }
        return current_str;
    }

    string term () {
        string current_str = "";
        check_for_error("T");
        switch (next_token) {
            case t_id:
            case t_inum:
            case t_rnum:
            case t_lparen:
            case t_trunc:
            case t_float:
                // cout << "predict term --> factor factor_tail" << endl;
                current_str = factor_tail (factor ());
                break;
            // t_add, t_sub, t_rparen, t_eq, t_neq, t_lt, t_gt, t_le, t_ge, t_then, t_do, t_semi
            case t_add:
            case t_sub:
            case t_rparen:
            case t_eq:
            case t_neq:
            case t_lt:
            case t_gt:
            case t_le:
            case t_ge:
            case t_then:
            case t_do:
            case t_semi:
            case t_eof:
                // cout << "predict term --> epsilon" << endl;
                break;          // epsilon production
            default: error ("T");
        }
        return current_str;
    }

    string factor_tail (string inner_str) { // inner_str from factor
        string current_str = "";
        check_for_error("FT");
        switch (next_token) {
            case t_mul:
            case t_div:
                // cout << "predict factor_tail --> mul_op factor factor_tail"
                    //  << endl;
                //predict FT --> mo F FT
                current_str = current_str + " (" + mul_op();
                current_str = current_str + inner_str;
                current_str = current_str + factor_tail(factor()) + ")";
                break;
            // t_add, t_sub, t_rparen, t_eq, t_neq, t_lt, t_gt, t_le, t_ge, t_then, t_do, t_semi
            case t_add:
            case t_sub:
            case t_rparen:
            case t_eq:
            case t_neq:
            case t_lt:
            case t_gt:
            case t_le:
            case t_ge:
            case t_then:
            case t_do:
            case t_semi:
            case t_eof:
                // cout << "predict factor_tail --> epsilon" << endl;
                return inner_str;
                break;          // epsilon production
            default: error ("FT");
        }
        return current_str;
    }

    string factor () {

        string current_str = "";
        check_for_error("F");
        switch (next_token) {
            case t_inum:
                // cout << "predict factor --> inum" << endl;
                current_str = current_str + " \"" + token_image + "\"";
                match (t_inum);
                break;
            case t_rnum:
                // cout << "predict factor --> rnum" << endl;
                current_str = current_str + " \"" + token_image + "\"";
                match (t_rnum);
                break;
            case t_id :
                // cout << "predict factor --> id" << endl;
                current_str = current_str + " \"" + token_image + "\"";
                match (t_id);
                break;
            case t_lparen:
                // cout << "predict factor --> lparen expr rparen" << endl;
                match (t_lparen);
                current_str = current_str + expr ();
                match (t_rparen);
                break;
            case t_trunc:
                // cout << "predict factor --> trunc lparen expr rparen" << endl;
                match (t_trunc);
                match (t_lparen);
                current_str = current_str + expr ();
                match (t_rparen);
                break;
            case t_float:
                // cout << "predict factor --> float lparen expr rparen" << endl;
                match (t_float);
                match (t_lparen);
                current_str = current_str + expr ();
                match (t_rparen);
                break;
            // t_mul, t_div, t_add, t_sub, t_rparen, t_eq, t_neq, t_lt, t_gt, t_le, t_ge, t_then, t_do, t_semi
            case t_mul:
            case t_div:
            case t_add:
            case t_sub:
            case t_rparen:
            case t_eq:
            case t_neq:
            case t_lt:
            case t_gt:
            case t_le:
            case t_ge:
            case t_then:
            case t_do:
            case t_semi:
            case t_eof:
                // cout << "predict factor --> epsilon" << endl;
                break;          // epsilon production
            default: error ("F");
        }
        return current_str;
    }

    string add_op () {
        string current_str = "";
        check_for_error("AO");
        switch (next_token) {
            case t_add:
                // cout << "predict add_op --> add" << endl;
                match (t_add);
                current_str = current_str + "+";
                break;
            case t_sub:
                // cout << "predict add_op --> sub" << endl;
                match (t_sub);
                current_str = current_str + "-";
                break;
            // t_lparen, t_id, t_inum, t_rnum
            case t_lparen:
            case t_id:
            case t_inum:
            case t_rnum:
            case t_trunc:
            case t_float:
                // cout << "predict add_op --> epsilon" << endl;
                break;          // epsilon production
            default: error ("AO");
        }
        return current_str;
    }

    string mul_op () {
        string current_str = "";
        check_for_error("MO");
        switch (next_token) {
            case t_mul:
                // cout << "predict mul_op --> mul" << endl;
                match (t_mul);
                current_str = current_str + "*";
                break;
            case t_div:
                // cout << "predict mul_op --> div" << endl;
                match (t_div);
                current_str = current_str + "/";
                break;
            // t_lparen, t_id, t_inum, t_rnum
            case t_lparen:
            case t_id:
            case t_inum:
            case t_rnum:
            case t_trunc:
            case t_float:
                // cout << "predict mul_op --> epsilon" << endl;
                break;          // epsilon production
            default: error ("MO");
        }
        return current_str;
    }

    string C(){
        string current_str = "";
        string temp = "";
        check_for_error("C");
        switch (next_token) {
            case t_id:
            case t_inum:
            case t_rnum:
            case t_lparen:
            case t_trunc:
            case t_float:
                // cout << "predict C --> E relop E" << endl;
                temp = expr();
                current_str = current_str + RO();
                current_str = current_str + temp;
                current_str = current_str + expr();
                break;
            // t_then, t_do
            case t_then:
            case t_do:
            case t_eof:
                // cout << "predict C --> epsilon" << endl;
                break;          // epsilon production
            default: error ("C");
        }
        return current_str;
    }

    string TP(){
        string current_str = "";
        check_for_error("TP");
        switch (next_token) {
            case t_int:
                // cout << "predict TP --> type integer" << endl;
                match(t_int);
                current_str = current_str + "(" + "int";
                break;
            
            case t_real:
                // cout << "predict TP --> type real" << endl;
                match(t_real);
                break;
            // t_id
            case t_id:
            case t_eof:
                // cout << "predict TP --> epsilon" << endl;
                break;          // epsilon production
            default: error ("TP");
        }
        return current_str;
    }

    string RO(){
        string current_str = "";
        // cout << token_image << endl;
        check_for_error("RO");
        switch (next_token) {
            case t_eq:
                // cout << "predict relop --> eq" << endl;
                match (t_eq);
                current_str = current_str + "==";
                break;
            case t_neq:
                // cout << "predict relop --> neq" << endl;
                match (t_neq);
                current_str = current_str + "<>";
                break;
            case t_lt:
                // cout << "predict relop --> lt" << endl;
                match (t_lt);
                current_str = current_str + "<";
                break;
            case t_gt:
                // cout << "predict relop --> gt" << endl;
                match (t_gt);
                current_str = current_str + ">";
                break;
            case t_le:
                // cout << "predict relop --> le" << endl;
                match (t_le);
                current_str = current_str + "<=";
                break;
            case t_ge:
                // cout << "predict relop --> ge" << endl;
                match (t_ge);
                current_str = current_str + ">=";
                break;
            // t_lparen, t_id, t_inum, t_rnum
            case t_lparen:
            case t_id:
            case t_inum:
            case t_rnum:
            case t_trunc:
            case t_float:
            case t_eof:
                // cout << "predict relop --> epsilon" << endl;
                break;          // epsilon production
            default: error ("RO");
        }
        return current_str;
    }

};

int main () {
    
    parser p;
    p.build_eps();
    p.build_first();
    p.build_follow();
    string answer = p.program (); //AST tree
    cout << answer << endl;
    return 0;
}