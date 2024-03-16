///author George Kiriakidis
///libraries used: mpg123, fftw3, libpng, png++
#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;
    class match_data{
        string trackname;
        size_t hits;
        public:
            match_data(string trackname, size_t hits){this->trackname = trackname; this->hits = hits;}
            string getTrackname()const{return this->trackname;}
            size_t getHits()const{return this->hits;}
    };

    class Database{

        unordered_map<long, vector<size_t>> base;
        vector<string> songlist;
        string dbfile;
    public:
        Database(const char *file);
        Database(){}
        bool isinbase(string trackname);
        void put(string trackname, unsigned long *fingerprints, size_t n, const char *file);
        void matchsong(unsigned long *fingerprints, size_t n, vector<match_data> &results);
        bool exportdb(const char *dbfile, const char *file);
        void loadfromfile(const char *file);
        void clearbase();
    };
#endif // DATABASE_H_INCLUDED
