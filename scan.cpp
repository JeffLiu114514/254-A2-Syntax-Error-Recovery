/* Simple ad-hoc scanner for the calculator language.
   Dies on lexical errors.
   Michael L. Scott, 2008-2022.
*/

#include <iostream>
#include <cctype>   // isalpha, isspace, isdigit
#include <tuple>
using std::cerr;
using std::cin;
using std::cout;
using std::hex;
using std::endl;
using std::string;
using std::make_tuple;

#include "scan.hpp"

tuple<token, string> scanner::scan() {
    string token_image;

    // skip white space
    while (isspace(c)) {
        c = cin.get();
    }
    if (c == EOF)
        return make_tuple(t_eof, "eof");
    if (isalpha(c)) { 
        do { // variable name
            token_image += c;
            c = cin.get();
        } while (isalpha(c) || isdigit(c) || c == '_');
        if (token_image == "read") return make_tuple(t_read, "read");
        else if (token_image == "write") return make_tuple(t_write, "write");
        else if (token_image == "trunc") return make_tuple(t_trunc, "trunc");
        else if (token_image == "float") return make_tuple(t_float, "float");
        else if (token_image == "while") return make_tuple(t_while, "while");
        else if (token_image == "int") return make_tuple(t_int, "int");
        else if (token_image == "i_num") return make_tuple(t_inum, "i_num");
        else if (token_image == "r_num") return make_tuple(t_rnum, "r_num");
        else if (token_image == "real") return make_tuple(t_real, "real");
        else if (token_image == "do") return make_tuple(t_do, "do");
        else if (token_image == "end") return make_tuple(t_end, "end");
        else if (token_image == "then") return make_tuple(t_then, "then");
        else if (token_image == "if") return make_tuple(t_if, "if");
        else return make_tuple(t_id, token_image);
    }

    // i_num  =  d+
    // r_num  =  ( d+ . d* | d* . d+ ) ( e ( + | - | ε ) d+ | ε )
    else if (isdigit(c)){
        do { // d+ . d*
            token_image += c;
            c = cin.get();
        } while (isdigit(c));
        if (c == '.') { 
            token_image += c;
            c = cin.get();
            if (isdigit(c)) {
                do {
                    token_image += c;
                    c = cin.get();
                } while (isdigit(c));
                if (c == 'e') { // ( e ( + | - | ε ) d+ | ε )
                    token_image += c;
                    c = cin.get();
                    if (c == '+' || c == '-') {
                        token_image += c;
                        c = cin.get();
                    }
                    if (isdigit(c)) {
                        do {
                            token_image += c;
                            c = cin.get();
                        } while (isdigit(c));
                        return make_tuple(t_rnum, token_image);
                    }
                    else {
                        cout << "Error: invalid real number: " << token_image << endl;
                    }
                }
                else {
                    return make_tuple(t_rnum, token_image);
                }
            }
            else {
                cout << "Error: invalid real number: " << token_image << endl;
            }
        }
        else if (c == 'e') {
            token_image += c;
            c = cin.get();
            if (c == '+' || c == '-') {
                token_image += c;
                c = cin.get();
            }
            if (isdigit(c)) {
                do {
                    token_image += c;
                    c = cin.get();
                } while (isdigit(c));
                return make_tuple(t_rnum, token_image);
            }
            else {
                cout << "Error: invalid real number: " << token_image << endl;
            }
        }
        else {
            return make_tuple(t_inum, token_image);
        }
    }
    else if (c == '.') {// d* . d+
            token_image += c;
            c = cin.get();
            if (isdigit(c)) {
                do {
                    token_image += c;
                    c = cin.get();
                } while (isdigit(c));
                if (c == 'e') { // ( e ( + | - | ε ) d+ | ε )
                    token_image += c;
                    c = cin.get();
                    if (c == '+' || c == '-') {
                        token_image += c;
                        c = cin.get();
                    }
                    if (isdigit(c)) {
                        do {
                            token_image += c;
                            c = cin.get();
                        } while (isdigit(c));
                        return make_tuple(t_rnum, token_image);
                    }
                    else {
                        cout << "Error: invalid real number: " << token_image << endl;
                    }
                }
                else {
                    return make_tuple(t_rnum, token_image);
                }
            }
            else {
                cout << "Error: invalid real number: " << token_image << endl;
            }
        }
    else switch (c) {
        case ':': 
            c = cin.get();
            if (c != '=') {// must have '=' after ':'
                cout << "expected '=' after ':', got '"
                     << c << "' (0x" << hex << c << ")\n";
                cin.get();
            } else {
                c = cin.get();
                return make_tuple(t_gets, ":=");
            }
            break;
        case '+': c = cin.get(); return make_tuple(t_add, "+");
        case '-': c = cin.get(); return make_tuple(t_sub, "-");
        case '*': c = cin.get(); return make_tuple(t_mul, "*");
        case '/': c = cin.get(); return make_tuple(t_div, "/");
        case '(': c = cin.get(); return make_tuple(t_lparen, "(");
        case ')': c = cin.get(); return make_tuple(t_rparen, ")");
        case '<': 
            c = cin.get();
            if(c == '='){
				c = cin.get();
				return make_tuple(t_le, "<=");
			}
            else if(c == '>'){
                c = cin.get();
				return make_tuple(t_neq, "<>");
            }
            else{
				return make_tuple(t_lt, "<");
			}
        case '>':
            c  = cin.get();
            if(c == '='){
				c = std::cin.get();
				return make_tuple(t_ge, ">=");
			}
            else{
				return make_tuple(t_gt, ">");
            }
        case '=':
            c = cin.get();
            if (c != '=') { // must have '=' after '='
                cout << "expected '=' after ':', got '"
                     << c << "' (0x" << hex << c << ")\n";
                c = cin.get();
            } else {
                c = cin.get();
                return make_tuple(t_eq, "==");
            }
            break;
        case ';':
            c = cin.get();
            return make_tuple(t_semi, ";");
        default:
            cout << "unexpected character '"  << c << "' (0x" << hex << c << ")\n";
            c = cin.get();
    }
} // scan
