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

map <string, int> label;
set <string> usedLabel;
set <pair <int, string> > error;
set < string > warning;

map <int, string> errorCode={
                            {0, "Incorrect LabelName"},
                            {1, "Incorrect Instruction format"},
                            {2, "Duplicate LabelNames"},
                            {3, "Not a Valid Number"}
                            };

map < string, int> inst_without_oper={
                                    {"add", 6},
                                    {"sub", 7},
                                    {"shl", 8},
                                    {"shr", 9},
                                    {"a2sp", 11},
                                    {"sp2a",12},
                                    {"return",14},
                                    {"HALT",18}
                                    };
map < string,int > inst_with_oper{
                                {"ldc",0},
                                {"adc",1},
                                {"ldl",2},
                                {"stl",3},
                                {"ldnl",4},
                                {"stnl",5},
                                {"adj",10},
                                {"call",13},
                                {"brz",15},
                                {"brlz",16},
                                {"br",17},
                                {"SET",19},
                                {"data",20}
                                };

map < string,int > inst_with_pcoff{
                                {"call",13},
                                {"brz",15},
                                {"brlz",16},
                                {"br",17}
                                };

vector <int> machineCode;

int linePos=1;

void removeComments(string &s){
    s=s.substr(0, s.find(';'));
}

vector <string> readWords(string &line){
    for(int i=0; i<line.length(); i++){
        if(line[i]==':'){
            line.insert(i+1, " ");
        }
    }
    removeComments(line);
            
    stringstream ss(line);

    vector <string> word;
    string input;
    while(ss >> input){
        word.push_back(input);
    }

    return word;
}

bool checkLabel(string &s){
    for(int i=0; i<s.length(); i++){
        char c=s[i];

        if(i==0){
            if(!isalpha(c) && c!='_'){
                return false;
            }
        }

        if(!iswalnum(c) && c!='_'){
            return false;
        }
    }

    return true;
}

int strToNo(string &s){
    if(s.length()>2){
        if(s[0]=='0' && s[1]=='x'){
            for(int i=2; i<s.length(); i++){
                if(!('0'<=s[i]<='9' || 'a'<=tolower(s[i])<='f')){
                    error.insert({linePos, errorCode[3]});
                    return -1;
                }
            }

            return stoi(s, 0, 16);
        }
    } 

    if(s.length()>1){
        if(s[0]=='0'){
            for(int i=1; i<s.length(); i++){
                if(!('0'<=s[i]<='7')){
                    error.insert({linePos, errorCode[3]});
                    return -1;
                }
            }

            return stoi(s, 0, 8);
        }
    }

    for(int i=0; i<s.length(); i++){
        if(i==0 && (s[0]=='+' || s[0]=='-')){
            i++;
        }

        if(!isdigit(s[i])){
            error.insert({linePos, errorCode[3]});
            return -1;
        }

        return stoi(s);
    }
}

void insertMachineCode(int nop, vector <string> word, ofstream *list){
    int code=0;

    if(nop==0){
        machineCode.push_back(inst_without_oper[word[0]]);
        *list << setw(8) << setfill('0') << hex << inst_without_oper[word[0]] << " ";
        return;
    }
    else{
        if(word.size()==1){
            return;
        }
        
        if(word[0]=="SET"){
            return;
        }
        if(word[0]=="data"){
            if(label.find(word[1])!=label.end()){
                usedLabel.insert(word[1]);
                code|=label[word[1]];
            }
            else{
                code|=strToNo(word[1]);
            }

            machineCode.push_back(code);
            *list << setw(8) << setfill('0') << hex << code << " ";
            return;
        }

        code=inst_with_oper[word[0]];

        if(inst_with_pcoff.find(word[0])!=inst_with_pcoff.end()){
            if(label.find(word[1])!=label.end()){
                usedLabel.insert(word[1]);
                code|=((label[word[1]]-machineCode.size()-1)<<8);
            }
            else{
                error.insert({linePos, "No such label"});
            }
        }
        else{
            if(label.find(word[1])!=label.end()){
                usedLabel.insert(word[1]);
                code|=((label[word[1]])<<8);
            }
            else{
                code|=(strToNo(word[1])<<8);
            }
        }
        machineCode.push_back(code);
        *list << setw(8) << setfill('0') << hex << code << " ";
    }

}

