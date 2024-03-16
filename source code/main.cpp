///author George Kiriakidis
///libraries used: mpg123, fftw3, libpng, png++

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <dirent.h>
#include "decode.h"
#include "fourier.h"
#include "exportspectrum.h"
#include "fingerprint.h"
#include "database.h"
#include "record.h"




using namespace std;

size_t srate = 44100;
size_t slicems=100;
string dbfile="db.txt";
float step_rate=0.5;
size_t minimum_seconds_matching=20;
size_t fuz_factor = 3;
size_t cont_threshold = 1;
bool sort_cont = false;

void pcm_to_fingerprints(short *pcm16, size_t n, unsigned long* &fingerprints, size_t &n_out){
    double **fourier_out;
    size_t fourier_slices;
    size_t fourier_slice_size;
    fourierTransform(pcm16, n, (srate*slicems)/1000, step_rate, fourier_out, fourier_slices, fourier_slice_size);
    produceFingerprint(fourier_out, fourier_slices, fuz_factor, fingerprints);
    n_out = fourier_slices;
    for(size_t i=0;i<n_out;i++)
        delete(fourier_out[i]);
    delete(fourier_out);
}
void analyzeandcatalog(Database &db, const char *file, const char *database_file){
    short *pcm16;
    size_t n, n_f;
    string trackname;
    cout<<"Processing file..."<<endl;
    if(decodeMp3(file, srate, trackname, pcm16, n)==false){
        cout<<"Could not decode file \""<<file<<"\""<<endl;
        return;
    }
    if(db.isinbase(trackname)){
        cout<<"skipped \""<<trackname<<"\" - already in database (\""<<database_file<<"\")"<<endl;
        delete(pcm16);
        return;
    }
    unsigned long *fingerprints;
    pcm_to_fingerprints(pcm16, n, fingerprints, n_f);
    db.put(trackname, fingerprints, n_f, database_file);
    cout<<"Added: "<<"\""<<trackname<<"\""<<" to the database (\""<<database_file<<"\") with ["<<n_f<<"] fingerprints"<<endl;
    delete(pcm16);
    delete(fingerprints);
}

