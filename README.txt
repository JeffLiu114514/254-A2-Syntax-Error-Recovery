# CSC254Project2


---------------------------------------------------------
Completed by: Junfei Liu (jliu137) & Pinxin Liu (pliu23)
---------------------------------------------------------

Hi grader,

--------------------- How to run ---------------------------------

Compilation and run instructions are:
You need to go the CSC254_project2 directory and type make in commandline for compilation.
After that, you can run the program by typing the following command:
./parse < [filename]
where filename is the file containing calculator language codes. You should be able to
feed standard input to parse in any eligible way.

We provide the following test files:
correct --- contains the tree on the A2 website
test --- contains the tree with wrong syntax

To run the test files, you can use the following commands:
./parse < correct
./parse < test


--------------------- work we have done ------------------------------

### 1. Lexical error recovery
- In any step we run into a lexical error, we follow the instructions according to the A2 webpage. 
  Specifically, we (1) throw away the current, invalid token and directly get the next one; 
                   (2) immediately after a sequence of characters that comprise a complete, valid token—in which case it can simply return that token; 
                   (3) read the ":=", "<>", "<=", ">=", "==" and the values for i_nim and r_num; 

### 2. Extend the calculator tokens
We noticed that that are no literal tokens in the calculator language but i_num and r_num, 
so we replaced the literal tokens with i_num and r_num. We also added the tokens as 
mentioend in Trivial2

### 3. Syntactic error recovery
- We build FIRST set, FOLLOW set and ESP for each non-terminal symbol. ESP is to determine whether the current token can go to epsilon or not
- We implement the parser error recovery mechanism of Niklaus Wirth
Specifically, we have constructed the check_for_error function. If the parsing routine for nonterminal discovers an error at the beginning of its code,
it deletes (skip) incoming tokens until it finds a member of FIRST(nonterminal), in which case it proceeds, or a member of FOLLOW(nonterminal), in which case it returns.

### 4. Syntax tree output
- We utilize the recurrent descent parser program to grow the tree according to the grammar.
- We do so by recursively growing the syntax tree within each subroutine, stored in a string under the linear, parenthesized form.
- Upon seeing an error, we would output syntax error messages.

### 5. immediate error checking
- We are undergraduate students and we have done the immediate error detection.
- We added follow set to the if condition in the original check_for_error to check the case where the input token is invalid but the current nonterminal can go to epsilon. 

Our code is very loyal to the assignment description and pseudocode on lecture notes.
You can find corresponding code to 1. lexical errors in scan.cpp, to 2. extended language
with all first set, follow set, and grammar in parse.cpp, to 3. error recovery with a 
method called check_for_error in parse.cpp which is basically the implementation of 
pseudocode on lecture note 2, to 4. AST generation by running parse.

Our sample output on test file "correct" is in output.txt run by
make
./parse < correct