void writeLog(string filename){
    string logfname=filename+".log";

    ofstream log;
    log.open(logfname, ios::out);

    if(error.size()){
        log << "ERRORS:" << endl;
    }
    for(auto i:error){
        log << i.second << " on line " << i.first << endl;
        cout << "ERROR:" <<i.second << " on line " << i.first << endl;
    }
    cout << endl << endl;

    if(warning.size()){
        log << "WARNINGS:" << endl;
    }
    for(auto i:warning){
        log << i << endl;
        cout << "WARNING:" << i << endl;
    }

    log.close();
}

void writeObj(string filename){
    string objfname=filename+".obj";

    ofstream obj;
    obj.open(objfname, ios::out | ios::binary);

    for(int i=0; i<machineCode.size(); i++){
        obj.write((char*)&machineCode[i], 4);
    }
    
    obj.close();
}

int main(int argc, char **argv){
    if(argc!=2){
        cout << "Less Number of Arguments given" << endl;
    }

    string filename;
    for(int i=0; i<strlen(argv[1]); i++){
        if(argv[1][i]=='.'){
            break;
        }
        filename+=argv[1][i];
    }

    ifstream assembly;
    assembly.open(argv[1], ios::in);

    if(assembly.is_open()){
        int pc=0;

        string line;
        while(getline(assembly, line)){
            vector <string> word=readWords(line);
            if(word.size()==0){
                linePos++;
                continue;
            }

            int lineLength=word.size();
            if(word[0][word[0].length()-1]==':'){
                string labelToCheck=word[0].substr(0, word[0].length()-1);
                if(checkLabel(labelToCheck)){
                    if(label.find(labelToCheck)==label.end()){
                        label[labelToCheck]=pc;
                    }
                    else{
                        error.insert({linePos, errorCode[2]});
                    }

                    if(word.size()==1){
                        linePos++;
                        continue;
                    }
                    else{
                        if(word.size()==3){
                            if(inst_with_oper.find(word[1])==inst_with_oper.end()){
                                error.insert({linePos, errorCode[1]});
                            }

                            if(word[1]=="SET"){
                                label[labelToCheck]=strToNo(word[2]);
                                linePos++;
                                continue;
                            }
                        }
                        else if (word.size()==2){
                            if(inst_without_oper.find(word[1])==inst_without_oper.end()){
                                error.insert({linePos, errorCode[1]});
                            }
                        }
                        else{
                            error.insert({linePos, errorCode[1]});
                        }
                    }
                }
                else{
                    error.insert({linePos, errorCode[0]});
                }
            }
            else{
                if(word.size()==2){
                    if(inst_with_oper.find(word[0])==inst_with_oper.end()){
                        error.insert({linePos, errorCode[1]});
                    }
                }
                else if(word.size()==1){
                    if(inst_without_oper.find(word[0])==inst_without_oper.end()){
                        error.insert({linePos, errorCode[1]});
                    }
                }
                else{
                    error.insert({linePos, errorCode[1]});
                }
            }

            pc++;
            linePos++;
        }
    }
    else{
        cout << "Unable to open the file" << endl;
    }

    assembly.close();

    string listName=filename+".lst";
    ofstream list;
    list.open(listName, ios::out);
    
    assembly.open(argv[1], ios::in);
    linePos=1;
    string line;
    while(getline(assembly, line)){
        list << setw(8) << setfill('0') << hex << linePos << " ";
        vector <string> word=readWords(line);
        
        int lineLength=word.size();
        if(word.size()==0){
            list << endl;
            linePos++;
            continue;
        }

        if(word[0][word[0].length()-1]==':'){
            word.erase(word.begin());
        }

        if(word.size()==0){
            list << endl;
            linePos++;
            continue;
        }

        if(inst_without_oper.find(word[0])!=inst_without_oper.end()){
            insertMachineCode(0, word, &list);
        }
        else{
            insertMachineCode(1, word, &list);
        }
        
        list << line << endl;
        linePos++;
    }

    for(auto it:label){
        if(usedLabel.find(it.first)==usedLabel.end()){
            string warn="Unused Label ";
            warn+=it.first;

            warning.insert(warn);
        }
    }

    assembly.close();
    list.close();

    writeLog(filename);

    writeObj(filename);
}