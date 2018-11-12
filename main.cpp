//
//  main.cpp
//  cplusplus
//
//  Created by Joris Borst Pauwels on 30/10/2018.
//  Copyright © 2018 Joris Borst Pauwels. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <vector>
#include <tuple>
//using namespace std;


#if DEBUG
#define debugLog(s) (std::cout << (s)) // in c use: #define debugLog( ... ) printf( __VA_ARGS__ )
#else
#define debugLog(s) ;
#endif


// MyBaseClass
static int cnt = 0;
class MyBaseClass {
private:
    std::string name;
    
public:
    enum myenum_t {
        
        opt_a,
        opt_b,
        opt_c
    } myenum;

public:
    static std::string whoami( void ) {
        
        return "MyBaseClass";
    }

    virtual std::string description( void ) {
        
        std::stringstream ss;
        ss << typeid(this).name() << "<<" << this << ">>: " << cnt;
        return (ss.str());
    };
    
    MyBaseClass( std::string name ) {
        
        this->name = name;
        cnt++;
    }

    ~MyBaseClass() {
        
        std::stringstream ss;
        ss << typeid(this).name() << " (" << cnt << "-" << this->name << "): Bye bye!\n";
        debugLog( ss.str() );
        cnt--;
    }
    
    void genException() {
        
        throw std::runtime_error( "My bad!" );
    }
};

//
// MyClass
//
class MyClass: public MyBaseClass {
    
private:
    int i;
    float f;
    
public:
    
    static std::string whoami( void ) {
        
        return "MyClass";
    }
    
    static MyClass* fromStock() {
        
        return new MyClass;
    }

    MyClass( int a, float b) : MyBaseClass( typeid(this).name() ) {

        this->i = a;
        this->f = b;
    }
    
    MyClass() : MyBaseClass( typeid(this).name() ) {

        this->i = 0;
        this->f = 0.0f;
    }
    
    // Copy contructor
    MyClass( const MyClass& rval ) : MyBaseClass( typeid(this).name() ) {
        
        this->i = rval.i;
        this->f = rval.f;
    }
    
    std::string description( void ) {

        std::stringstream ss;
        ss << MyBaseClass::description() << typeid(this).name() << "<<" << this << ">>: " << this->i << ", " << this->f;
        return (ss.str());
    }

    // function operator (use object as function eg: string str = myclass())
    std::tuple<int, float> operator() (void) const {
        
        return std::tuple<int, float> (this->i, this->f);
    }
    
    // operator + overload
    const MyClass operator+(const MyClass& rval) const {
        
        MyClass result;
        result.i = this->i + rval.i;
        result.f = this->f + rval.f;
        return result;
    }
    
};

// Operator << overload (globalspace, but determined by std::ostream and MyClass types)
std::ostream &operator<<(std::ostream& os, MyClass& myclass) {
    
    return os << myclass.description();
}

// Operator - overload (globalspace, but determined by std::ostream and MyClass types)
const int operator-(const int lval, const MyClass& mc)  {
    
    std::tuple<int, float> t = mc();
    return lval - std::get<0>(t);
}

// Free function returning a <int, MyClass> tuple
std::tuple<int, MyClass> tupleFunc( void ) {
    
    MyClass myclass;
    std::tuple<int, MyClass> result(1, myclass);
    return result;
}

// Free function returning a std::string based on a printf like format input.
template<typename ... VarArgsType>
std::string string_format( const std::string& format, VarArgsType ... args ) {
    
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // calc space needed (note: null pointer& size = 0)
    std::unique_ptr<char[]> buf( new char[ size ] ); // smart pointer to buffe with size calculated using snprintf
    snprintf( buf.get(), size, format.c_str(), args ... ); // Now the real thing with snprintf
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside our std::string
}


int main(int argc, const char * argv[]) try {
   
    // Report 'in'
    std::cout << __FILE__ << " argc = " << argc << " argv[] = " << argv << std::endl;

    // Create a std:string using a formatstring with var args
    std::string s = string_format( "Hello %dst %s!\n", 1, "world" );
    std::cout << s << std::endl;

    // Create a 'default' (stock) MyClass object using a static member func
    MyClass* classp = MyClass::fromStock();
    std::cout << *classp << std::endl;
    __unused std::tuple<int, float> fft = (*classp)();
    __unused MyBaseClass::myenum_t enam = MyBaseClass::opt_a;
    
    // Some std::string magic...
    std::string s0 (" Initial string");
    std::string s1 = s0 + " Hello";
    s1 += "?";
    s1.append("_World ", 1, 6 );
    s1.erase( remove_if( s1.begin(), s1.end(), isspace), s1.end() );
    __unused std::string::size_type cap = s1.capacity();
    __unused std::string::size_type len = s1.length();

    // Class operator overload
    MyClass a {1, 1.0f};
    MyClass b {2, 2.0f};
    MyClass res = a+b;
    int res2 = 1-b;
    __unused int res3 = 1-2;
    std::cout << res2 << std::endl;
    MyClass x = res;
    
    // This is using the free << operator overload so that cout is printing our custom description.
    std::cout << a << " + " << b << " = " << res << std::endl;
    
    // Stuff with vectors...
    std::vector<int> vec1 (2,11);
    std::vector<int> vec2 {1,2,3,4,5};
    std::vector<int> vec3 = vec1;
    vec3.insert( vec3.end(), vec2.begin(), vec2.end() );

    std::cout << "=================\n";
    for( std::vector<int>::iterator it = vec3.begin(); it != vec3.end(); ++it ) {
        
        std::cout << *it << ", ";
    }
    std::cout << "\n=================\n";
    
    std::tuple<int, MyClass> t = tupleFunc();
    std::cout << std::get < 0 > (t) << ", " << std::get < 1 > (t) << std::endl;
    
    // Exit with an exception
    classp->genException();

    // Won't reach here because of above exception.
    std::cerr << "All is OK now. Bye, bye!\n";

    return 0;
}
catch (std::exception& e)
{
    std::cerr << "exception caught: " << e.what() << '\n';
    std::cerr << "Exceution failed. Bye, bye!\n";
    return -1; // seems to return a uint8 here???
}
