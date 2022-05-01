/* Implementation of PLSL Interpreter
 * parseInt.cpp
 * Programming Assignment 3
 * Spring 2022
*/

#include <vector>
#include <string>
#include <cmath>
#include "parseInt.h"
#include "val.h"

//#include "lex.cpp"
using namespace std;
map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects

namespace Parser {
    bool pushed_back = false;
    LexItem	pushed_token;

    static LexItem GetNextToken(istream& in, int& line) {
        if( pushed_back ) {
            pushed_back = false;
            return pushed_token;
        }
        return getNextToken(in, line);
    }

    static void PushBackToken(LexItem & t) {
        if( pushed_back ) {
            abort();
        }
        pushed_back = true;
        pushed_token = t;
    }

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
    ++error_count;
    cout << line << ": " << msg << endl;
}

//Program is: Prog = PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line)
{
    bool f1, f2;
    LexItem tok = Parser::GetNextToken(in, line);

    if (tok.GetToken() == PROGRAM) {
        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == IDENT) {

            tok = Parser::GetNextToken(in, line);
            if (tok.GetToken() == SEMICOL) {
                f1 = DeclBlock(in, line);

                if(f1) {
                    f2 = ProgBody(in, line);
                    if(!f2)
                    {
                        ParseError(line, "Incorrect Program Body.");
                        return false;
                    }

                    return true;//Successful Parsing is completed
                }
                else
                {
                    ParseError(line, "Incorrect Declaration Section.");
                    return false;
                }
            }
            else
            {
                //Parser::PushBackToken(tok);
                ParseError(line-1, "Missing Semicolon.");
                return false;
            }
        }
        else
        {
            ParseError(line, "Missing Program Name.");
            return false;
        }
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else if(tok.GetToken() == DONE && tok.GetLinenum() <= 1){
        ParseError(line, "Empty File");
        return true;
    }
    ParseError(line, "Missing PROGRAM.");
    return false;
}

bool ProgBody(istream& in, int& line) {
    bool status;

    LexItem tok = Parser::GetNextToken(in, line);

    if (tok.GetToken() == BEGIN) {

        status = Stmt(in, line);

        while(status) {
            tok = Parser::GetNextToken(in, line);
            if(tok != SEMICOL) {
                line--;
                ParseError(line, "Missing semicolon in Statement.");
                return false;
            }

            status = Stmt(in, line);
        }

        tok = Parser::GetNextToken(in, line);
        if(tok == END )
        {
            return true;
        }
        else
        {
            ParseError(line, "Syntactic error in Program Body.");
            return false;
        }
    }
    else
    {
        ParseError(line, "Non-recognizable Program Body.");
        return false;
    }
}//End of ProgBody function

bool DeclBlock(istream& in, int& line) {
    bool status = false;
    LexItem tok;

    LexItem t = Parser::GetNextToken(in, line);
    if(t == VAR)
    {
        status = DeclStmt(in, line);

        while(status)
        {
            tok = Parser::GetNextToken(in, line);
            if(tok != SEMICOL)
            {
                line--;
                ParseError(line, "Missing semicolon in Declaration Statement.");
                return false;
            }
            status = DeclStmt(in, line);
        }

        tok = Parser::GetNextToken(in, line);
        if(tok == BEGIN )
        {
            Parser::PushBackToken(tok);
            return true;
        }
        else
        {
            ParseError(line, "Syntactic error in Declaration Block.");
            return false;
        }
    }
    else
    {
        ParseError(line, "Non-recognizable Declaration Block.");
        return false;
    }

}//end of DeclBlock function

