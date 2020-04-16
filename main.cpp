/* =============================================
 *  MapleMML Converter
 * ---------------------------------------------
 *    This program converts MML files saved by
 *  3ML Editor to MS2MML files compatible in
 *  the online game MapleStory2.
 * ---------------------------------------------
 *  Author: SaltfishAmi @ MultiLine PearlHarbor
 *  Website: http://fuckami.moe/
 * ---------------------------------------------
 *  This program is distributed under WTFPLv2.
 *  For more details, visit www.wtfpl.org.
 *    THIS PROGRAM IS PROVIDED 'AS IS' WITHOUT
 *  ANY WARRANTY, EITHER EXPRESSED OR IMPLIED.
 * ---------------------------------------------
 *  SaltfishAmi 2017/10/04
 * =============================================
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <conio>
using namespace std;
#define VERSION "1.3.0"
//#define __DEBUG
bool overwrite = false;
bool test = false;
int main(int argc, char** argv){
    if(argc==1){
        //show help
        cout << "MapleMML Converter v" << VERSION << endl;
        cout << "Converts MML files to MapleStory2 compatible MS2MML files." << endl << endl;
        cout << "maplemml [/O | /T] [filename[filename [...]]] " << endl << endl;
        cout << "  no arguments  Displays this help info." << endl;
        cout << "  filename      Specifies the input MML file name." << endl;
        cout << "                Output file will have the same name by default." << endl;
        cout << "                Only supports MML files created with 3ML editor." << endl;
        cout << "                Supports batch processing." << endl;
        cout << "  /O            Overwrites any existing file." << endl;
        cout << "                If this is not set," << endl;
        cout << "                you will be prompted for a new name when a name conflict happens." << endl;
        cout << "  /T            Only tests the file length and prints results," << endl;
        cout << "                without actually writing outputs." << endl << endl;
        cout << "This software is written by SaltfishAmi and is licensed under WTFPLv2." << endl;
        cout << "Contact me at: http://fuckami.moe/ or MS2 China MultiLine PearlHarbor server." << endl;
        cout << "Press any key to exit..." <<endl;
        getch();
        return 0;
    }
    vector<string> args;
    vector<string>::iterator argIt;
    for(int i=1; i<argc; i++){
        args.push_back(string(argv[i]));
        #ifdef __DEBUG
            cout << args[i-1] << " ";
        #endif
    }
    cout << endl;
    argIt = args.begin();

    if(stricmp(args[0].c_str(), "/T")==0){
        test = true;
        args.erase(argIt);
        //#ifdef __DEBUG
            cout << endl << "Test mode: no files will be actually written";
        //#endif
    } else if(stricmp(args[0].c_str(), "/O")==0){
        overwrite = true;
        args.erase(argIt);
        //#ifdef __DEBUG
            cout << endl << "Overwrite mode: existing files will be overwritten";
        //#endif
    }
    cout << endl;
    argIt = args.begin();
    string outname;
    //bool overwriteFlag = false;
    int fileCount=0, successCount=0, failCount=0;
    char buf[5001];
    int read;
    int chord;
    string channelData;
    while(argIt != args.end()){
        #ifdef __DEBUG
            cout << args.size();
        #endif
        ifstream infile, testin;
        ofstream outfile;
        cout << endl;
        fileCount++;
        cout << "Opening file: " << *argIt << endl;
        infile.open((*argIt).c_str());
        if(!infile){
            failCount++;
            cout << "Unable to open file: " << *argIt << ", skipping" << endl;
            infile.close();
            argIt++;
            continue;
        }
        outname = "";
        outname = *argIt;
        outname.resize(outname.size()-3);
        outname.append("ms2mml");
        #ifdef __DEBUG
            cout << endl << outname << endl;
        #endif
        while(!test){
            //overwriteFlag = false;
            testin.open(outname.c_str());
            if(testin){
                #ifdef __DEBUG
                    cout << endl << "destination file already exist" << endl;
                #endif
                if(!overwrite){   /*
                    failCount++;
                    cout << "File already exists: " << outname << ", try using /O option" <<endl;
                    infile.close();
                    testin.close();
                    argIt++;
                    continue; */
                    cout << "File already exists: " << outname << ", please enter new name: " << endl;
                    cin >> outname;
                    testin.close();
                    continue;
                }
            } else {
                #ifdef __DEBUG
                    cout << endl << "destination file not exist" << endl;
                #endif
            }
            testin.close();
            #ifdef __DEBUG
                //getch();
            #endif
            break;
        }
        int continueFlag=0;
        while(1){
            infile.getline(buf, 5001);
            #ifdef __DEBUG
            //    cout << buf << endl;
            #endif
            string tmp=string(buf).substr(0, 8);
            if(stricmp(tmp.c_str(), "[Channel")==0)break;
            if(infile.eof()){
                failCount++;
                cout << "Invalid file: " << outname << " (Channel not found), maybe not saved with 3ML Editor?" << endl;
                infile.close();
                argIt++;
                continueFlag=1;
                break;
            }
        }
        if(continueFlag--==1)continue;
        #ifdef __DEBUG
            cout << "found channel" << endl;
        #endif
        //------------------------
        read=0;
        chord=0;
        channelData = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<ms2>\n<melody>\n<![CDATA[\n";
        while(!infile.eof()){
            infile.getline(buf, 5001);
            #ifdef __DEBUG
            //    cout << buf << endl;
            #endif
            if(string(buf).find("[Channel", 0)!=string::npos){
                #ifdef __DEBUG
                    cout << "next channel" << endl;
                #endif
                channelData.append("\n]]>\n</");
                if(!chord)channelData.append("melody");
                else channelData.append("chord");
                chord++;
                channelData.append(">\n<chord index=\"");
                char* tmp = "";
                itoa(chord, tmp, 10);
                //string stmp = string(tmp);
                cout << "";
                cout.flush();
                #ifdef __DEBUG
                    cout.flush();
                    cout << "itoa result:" <<tmp<< endl;
                #endif
                channelData.append(tmp);
                channelData.append("\">\n<![CDATA[\n");
                continue;

            } else if(string(buf).find("[3MLE EXTENSION]", 0)!=string::npos){
                #ifdef __DEBUG
                    cout << "end of file" << endl;
                #endif
                channelData.append("\n]]>\n</chord>\n</ms2>");
                break;
            }
            channelData.append(buf);
            read += infile.gcount();
            read--;
            if(read > 5000){
                failCount++;
                cout << "File too long: " << *argIt << ", " << read << " chars in total; skipping" << endl;
                infile.close();
                argIt++;
                continueFlag = 1;
                break;
            }
        }
        if(continueFlag--==1)continue;
        //------------------------
        if(infile.eof()){
            //reached file end without finding [3MLE EXTENSION]
            //invalid file
            failCount++;
            cout << "Invalid file: " << *argIt << " (unexpected file end), maybe not saved with 3ML Editor?" << endl;
            infile.close();
            argIt++;
            continue;
        }
        infile.close();
        if(!test){
            outfile.open(outname.c_str());
            if(!outfile){
                failCount++;
                cout << "Unable to open file: " << outname << ", skipping" << endl;
                outfile.close();
                argIt++;
                continue;
            }
            outfile.write(channelData.c_str(), channelData.length());
            successCount++;
            cout << "Successfully saved file: " << outname << ", " << chord+1 << " channels, " << read << " chars in total" <<endl;
            outfile.close();
        } else successCount++;
        argIt++;
    }
    cout << endl << "Processed " << fileCount << " file(s), " << successCount << " done, " << failCount << " failed." << endl;
    if(test)cout << "Note that you enabled the /T option, therefore NO files will be actually created." << endl;
    cout << endl << "Press any key to exit..." <<endl;
    getch();
	return 0;
}