void iterate_and_analyze(Database &db, const char *folder){
    DIR *dir;
    struct dirent *ent;
    size_t len;
    string filename;
    string directory = folder;
    directory.append("\\");
    if((dir=opendir(directory.c_str()))!=NULL){

        while((ent = readdir(dir))!=NULL){
            filename=ent->d_name;
            len=filename.size();
            if(len>4 && filename.substr(len-4).find(".mp3")!=string::npos){
                filename=directory+filename;
                analyzeandcatalog(db, filename.c_str(), "db.txt");
            }
        }
        closedir(dir);
    }
    else
        cout<<"Could not open directory \""<<folder<<"\""<<endl;
}
void showresults(vector<match_data> results){
    cout<<"Minimum time matching required: "<<minimum_seconds_matching<<endl<<"results:"<<endl;
    if(results.size()>=1 && results[0].getHits()*step_rate*slicems/1000>=minimum_seconds_matching)cout<<"matches:"<<endl;
    else{
        cout<<"no results found..."<<endl;
        return;
    }
    size_t i=1;
    for(match_data md:results){
        if(md.getHits()*step_rate*slicems/1000<minimum_seconds_matching) break;
        cout<<i<<") ";
        if(i==1)cout<<"[Best match]: ";
        cout<<md.getTrackname()<<", "<<md.getHits()<<" hits("<<md.getHits()*step_rate*slicems/1000<<" seconds), "<<md.getContinuousHits()<<" continuous hits ("<<md.getContinuousHits()*step_rate*slicems/1000<<" seconds)"<<endl;
        i++;
    }
}
void analyzeandsearch(Database &db, const char *file){
    short *pcm16;
    size_t n, n_f;
    string trackname;
    cout<<"Processing file..."<<endl;
    if(decodeMp3(file, srate, trackname, pcm16, n)==false){
        cout<<"Could not decode file \""<<file<<"\""<<endl;
        return;
    }
    unsigned long *fingerprints;
    pcm_to_fingerprints(pcm16, n, fingerprints, n_f);
    vector<match_data> results;
    db.matchsong(fingerprints, n_f, cont_threshold, sort_cont, results);
    showresults(results);
}
void analyzeandcompare(const char *file1, const char *file2){
    Database db;
    short *pcm16;
    size_t n, n_f1, n_f2;
    string trackname;
    cout<<"starting matching..."<<endl;
    if(decodeMp3(file1, srate, trackname, pcm16, n)==false){
        cout<<"Could not decode file \""<<file1<<"\""<<endl;
        return;
    }
    unsigned long *fingerprints;
    pcm_to_fingerprints(pcm16, n, fingerprints, n_f1);
    db.put(file1, fingerprints, n_f1, nullptr);
    if(decodeMp3(file2, srate, trackname, pcm16, n)==false){
        cout<<"Could not decode file \""<<file2<<"\""<<endl;
        return;
    }
    pcm_to_fingerprints(pcm16, n, fingerprints, n_f2);
    vector<match_data> results;
    db.matchsong(fingerprints, n_f2, cont_threshold, sort_cont, results);
    if(results.size()<1)cout<<"The two files share no common fingerprints"<<endl;
    else{
        cout<<"The two files share "<<results[0].getHits()<<" hits, "<<results[0].getHits()*step_rate*slicems/1000<<" seconds (not continuously) and "<<results[0].getContinuousHits()<<" continuous hits ("<<results[0].getContinuousHits()*step_rate*slicems/1000<<" seconds)"<<endl;
        cout<<'\"'<<file1<<"\" matches with \""<<file2<<"\" by "<<100*(float)results[0].getHits()/n_f1<<'\%'<<endl;
        cout<<'\"'<<file2<<"\" matches with \""<<file1<<"\" by "<<100*(float)results[0].getHits()/n_f2<<'\%'<<endl;
    }
}
void recordandsearch(Database &db, const size_t seconds){
    short *pcm16;
    size_t n;
    if(recordAudio(seconds, srate, pcm16, n)==false){
        cout<<"Error, could not record!"<<endl;
        cin>>n;
        return;
    }
    unsigned long *fingerprints;
    pcm_to_fingerprints(pcm16, n, fingerprints, n);
    vector<match_data> results;
    db.matchsong(fingerprints, n, cont_threshold, sort_cont, results);
    showresults(results);
}
inline bool isInteger(const string s){
    for(string::const_iterator k=s.begin(); k!=s.end(); ++k){
        if(!isdigit(*k))return false;
    }
    return true;
}
void clear() {
    COORD topLeft  = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(
        console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    FillConsoleOutputAttribute(
        console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(console, topLeft);
}

enum Options{
    undefined,
    help,
    load,
    save,
    scanfolder,
    searchmp3,
    recordandmatch,
    compare,
    setslidestep,
    setsamplerate,
    setfuzz,
    setminimummatch,
    setfingerprintframe,
    setthreshold,
    setsort,
    spectrogram,
    execute,
    quit,
    optionsend
};
static map<string,Options> stringmap;
static map<Options,int> parmsmap;
void initializecommandmap(){
    stringmap["help"]=help;
    stringmap["load"]=load;
    stringmap["save"]=save;
    stringmap["scan"]=scanfolder;
    stringmap["identify"]=searchmp3;
    stringmap["record"]=recordandmatch;
    stringmap["compare"]=compare;
    stringmap["slidestep"]=setslidestep;
    stringmap["samplerate"]=setsamplerate;
    stringmap["mintime"]=setminimummatch;
    stringmap["fplength"]=setfingerprintframe;
    stringmap["threshold"]=setthreshold;
    stringmap["continuous"]=setsort;
    stringmap["fuzz"]=setfuzz;
    stringmap["spectrogram"]=spectrogram;
    stringmap["exec"]=execute;
    stringmap["quit"]=quit;
    stringmap["exit"]=quit;
}
void initializeparmsmap(){
    parmsmap[help]=1;
    parmsmap[load]=2;
    parmsmap[save]=2;
    parmsmap[scanfolder]=2;
    parmsmap[searchmp3]=2;
    parmsmap[recordandmatch]=2;
    parmsmap[compare]=3;
    parmsmap[setslidestep]=2;
    parmsmap[setsamplerate]=2;
    parmsmap[setminimummatch]=2;
    parmsmap[setfingerprintframe]=2;
    parmsmap[setthreshold]=2;
    parmsmap[setsort]=2;
    parmsmap[setfuzz]=2;
    parmsmap[spectrogram]=2;
    parmsmap[execute]=2;
    parmsmap[quit]=1;
}
void printoptionhelp(Options opt){
    switch(opt){
    case help:
        cout<<"help\t\t\t\tshows list of all possible commands";break;
    case save:
        cout<<"save <filepath>\t\t\texport fingerprint data to file <filepath>";break;
    case load:
        cout<<"load <filepath>\t\t\tloads fingerprint data from database file <filepath>.";break;
    case scanfolder:
        cout<<"scan <path>\t\t\tscan <path> folder/file, fingerprint and save to database.";break;
    case searchmp3:
        cout<<"identify <filepath>\t\tfingerprint mp3 file <filepath> and identify it from the database.";break;
    case recordandmatch:
        cout<<"record <number>\t\t\trecord for <number> seconds and find matches from database.";break;
    case compare:
        cout<<"compare <filepath> <filepath>\tfingerprints both files and then compares them.";break;
    case setslidestep:
        cout<<"slidestep <number>\t\tpercentage of frame to slide when fingerprinting [50]";break;
    case setsamplerate:
        cout<<"samplerate <number>\t\tchange the samples per second to decode audio in [44100] ERASES FINGERPRINTS";break;
    case setminimummatch:
        cout<<"mintime <number>\t\tset the minimum time in seconds that songs have to match to accept [5]";break;
    case setfingerprintframe:
        cout<<"fplength <number>\t\tset the fingerprint frame length in seconds [100]";break;
    case setthreshold:
        cout<<"threshold <number>\t\tset the number of frames that can pass until the next hit is not considered continuous [5]";break;
    case setsort:
        cout<<"continuous <1/0>\t\tset continuous-hit sorting on or off to sort results by continuous hits or sum of hits";break;
    case setfuzz:
        cout<<"fuzz <number>\t\t\tset fuzz factor (make program more tolerant to noise or variations [5]";break;
    case spectrogram:
        cout<<"spectrogram <filepath>\t\texport spectrogram of mp3 file <filepath>.";break;
    case execute:
        cout<<"exec <filepath>\t\t\texecute configuration file (write any commands in a file, then call exec)";break;
    case quit:
        cout<<"exit / quit\t\t\tterminates program.";break;
    default:
        return;
    }
}

void printline(){
    cout<<endl;
    for(size_t i=0; i<120; i++)cout<<'-';
    cout<<endl;
}
void printheader(string h){
    cout<<endl;
    for(size_t i=0;i<46; i++)cout<<'=';
    cout<<"[\t"<<h<<":\t]";
    for(size_t i=0;i<46; i++)cout<<'=';
    cout<<endl<<endl;
}
void printinfo(){
    cout<<endl<<"Audio fingerprinting prototype v0.8, Author George Kiriakidis"<<endl;
    cout<<"Libraries used: mpg123, fftw3, libpng, png++, dirent, zlib"<<endl;
    cout<<"All library rights belong to their respective authors and their work."<<endl;
    cout<<"Type \"help\" for list of options"<<endl;
}
void helpme(){
    printheader("Current settings");
    cout<<"Audio sample rate: "<<srate<<endl;
    cout<<"Fingerprint frame length in ms: "<<slicems<<endl;
    cout<<"(frame) sliding: "<<step_rate*100<<"\%, "<<slicems*step_rate<<" milliseconds sliding"<<endl;
    cout<<"File to export fingerprint database to: "<<dbfile<<endl;
    cout<<"Minimum matching time to accept results: "<<minimum_seconds_matching<<endl;
    cout<<"Fuzz factor (tolerance to variation): "<<fuz_factor<<endl;
    printheader("Available commands");
    for(int i=undefined+1; i<optionsend; i++){
        printoptionhelp(Options(i));
        printline();
    }
}

void printerror(Options o){
    cout<<"Incorrect usage of command:"<<endl;
    printoptionhelp(o);
}

int limitNumber(string s, int mi, int ma){
    if(!isInteger(s))return -1;
    stringstream ss(s);
    int r;
    ss>>r;
    if(r<mi || r>ma)return -1;
    return r;
}

bool applyCommand(Database &db, string command, bool iscfg){
    istringstream iss(command);
    vector<string> cmds;
    string foo;
    while(iss>>foo)
        cmds.push_back(foo);
    clear();
    Options o=stringmap[cmds[0]];
    if(parmsmap[o]!=cmds.size() && o!=undefined){
        cout<<"Wrong number of parameters (should be "<<parmsmap[o]<<" instead of "<<cmds.size()<<endl;
        printerror(o);
        cout<<endl;
        return true;
    }
    switch(o){

    case help:
        helpme();
        break;
    case save:
        if(db.exportdb(dbfile.c_str(), cmds[1].c_str()))
            cout<<"Exported from database file \""<<dbfile<<"\" to \""<<cmds[1]<<"\""<<endl;
        else
            cout<<"Failed to export database data"<<endl;
        break;
    case load:
        db.loadfromfile(cmds[1].c_str());
        break;
    case scanfolder:
        if(cmds[1].size()>4 && cmds[1].substr(cmds[1].size()-4).find(".mp3")!=string::npos)
            analyzeandcatalog(db, cmds[1].c_str(), dbfile.c_str());
        else
            iterate_and_analyze(db, cmds[1].c_str());
        break;
    case searchmp3:
        analyzeandsearch(db, cmds[1].c_str());
        break;
    case recordandmatch:
        int seconds;
        seconds = limitNumber(cmds[1], minimum_seconds_matching, 600);
        if(seconds==-1){
            cout<<"You can only record for seconds in range ["<<minimum_seconds_matching<<'-'<<600<<"]s"<<endl;
            break;
        }
        cout<<"Recording for "<<seconds<<" seconds, please wait..."<<endl;
        recordandsearch(db, seconds);
        break;
    case compare:
        analyzeandcompare(cmds[1].c_str(), cmds[2].c_str());
        break;
    case setslidestep:
        int sstep;
        sstep=limitNumber(cmds[1].c_str(), 10, 100);
        if(sstep==-1){
            cout<<"Fingerprinting cannot slide for values outside of range ["<<10<<'-'<<100<<"]\%"<<endl;
            break;
        }
        step_rate=(float)sstep/100;
        cout<<"New slide step is "<<sstep<<"\%"<<endl<<"Database emptied"<<endl;
        if(iscfg) db.clearbase();
        break;
    case setsamplerate:
        int rate;
        rate=limitNumber(cmds[1].c_str(), 22050, 44100);
        if(sstep==-1){
            cout<<"Sample rate should be in range ["<<22050<<'-'<<44100<<"] samples per second"<<endl;
            break;
        }
        srate=rate;
        cout<<"New sample rate is "<<srate<<endl<<"Database emptied"<<endl;
        if(!iscfg) db.clearbase();
        break;
    case setfingerprintframe:
        int fp;
        fp=limitNumber(cmds[1].c_str(), 40, 1000);
        if(fp==-1){
            cout<<"Fingerprint time frame should be in range ["<<40<<'-'<<1000<<"]ms"<<endl;
            break;
        }
        slicems=fp;
        cout<<"New fingerprint time frame is "<<slicems<<"\%"<<endl<<"Database emptied"<<endl;
        if(!iscfg) db.clearbase();
        break;
    case setthreshold:
        int th;
        th=limitNumber(cmds[1].c_str(), 1, 1000);
        if(th==-1){
            cout<<"Threshold should be in range ["<<1<<'-'<<1000<<"] frames"<<endl;
            break;
        }
        cont_threshold=th;
        break;
    case setsort:
        int so;
        so=limitNumber(cmds[1].c_str(), 0, 1);
        if(so==-1){
            cout<<"Sorting value is either 1 for continuous-hit sorting or 0 for hit-sum sorting."<<endl;
            break;
        }
        sort_cont=(so==1);
        break;
    case setfuzz:
        int tole;
        tole=limitNumber(cmds[1].c_str(), 1, 10);
        if(fp==-1){
            cout<<"Fingerprint tolerance should be in range ["<<1<<'-'<<10<<"]ms"<<endl;
            break;
        }
        fuz_factor=tole;
        cout<<"New fingerprint tolerance is "<<tole<<endl<<"Database emptied"<<endl;
        if(!iscfg) db.clearbase();
        break;
    case setminimummatch:
        int t;
        t=limitNumber(cmds[1].c_str(), 2, 60*60);
        if(t==-1){
            cout<<"Minimum matching time should be in range ["<<2<<'-'<<60*60<<"] seconds"<<endl;
            break;
        }
        minimum_seconds_matching=t;
        cout<<"New minimum matching time requirement is "<<t<<" seconds"<<endl<<"Database emptied"<<endl;
        if(!iscfg) db.clearbase();
        break;
    case spectrogram:
        long *fingerprints;
        short *pcm16;
        size_t n;
        if(decodeMp3(cmds[1].c_str(), srate, command, pcm16, n)==false)
        {
            cout<<"Could not decode file \""<<cmds[1].c_str()<<"\""<<endl;
            break;
        }
        cout<<"Analyzing mp3 file..."<<endl;
        command = cmds[1];
        command=command.substr(0, command.size()-3).append("png");
        double **f_out;
        size_t f_n;
        size_t f_slicesz;
        fourierTransform(pcm16, n, (slicems*srate)/1000, step_rate, f_out, f_n, f_slicesz);
        exportspectrum(f_out, f_n, f_slicesz, 1, command.c_str());
        cout<<"Written to \""<<command<<"\""<<endl;
        break;
    case execute:
        {
        ifstream ifs(cmds[1]);
        if(ifs.is_open()){
            cout<<"Executing \""<<cmds[1]<<"\"..."<<endl;
            while(getline(ifs, command))
                applyCommand(db, command, true);
        }
        else
            cout<<"Could not find/execute \""<<cmds[1]<<"\"..."<<endl;
        break;
        }
    case quit:return false;
    default:
        cout<<"Command \""<<command<<"\" not recognised, type help for list of options..."<<endl;
    }
    return true;
}

int main(int argc, char* argv[])
{
    initializecommandmap();
    initializeparmsmap();
    Database *db = new Database();
    string command;
    ifstream ifs("config.cfg");
    if(ifs.is_open()){
        while(getline(ifs, command))
            applyCommand(*db, command, true);
        ifs.close();
    }
    else
    {
        cout<<"Could not find/open config.cfg, creating new one..."<<endl;
        ofstream ofs("config.cfg");
        ofs<<"samplerate "<<44100<<endl;
        ofs<<"fplength "<<100<<endl;
        ofs<<"fuzz "<<3<<endl;
        ofs<<"mintime "<<20<<endl;
        ofs<<"slidestep "<<50<<endl;
        ofs<<"continuous "<<1<<endl;
        ofs<<"threshold "<<1<<endl;
        ofs<<"load "<<dbfile<<endl;
        ofs<<"scan tracks"<<endl;
        ofs.close();
        ifs.open("config.cfg");
        while(getline(ifs, command))
            applyCommand(*db, command, true);
        ifs.close();
    }
    if(argc>1){
        cout<<endl<<"Processing parameters, please wait..."<<endl<<endl;
        int i;
        for(i=1; i<argc; i++){
            command = argv[i];
            if(command.size()>4 && command.substr(command.size()-4).find(".mp3")!=string::npos)
                analyzeandcatalog(*db, command.c_str(), dbfile.c_str());
            else
                iterate_and_analyze(*db, command.c_str());
        }

    }
    while(true){
        printinfo();
        cout<<endl<<">>";
        getline(cin, command);
        if(applyCommand(*db, command, false)==false)break;
    }
}
