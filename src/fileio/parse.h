#ifndef __PARSE_H__
#define __PARSE_H__
// Stupid MSVC complains (for like 1 million lines) that the ugly identifiers for templated
// classes are truncated.  Not my problem, eh?
#pragma warning( disable : 4786 )

#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

class Exception
{
public:
	Exception( const string& m ) 
		: msg( m ) {}
	
	string getMsg() const { return msg; }

private:
	string msg;
};

inline ostream& operator <<( ostream& os, const Exception& x )
{
	return os << x.getMsg();
}

class Obj;

typedef vector<Obj*> 		mytuple;
typedef map<string,Obj*> 	dict;

class ParseError
	: public Exception
{
public:
	ParseError( const string& msg )
		: Exception( msg )
	{}
};

class ObjTypeMismatch
	: public ParseError
{
public:
	ObjTypeMismatch( string expected, string got ) 
		: ParseError( string( "Type mismatch error during parse: expected " ) + 
				expected + string( ", got " ) + got + string( "." ) )
	{}
};

class Obj
{
public:
	virtual ~Obj() {}

	virtual string getTypeName() const { return string( "token" ); }
	virtual void printOn( ostream& os ) const {};

	virtual double 		 getScalar() const 
	{ throw ObjTypeMismatch( string( "scalar" ), getTypeName() ); }
	virtual bool   		 getBoolean() const 
	{ throw ObjTypeMismatch( string( "bool" ), getTypeName() ); }
	virtual string 		 getID() const 
	{ throw ObjTypeMismatch( string( "id" ), getTypeName() ); }
	virtual string 		 getString() const 
	{ throw ObjTypeMismatch( string( "string" ), getTypeName() ); }
	virtual const mytuple& getTuple() const 
	{ throw ObjTypeMismatch( string( "tuple" ), getTypeName() ); }
	virtual const dict&  getDict() const 
	{ throw ObjTypeMismatch( string( "dict" ), getTypeName() ); }

	virtual string 		 getName() const
	{ throw ObjTypeMismatch( string( "named" ), getTypeName() ); }
	virtual Obj 		 *getChild() const
	{ throw ObjTypeMismatch( string( "named" ), getTypeName() ); }
protected:
	Obj() {}

private:
};

class ScalarObj
	: public Obj
{
public:
	ScalarObj( double v )
		: Obj()
		, val( v )
	{}
	virtual ~ScalarObj() {}

	virtual string getTypeName() const { return string( "scalar" ); }
	virtual void printOn( ostream& os ) const { os << val; }

	virtual double getScalar() const { return val; }

private:
	double val;
};

class BooleanObj
	: public Obj
{
public:
	BooleanObj( bool b )
		: Obj()
		, val( b )
	{}
	virtual ~BooleanObj() {}

	virtual string getTypeName() const { return string( "bool" ); }
	virtual void printOn( ostream& os ) const { os << (val?"true":"false"); }

	virtual bool getBoolean() const { return val; }

private:
	bool val;
};

class IdObj
	: public Obj
{
public:
	IdObj( const string& s )
		: Obj()
		, val( s )
	{}
	virtual ~IdObj() {}

	virtual string getTypeName() const { return string( "id" ); }
	virtual void printOn( ostream& os ) const { os << val; }
	virtual string getID() const { return val; }

private:
	string val;
};

class StringObj
	: public Obj
{
public:
	StringObj( const string& s )
		: Obj()
		, val( s )
	{}
	virtual ~StringObj() {}

	virtual string getTypeName() const { return string( "string" ); }
	virtual void printOn( ostream& os ) const { os << '"' << val << '"'; }
	virtual string getString() const { return val; }

private:
	string val;
};

class TupleObj
	: public Obj
{
public:
	TupleObj( const mytuple& vec )
		: Obj()
		, val( vec )
	{}
	virtual ~TupleObj()
	{
		for( mytuple::iterator i = val.begin(); i != val.end(); ++i ) {
			delete (*i);
		}
	}

	virtual string getTypeName() const { return string( "tuple" ); }
	virtual void printOn( ostream& os ) const 
	{ 
		bool first = true;
		os << '(';
		for( int idx = 0; idx < val.size(); ++idx ) {
			if( first ) {
				first = false;
			} else {
				os << ", ";
			}
			val[ idx ]->printOn( os );
		}
		os << ')';
	}

	virtual const mytuple& getTuple() const { return val; }

private:
	mytuple val;
};

class DictObj
	: public Obj
{
public:
	DictObj( const dict& m )
		: Obj()
		, val( m )
	{}
	virtual ~DictObj()
	{
		for( dict::iterator i = val.begin(); i != val.end(); ++i ) {
			delete ((*i).second);
		}
	}

	virtual string getTypeName() const { return string( "dict" ); }
	virtual void printOn( ostream& os ) const 
	{ 
		bool first = true;
		os << '{';
		for( dict::const_iterator ci = val.begin(); 
				ci != val.end(); ++ci ) {
			if( first ) {
				first = false;
			} else {
				os << "; ";
			}
			os << (*ci).first << " = ";
			(*ci).second->printOn( os );
		}
		os << '}';
	}
	virtual const dict& getDict() const { return val; }

private:
	dict val;
};

class NamedObj
	: public Obj
{
public:
	NamedObj( const string& n, Obj *ch )
		: Obj()
		, name( n )
		, child( ch )
	{}
	virtual ~NamedObj()
	{
		delete child;
	}

	virtual string getTypeName() const { return string( "named" ); }
	virtual void printOn( ostream& os ) const 
	{ 
		os << name << ' ';
		child->printOn( os );
	}

	virtual string getName() const { return name; }
	virtual Obj *getChild() const { return child; }

private:
	string name;
	Obj *child;
};

Obj *readFile( istream& is );

#endif // __PARSE_H__
