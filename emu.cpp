/*****************************************************************************
TITLE: Assembler, Emulator																																
AUTHOR:   Hardik Goyal
ROLLNO:   2001CS27
Declaration of Authorship
This txt file, claims.txt, is part of the assignment of CS322 at the 
department of Computer Science and Engg, IIT Patna . 
*****************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <string.h>
#include <map>
#include <set>
#include <iomanip>

using namespace std;

int a, b, memory[10000];
int sp=10000, pc=0;

int action(ofstream *out){
    unsigned int uninst= memory[pc];
    int mnemonic=uninst % 256u, operand=memory[pc]>>8;

    pc++;

    switch(mnemonic){
        case 0:
            b=a;
            a=operand;
            break;

        case 1:
            a=a+operand;
            break;
        
        case 2:
            b=a;
            a=memory[sp+operand];
            break;
        
        case 3:
            memory[sp+operand]=a;
            a=b;
            break;

        case 4:
            a=memory[a+operand];
            break;
        
        case 5:
            memory[a+operand]=b;
            break;

        case 6:
            a=b+a;
            break;
        
        case 7:
            a=b-a;
            break;

        case 8:
            a=b<<a;
            break;
        
        case 9:
            a=b>>a;
            break;

        case 10:
            sp=sp+operand;
            break;

        case 11:
            sp=a;
            a=b;
            break;

        case 12:
            b=a;
            a=sp;
            break;

        case 13:
            b=a;
            a=pc;
            pc=pc+operand;
            break;
        
        case 14:
            pc=a;
            a=b;
            break;

        case 15:
            if(a==0){
                pc=pc+operand;
            }
            break;

        case 16:
            if(a<0){
                pc=pc+operand;
            }
            break;

        case 17:
            pc=pc+operand;
            break;

        case 18:
            return 1;
    }
    
    *out << "a: " << a << "\tb: " << b << "\tsp: " << sp << endl;
    cout << "a: " << a << "\tb: " << b << "\tsp: " << sp << endl;

    return 0;
}


int main(int argc, char **argv){
    if(argc!=3){
        cout << "Improper Number of Arguments given" << endl;
        return 0;
    }

    string filename;
    for(int i=0; i<strlen(argv[2]); i++){
        if(argv[2][i]=='.'){
            break;
        }
        filename+=argv[2][i];
    }

    ifstream machine;
    machine.open(argv[2], ios::in | ios::binary);
    machine.seekg(0, ios::end);
    int fileSize=machine.tellg();
    int lineNo=fileSize/4;
    
    machine.seekg(0, ios::beg);
    for(int i=0; i<lineNo; i++){
        machine.read((char*)&memory[i], 4);
    }
    machine.close();

    filename+=".txt";
    ofstream out;
    out.open(filename, ios::out);
    while(true){
        out << setw(8) << setfill('0') << hex << pc << " ";
        cout << setw(8) << setfill('0') << hex << pc << " ";

        if(action(&out)) {
            out << "a: " << a << "\tb: " << b << "\tsp: " << sp << endl; 
            cout << "a: " << a << "\tb: " << b << "\tsp: " << sp << endl; 
            
            break;
        }
    }

    out.close();

    return 0;
}