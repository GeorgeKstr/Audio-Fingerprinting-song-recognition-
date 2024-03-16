///author George Kiriakidis
///libraries used: mpg123, fftw3, libpng, png++
#include "decode.h"
#include <iostream>
bool decodeMp3(const char *file, size_t rate, string &trackname, short* &out, size_t &out_n){
    mpg123_init();
    int err;
    string s=file;
    mpg123_handle *mh = mpg123_new(NULL, &err);
	size_t buffer_size = mpg123_outblock(mh);

	mpg123_format_none(mh);
	mpg123_format(mh, rate, 1, MPG123_ENC_SIGNED_16);
	size_t bytes_read;
	size_t lastposition = 0;
	mpg123_id3v1 *v1;
	mpg123_id3v2 *v2;
	int meta;
	if(mpg123_open(mh, file)!=MPG123_OK)
        return false;

	long k;
	int u;
	mpg123_getformat(mh, &k, &u, &u);
    mpg123_scan(mh);

    meta = mpg123_meta_check(mh);

    trackname = "Unknown track";
    if(meta & MPG123_ID3 && mpg123_id3(mh, &v1, &v2)==MPG123_OK){
        ostringstream oss;
        if(v1!=NULL){
            oss<<v1->artist<<'-'<<v1->title;
            trackname = oss.str();
        }
        else if(v2!=NULL){
            oss<<v2->artist<<'-'<<v2->title;
            trackname = oss.str();
        }
    }
    if(trackname=="0-0"){
        trackname=file;
        int i;
        for(i=trackname.size()-1; i>=0 && trackname[i]!='\\'; i--){}
        trackname=trackname.substr(i+1,trackname.size()-4);
    }
	out_n=0;
	size_t length_in_bytes = 2*mpg123_length(mh);

	unsigned char* buffer = (unsigned char*)malloc(length_in_bytes*sizeof(unsigned char));
	while(mpg123_read(mh, buffer+lastposition, buffer_size, &bytes_read)==MPG123_OK){
		lastposition+=bytes_read;
		out_n+=bytes_read/2;
		//std::cout<<length_in_bytes/2;
	}
	out=reinterpret_cast<short*>(buffer);
	mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    return true;
}
