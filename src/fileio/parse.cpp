// Stupid MSVC complains (for like 1 million lines) that the ugly identifiers for templated
// classes are truncated.  Not my problem, eh?
#ifdef WIN32
#pragma warning( disable : 4786 )
#endif

#include <cstring>

#include "parse.h"

static string readID( istream& is );
static Obj *readString( istream& is );
static Obj *readScalar( istream& is );
static Obj *readTuple( istream& is );
static Obj *readDict( istream& is );
static Obj *readObject( istream& is );
static Obj *readName( istream& is );
static void eatWS( istream& is );
static void eatNL( istream& is );

Obj *readFile( istream& is )
{
	return readObject( is );
}

static void eatWS( istream& is )
{
	int ch = is.peek();
	while( ch == ' ' || ch == '\t' || ch == '\n' || ch == 0x0D || ch == 0x0A) {
		is.get();
		if( !is ) {
			return;
		}
		ch = is.peek();
	}
}

static void eatNL( istream& is )
{
	int ch = is.peek();
	while( ch != '\n' ) {
		is.get();
		if( !is ) {
			return;
		}
		ch = is.peek();
	}
}

static bool eat( istream& is ) 
{
	while( is ) {
		eatWS( is );
		if( is ) {
			int ch = is.peek();
			if( ch == '/' ) {
				is.get();
				int ch = is.peek();
				if( ch == '/' ) {
					eatNL( is );
				} else if( ch == '*' ) {
					while( true ) {
						is.get();
						ch = is.peek();
						if( ch == '*' ) {
							is.get();
							ch = is.peek();
							if( ch == '/' ) {
								is.get();
								break;
							} else if( ch == -1 ) {	
								is.get();
								throw ParseError( 
									"Parse Error: unterminated comment" );
							}
						} else if( ch == -1 ) {
							is.get();
							throw ParseError( 
								"Parse Error: unterminated comment" );
						}
					}
				} else {
					return true;
				}
			} else if( ch == -1 ) {
				is.get();
				return false;
			} else {
				return true;
			}
		} else {
			return false;
		}
	}
	return false;
}

static Obj *readName( istream& is )
{
	string s = readID( is );

	if( s == "true" ) {
		return new BooleanObj( true );
	} else if( s == "false" ) {
		return new BooleanObj( false );
	} else {
		if( !eat( is ) ) {
			return new IdObj( s );
		}

		int ch = is.peek();
		if( strchr( "}),;", ch ) != NULL ) {
			return new IdObj( s );
		} else {
			return new NamedObj( s, readObject( is ) );
		}
	}
}

static string readID( istream& is )
{
	int ch;
	string ret( "" );

	ret += char( is.get() );

	while( is ) {
		ch = is.peek();
		if( strchr( " \t\n={}();,/", ch ) != NULL ) {
			break;
		} else {
			ret += char( ch );
			is.get();
		}
	}

	return ret;
}

static Obj *readString( istream& is ) 
{
	int ch;
	string ret( "" );

	is.get();

	while( true ) {
		ch = is.peek();
		if( ch == '"' ) {
			is.get();
			return new StringObj( ret );
		} else {
			ret += char( ch );
		}
		is.get();
	}
}

static Obj *readScalar( istream& is )
{
	int ch;
	string ret( "" );

	while( true ) {
		ch = is.peek();
		if( (ch == '-') || (ch == '.') || (ch == 'e') || (ch == 'E')
				|| (ch >= '0' && ch <= '9') ) {
			ret += char( ch );
			is.get();
		} else {
			break;
		}
	}

	return new ScalarObj( atof( ret.c_str() ) );
}

static Obj *readTuple( istream& is )
{
	vector<Obj*> ret;

	is.get();

	while( true ) {
		eat( is );
		ret.push_back( readObject( is ) );	
		eat( is );
		int ch = is.get();
		if( ch == ')' ) {
			return new TupleObj( ret );
		} else if( ch == ',' ) {
			continue;
		} else {
			throw ParseError( "Parse error: expected comma." );
		}
	}

	throw ParseError( "Parse error: internal error." );
}

static Obj *readDict( istream& is )
{
	string lhs;
	Obj *rhs;

	map<string,Obj*> ret;

	is.get();

	while( true ) {
		eat( is );
		if( is.peek() == '}' ) {
			is.get();
			return new DictObj( ret );
		}
		lhs = readID( is );
		eat( is );
		if( is.get() != '=' ) {
			throw ParseError( "Parse error: expected equals." );
		}
		rhs = readObject( is );
		ret[ lhs ] = rhs;
		eat( is );
		int ch = is.peek();
		if( ch == ';' ) {
			is.get();
		} else if( ch != '}' ) {
			throw ParseError( "Parse error: expected semicolon or brace." );
		}
	}
}

static Obj *readObject( istream& is )
{
	if( !eat( is ) ) {
		return NULL;
	}

	int ch = is.peek();

	if( (ch == '-') || (ch >= '0' && ch <= '9') ) {
		return readScalar( is );
	} else if( ch == '"' ) {
		return readString( is );
	} else if( ch == '(' ) {
		return readTuple( is );
	} else if( ch == '{' ) {
		return readDict( is );
	} else {
		return readName( is );
	}
}

/*
int main( void )
{
	Obj *o = readFile( cin );
	o->printOn( cout );
	delete o;
	return 0;
}
*/
