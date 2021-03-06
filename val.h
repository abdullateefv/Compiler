#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <iomanip>

using namespace std;

enum ValType { VINT, VREAL, VSTRING, VBOOL, VERR };

class Value {
    ValType	T;
    bool    Btemp;
	int		Itemp;
	float   Rtemp;
    string	Stemp;
       
public:
    Value() : T(VERR), Btemp(false), Itemp(0), Rtemp(0.0), Stemp("") {}
    Value(bool vb) : T(VBOOL), Btemp(vb), Itemp(0), Rtemp(0.0), Stemp("") {}
    Value(int vi) : T(VINT), Btemp(false), Itemp(vi), Rtemp(0.0), Stemp("") {}
	Value(float vr) : T(VREAL), Btemp(false), Itemp(0), Rtemp(vr), Stemp("") {}
    Value(string vs) : T(VSTRING), Btemp(false), Itemp(0), Rtemp(0.0), Stemp(vs) {}
    
    
    ValType GetType() const { return T; }
    bool IsErr() const { return T == VERR; }
    bool IsInt() const { return T == VINT; }
    bool IsString() const { return T == VSTRING; }
    bool IsReal() const {return T == VREAL;}
    bool IsBool() const {return T == VBOOL;}
    
    int GetInt() const { if( IsInt() ) return Itemp; throw "RUNTIME ERROR: Value not an integer"; }
    
    string GetString() const { if( IsString() ) return Stemp; throw "RUNTIME ERROR: Value not a string"; }
    
    float GetReal() const { if( IsReal() ) return Rtemp; throw "RUNTIME ERROR: Value not an integer"; }
    
    bool GetBool() const {if(IsBool()) return Btemp; throw "RUNTIME ERROR: Value not a boolean";}
    
    void SetType(ValType type)
    {
        T = type;
	}

    void SetInt(int val)
    {
    	Itemp = val;
	}
	
	void SetReal(float val)
    {
    	Rtemp = val;
	}
	
	void SetString(string val)
    {
    	Stemp = val;
	}
	
	void SetBool(bool val)
    {
    	Btemp = val;
	}
	
	
    // add op to this
    Value operator+(const Value& op) const {
        //String Concat
        if (this->GetType() == VSTRING && op.GetType() == VSTRING) {
            return this->GetString() + op.GetString();
        }

            //Addition
        else if (this->GetType() == VINT && op.GetType() == VINT) {
            return this->GetInt() + op.GetInt();
        } else if (this->GetType() == VREAL && op.GetType() == VREAL) {
            return this->GetReal() + op.GetReal();
        } else if (this->GetType() == VREAL && op.GetType() == VINT) {
            return this->GetReal() + op.GetInt();
        } else if (this->GetType() == VINT && op.GetType() == VREAL) {
            return this->GetInt() + op.GetReal();
        } else {
            Value errorVal;
            return errorVal;
        }
    };
    
    // subtract op from this
    Value operator-(const Value& op) const {
        if (this->GetType() == VINT && op.GetType() == VINT) {
            return this->GetInt() - op.GetInt();
        } else if (this->GetType() == VREAL && op.GetType() == VREAL) {
            return this->GetReal() - op.GetReal();
        } else if (this->GetType() == VREAL && op.GetType() == VINT) {
            return this->GetReal() - op.GetInt();
        } else if (this->GetType() == VINT && op.GetType() == VREAL) {
            return this->GetInt() - op.GetReal();
        } else {
            Value errorVal;
            return errorVal;
        }
    };
    
    // multiply this by op
    Value operator*(const Value& op) const {
        if (this->GetType() == VINT && op.GetType() == VINT) {
            return this->GetInt() * op.GetInt();
        } else if (this->GetType() == VREAL && op.GetType() == VREAL) {
            return this->GetReal() * op.GetReal();
        } else if (this->GetType() == VREAL && op.GetType() == VINT) {
            return this->GetReal() * op.GetInt();
        } else if (this->GetType() == VINT && op.GetType() == VREAL) {
            return this->GetInt() * op.GetReal();
        } else {
            Value errorVal;
            return errorVal;
        }
    };
    
    // divide this by op
    Value operator/(const Value& op) const {
        if (this->GetType() == VINT && op.GetType() == VINT) {
            return this->GetInt() / op.GetInt();
        } else if (this->GetType() == VREAL && op.GetType() == VREAL) {
            return this->GetReal() / op.GetReal();
        } else if (this->GetType() == VREAL && op.GetType() == VINT) {
            return this->GetReal() / op.GetInt();
        } else if (this->GetType() == VINT && op.GetType() == VREAL) {
            return this->GetInt() / op.GetReal();
        } else {
            Value errorVal;
            return errorVal;
        }
    };
    
    Value operator==(const Value& op) const {
        if (this->GetType() == VINT && op.GetType() == VINT) {
            return Value(this->GetInt() == op.GetInt());
        } else if (this->GetType() == VREAL && op.GetType() == VREAL) {
            return Value(this->GetReal() == op.GetReal());
        } else if (this->GetType() == VREAL && op.GetType() == VINT) {
            return Value(this->GetReal() == op.GetInt());
        } else if (this->GetType() == VINT && op.GetType() == VREAL) {
            return Value(this->GetInt() == op.GetReal());
        } else if (this->GetType() == VSTRING && op.GetType() == VSTRING) {
            return Value(this->GetString() == op.GetString());
        } else {
            Value errorVal;
            return errorVal;
        }
    };

	Value operator>(const Value& op) const {
        if (this->GetType() == VINT && op.GetType() == VINT) {
            return Value(this->GetInt() > op.GetInt());
        } else if (this->GetType() == VREAL && op.GetType() == VREAL) {
            return Value(this->GetReal() > op.GetReal());
        } else if (this->GetType() == VREAL && op.GetType() == VINT) {
            return Value(this->GetReal() > op.GetInt());
        } else if (this->GetType() == VINT && op.GetType() == VREAL) {
            return Value(this->GetInt() > op.GetReal());
        } else {
            Value errorVal;
            return errorVal;
        }
    };
	
	Value operator<(const Value& op) const {
        if (this->GetType() == VINT && op.GetType() == VINT) {
            return Value(this->GetInt() < op.GetInt());
        } else if (this->GetType() == VREAL && op.GetType() == VREAL) {
            return Value(this->GetReal() < op.GetReal());
        } else if (this->GetType() == VREAL && op.GetType() == VINT) {
            return Value(this->GetReal() < op.GetInt());
        } else if (this->GetType() == VINT && op.GetType() == VREAL) {
            return Value(this->GetInt() < op.GetReal());
        } else {
            Value errorVal;
            return errorVal;
        }
    };

    friend ostream& operator<<(ostream& out, const Value& op) {
        if( op.IsInt() ) out << op.Itemp;
        else if( op.IsString() ) out << op.Stemp;
        else if( op.IsReal()) out <<  fixed << showpoint << setprecision(2) << op.Rtemp;
        else out << "ERROR";
        return out;
    }
};


#endif