//IdList:= IDENT {,IDENT}
bool IdentList(istream& in, int& line, vector<string> &IdList) {
    bool status = false;
    string identstr;

    LexItem tok = Parser::GetNextToken(in, line);
    if(tok == IDENT)
    {
        //set IDENT lexeme to the type tok value
        identstr = tok.GetLexeme();
        if (!(defVar.find(identstr)->second))
        {
            defVar[identstr] = true;
            IdList.push_back(identstr);
        }
        else
        {
            ParseError(line, "Variable Redefinition");
            return false;
        }

    } else {
        Parser::PushBackToken(tok);
        return true;
    }

    tok = Parser::GetNextToken(in, line);

    if (tok == COMMA) {
        status = IdentList(in, line, IdList);
    }
    else if(tok == COLON)
    {
        Parser::PushBackToken(tok);
        return true;
    }
    else {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    return status;

}//End of IdentList

bool DeclStmt(istream& in, int& line) {
    LexItem t;
    vector<string> idList;

    bool status = IdentList(in, line, idList);

    if (!status) {
        ParseError(line, "Incorrect variable in Declaration Statement.");
        return status;
    }

    t = Parser::GetNextToken(in, line);
    if(t == COLON)
    {
        t = Parser::GetNextToken(in, line);
        if(t == INTEGER || t == REAL || t == STRING)
        {
            for (string id : idList) {
                SymTable[id] = {t.GetToken()};
            }
            return true;
        }
        else
        {
            ParseError(line, "Incorrect Declaration Type.");
            return false;
        }
    }
    else
    {
        Parser::PushBackToken(t);
        return false;
    }

}//End of DeclStmt

//Stmt is either a WriteLnStmt, ForepeatStmt, IfStmt, or AssigStmt
//Stmt = AssigStmt | IfStmt | WriteStmt | ForStmt
bool Stmt(istream& in, int& line) {
    bool status;

    LexItem t = Parser::GetNextToken(in, line);

    switch( t.GetToken() ) {

        case WRITELN:
            status = WriteLnStmt(in, line);
            break;

        case IF:
            status = IfStmt(in, line);
            break;

        case IDENT:
            Parser::PushBackToken(t);
            status = AssignStmt(in, line);
            break;

        default:
            Parser::PushBackToken(t);
            return false;
    }

    return status;
}//End of Stmt

//WriteStmt:= wi, ExpreList
bool WriteLnStmt(istream& in, int& line) {
    LexItem t;
    ValQue = new queue<Value>;

    t = Parser::GetNextToken(in, line);
    if( t != LPAREN ) {

        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    bool ex = ExprList(in, line);

    if( !ex ) {
        ParseError(line, "Missing expression after WriteLn");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t != RPAREN ) {

        ParseError(line, "Missing Right Parenthesis");
        return false;
    }


    //Evaluate: print out the list of expressions' values
    while (!(*ValQue).empty())
    {
        Value nextVal = (*ValQue).front();
        cout << nextVal;
        ValQue->pop();
    }
    cout << endl;

    return ex;
}//End of WriteLnStmt

//IfStmt:= if (Expr) then Stm} [Else Stmt]
bool IfStmt(istream& in, int& line) {
    bool ex=false, status;
    LexItem t;

    t = Parser::GetNextToken(in, line);
    if( t != LPAREN ) {

        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    Value expressionEval;
    ex = LogicExpr(in, line, expressionEval);

    if( !ex ) {
        ParseError(line, "Missing if statement Logic Expression");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t != RPAREN ) {

        ParseError(line, "Missing Right Parenthesis");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t != THEN)
    {
        ParseError(line, "If-Stmt Syntax Error");
        return false;
    }
    status = Stmt(in, line);
    if(!status)
    {
        ParseError(line, "Missing Statement for If-Stmt Then-Part");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if( t == ELSE ) {
        status = Stmt(in, line);
        if(!status)
        {
            ParseError(line, "Missing Statement for If-Stmt Else-Part");
            return false;
        }
        //cout << "in IFStmt status of Stmt true" << endl;
        return true;
    }

    Parser::PushBackToken(t);
    return true;
}//End of IfStmt function

//Var:= ident
bool Var(istream& in, int& line, LexItem& idtok) {
    //called only from the AssignStmt function
    string identstr;

    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == IDENT) {
        identstr = tok.GetLexeme();

        if (!(defVar.find(identstr)->second)) {
            ParseError(line, "Undeclared Variable");
            return false;
        }
        idtok = tok;
        return true;

    } else if(tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    return false;
}//End of Var

//AssignStmt:= Var = Expr
bool AssignStmt(istream& in, int& line) {

    bool varstatus = false, status = false;
    LexItem t;

    LexItem variableRef;
    varstatus = Var(in, line, variableRef);

    if (varstatus) {
        t = Parser::GetNextToken(in, line);

        if (t == ASSOP) {
            Value assignedValue;
            status = Expr(in, line, assignedValue);
            if (!status) {
                ParseError(line, "Missing Expression in Assignment Statment");
                return status;
            } else {
                //DYNAMIC TYPE CONVERSION
                if (SymTable.find(variableRef.GetLexeme())->second == INTEGER) {
                    if (assignedValue.IsInt()) {
                        TempsResults.insert({variableRef.GetLexeme(), assignedValue});
                    } else if (assignedValue.IsReal()) {
                        int value = assignedValue.GetReal();
                        TempsResults.insert({variableRef.GetLexeme(), Value(value)});
                    } else {
                        ParseError(line, "Int cannot be assigned this type");
                        return false;
                    }
                } else if (SymTable.find(variableRef.GetLexeme())->second == REAL) {
                    if (assignedValue.IsReal()) {
                        TempsResults.insert({variableRef.GetLexeme(), assignedValue});
                    } else if (assignedValue.IsInt()) {
                        float value = assignedValue.GetInt();
                        TempsResults.insert({variableRef.GetLexeme(), Value(value)});
                    } else {
                        ParseError(line, "Real cannot be assigned this type");
                        return false;
                    }
                } else {
                    TempsResults.insert({variableRef.GetLexeme(), assignedValue});
                }
            }
        } else if (t.GetToken() == ERR) {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << t.GetLexeme() << ")" << endl;
            return false;
        } else {
            ParseError(line, "Missing Assignment Operator");
            return false;
        }
    } else {
        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
        return false;
    }
    return status;
}

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
    bool status = false;

    Value aValue;
    status = Expr(in, line, aValue);

    ValQue->push(aValue);

    if(!status){
        ParseError(line, "Missing Expression");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == COMMA) {
        status = ExprList(in, line);
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else{
        Parser::PushBackToken(tok);
        return true;
    }
    return status;
}

//Expr:= Term {(+|-) Term}
bool Expr(istream& in, int& line, Value& retVal) {
    Value val1, val2;

    bool t1 = Term(in, line, val1);
    LexItem tok;

    if( !t1 ) {
        return false;
    }
    retVal = val1;

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    //Evaluate: evaluate the expression for addition or subtraction
    while ( tok == PLUS || tok == MINUS ) {
        t1 = Term(in, line, val2);
        if( !t1 )
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        if(tok == PLUS)
        {
            retVal = retVal + val2;
            if(retVal.IsErr())
            {
                ParseError(line, "Illegal addition operation.");
                //cout << "(" << tok.GetLexeme() << ")" << endl;
                return false;
            }
        }
        else if(tok == MINUS)
        {
            retVal = retVal - val2;
            if(retVal.IsErr())
            {
                ParseError(line, "Illegal subtraction operation.");
                return false;
            }
        }

        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }


    }
    Parser::PushBackToken(tok);
    return true;
}//end of Expr

//Term:= SFactor {(*|/) SFactor}
bool Term(istream& in, int& line, Value& retVal) {
    Value val1, val2;

    bool t1 = SFactor(in, line, val1);
    LexItem tok;

    if( !t1 ) {
        return false;
    }
    retVal = val1;

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    //Evaluate: evaluate the expression for addition or subtraction
    while ( tok == MULT || tok == DIV ) {
        t1 = SFactor(in, line, val2);
        if( !t1 )
        {
            ParseError(line, "Missing operand after operator");
            return false;
        }

        if(tok == MULT)
        {
            retVal = retVal * val2;
            if(retVal.IsErr())
            {
                ParseError(line, "Illegal Multiplication operation.");
                //cout << "(" << tok.GetLexeme() << ")" << endl;
                return false;
            }
        }
        else if(tok == DIV)
        {
            retVal = retVal / val2;
            if(retVal.IsErr())
            {
                ParseError(line, "Illegal division operation.");
                return false;
            }
            if (isinf(retVal.GetReal())) {
                ParseError(line, "Run-Time Error-Illegal Division by Zero");
                return false;
            }
        }

        tok = Parser::GetNextToken(in, line);
        if(tok.GetToken() == ERR){
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }
    Parser::PushBackToken(tok);
    return true;
}

//SFactor = Sign Factor | Factor
bool SFactor(istream& in, int& line, Value & retVal) {
    LexItem t = Parser::GetNextToken(in, line);

    bool status;
    int sign = 0;
    if(t == MINUS )
    {
        sign = -1;
    }
    else if(t == PLUS)
    {
        sign = 1;
    }
    else
        Parser::PushBackToken(t);

    status = Factor(in, line, sign, retVal);
    return status;
}

//LogicExpr = Expr (== | <) Expr
bool LogicExpr(istream& in, int& line, Value & retVal)
{
    Value leftHandSide, rightHandSide;
    bool t1 = Expr(in, line, leftHandSide);
    LexItem tok;

    if( !t1 ) {
        return false;
    }

    tok = Parser::GetNextToken(in, line);
    if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    if ( tok == GTHAN  || tok == EQUAL  || tok == LTHAN)
    {
        t1 = Expr(in, line, rightHandSide);
        if( !t1 )
        {
            ParseError(line, "Missing expression after relational operator");
            return false;
        }

        if (tok == EQUAL) {
            retVal = (leftHandSide == rightHandSide);
        } else if (tok == GTHAN) {
            retVal = (leftHandSide > rightHandSide);
        } else if (tok == LTHAN) {
            retVal = (leftHandSide < rightHandSide);
        }

        if (retVal.IsErr()) {
            ParseError(line, "Run-Time Error-Illegal Mixed Type Operands for a Logic Expression");
            return  false;
        }

        return true;
    }
    Parser::PushBackToken(tok);
    return true;
}

//Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream& in, int& line, int sign, Value & retVal) {
    LexItem tok = Parser::GetNextToken(in, line);

    if( tok == IDENT ) {
        string lexeme = tok.GetLexeme();
        if (!(defVar.find(lexeme)->second)) {
            ParseError(line, "Using Undefined Variable");
            return false;
        } else if ((TempsResults.find(lexeme)->first) != lexeme) {
            ParseError(line, "Undefined Variable");
            return false;
        }
        retVal = TempsResults.find(lexeme)->second;
        return true;
    }
    else if( tok == ICONST ) {
        int value = stoi(tok.GetLexeme());
        retVal = Value(value);
        return true;
    }
    else if( tok == SCONST ) {
        if (sign == 1) {
            ParseError(line, "Illegal Operand Type for Sign Operator");
            return false;
        }
        retVal = Value(tok.GetLexeme());
        return true;
    }
    else if( tok == RCONST ) {
        float value = stof(tok.GetLexeme());
        retVal = Value(value);
        return true;
    }
    else if( tok == LPAREN ) {
        Value temp = Value();
        bool ex = Expr(in, line, temp);
        if (!ex) {
            ParseError(line, "Missing expression after (");
            return false;
        }
        if (Parser::GetNextToken(in, line) == RPAREN) {
            retVal = temp;
            return ex;
        } else {
            Parser::PushBackToken(tok);
            ParseError(line, "Missing ) after expression");
            return false;
        }
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }

    return false;
}