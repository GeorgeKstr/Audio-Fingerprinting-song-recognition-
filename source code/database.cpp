///author George Kiriakidis
///libraries used: mpg123, fftw3, libpng, png++
#include "database.h"
#include <iostream>
using namespace std;
Database::Database(const char *file){
    this->dbfile = file;
    loadfromfile(file);
}

void Database::put(string trackname, unsigned long *fingerprints, size_t n, const char *file){
    songlist.push_back(trackname);
    size_t songindex = songlist.size()-1;
    unsigned long fp;
    for(size_t i=0; i<n; i++){
        fp = fingerprints[i];
        auto s = base.insert(make_pair(fp, vector<size_t>()));
        if(find(s.first->second.begin(), s.first->second.end(), songindex)>=s.first->second.end())
            s.first->second.push_back(songindex);
    }
    if(file){
        ofstream ofs;
        ofs.open(file, std::ios_base::app);
        ofs<<endl<<trackname<<'{'<<n<<endl<<'\t';
        for(size_t i=0; i<n; i++){
            ofs<<fingerprints[i]<<' ';
            if((i+1)%30==0)ofs<<endl<<'\t';
        }
        ofs<<endl<<"} ";
    }
}
void Database::matchsong(unsigned long *fingerprints, size_t n, vector<match_data> &results){
    unordered_map<size_t, size_t> r;
    results.clear();
    for(size_t i=0; i<n; i++){
        auto s = base.find(fingerprints[i]);
        if(s!=base.end()){
            for(size_t songindex : s->second)
                r[songindex]++;
        }
    }
    for(auto sd: r)
        results.push_back(match_data(songlist[sd.first], sd.second));
    sort(results.begin(), results.end(), [](const match_data& md1, const match_data& md2)
    {
         return md1.getHits() > md2.getHits();
    });
}

void Database::loadfromfile(const char *file){
    unsigned long *fps;
    size_t num_of_fps;
    string line;
    bool addsong;
    string songname;
    size_t n;
    if(file){
        ifstream ifs;
        ifs.open(file);
        if(ifs.is_open()){

            while(getline(ifs, line, '\n')){
                addsong=true;
                n=line.find('{');
                if(n==string::npos)continue;
                songname=line.substr(0, n);
                for(string s:songlist){
                    if(s==songname){
                        addsong=false;
                        break;
                    }
                }
                num_of_fps=atoi(line.substr(n+1).c_str());
                n=0;
                if(addsong){
                    fps=new unsigned long[num_of_fps];
                    while(getline(ifs, line, ' ') && n<num_of_fps){
                        if(line.find('}')!=string::npos)break;
                        fps[n] = atoi(line.c_str());
                        n++;
                    }
                    put(songname, fps, n, dbfile.c_str());
                    cout<<"loaded: \""<<songname<<"\" with ["<<n<<"] fingerprints from file \""<<file<<"\""<<endl;
                }
                else{
                    getline(ifs, line, '}');
                    cout<<"skipped: \""<<songname<<"\" - already loaded"<<endl;
                }
            }
            ifs.close();
        }
        else
            cout<<"Could not open database file (probably not created yet)"<<endl;
    }
}

bool Database::isinbase(string trackname){
    for(string s:songlist){
        if(s==trackname)return true;
    }
    return false;
}

bool Database::exportdb(const char *dbfile, const char *file){
    ifstream ifs(dbfile);
    ofstream ofs(file);
    string line;
    if(!ifs.is_open())return false;
    if(!ofs.is_open())return false;
    while(getline(ifs, line)){
        ofs<<line<<endl;
    }
    return true;
}

void Database::clearbase(){
    base.clear();
    songlist.clear();
    ofstream ofs(dbfile, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
}
