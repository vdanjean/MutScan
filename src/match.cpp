#include "match.h"
#include <vector>
#include "globalsettings.h"
#include <memory.h>

Match::Match(Read* r, int pos, int distance, bool reversed){
    mRead = r;
    mSequence = NULL;
    mDistance = distance;
    mPos = pos;
    mReversed = reversed;
}

Match::Match(char* seq, char meanQual, int pos, int distance, bool reversed){
    mRead = NULL;
    mSequence = seq;
    mMeanQual = meanQual;
    mDistance = distance;
    mPos = pos;
    mReversed = reversed;
}

Match::~Match(){
    // we don't delete mRead or mSequence here since they are shared by different objects
    // and will be deleted in other places
    for(int i=0;i<mOriginalReads.size();i++){
        delete mOriginalReads[i];
        mOriginalReads[i] = NULL;
    }
}

int Match::readlength() const {
    if(GlobalSettings::simplifiedMode)
        return strlen(mSequence);
    else
        return mRead->length();
}

void Match::addOriginalRead(Read* r){
    mOriginalReads.push_back(new Read(*r));
}

void Match::addOriginalPair(ReadPair* pair){
    mOriginalReads.push_back(new Read(*pair->mLeft));
    mOriginalReads.push_back(new Read(*pair->mRight));
}

void Match::print(int leftlen, int centerlen, int rightlen){
    if(GlobalSettings::simplifiedMode)
        mRead = new Read(mSequence, mMeanQual);
    cout<<"pos: "<<mPos<<", distance: "<<mDistance;
    if(mReversed)
        cout<<", reverse";
    else
        cout<<", forward";
    cout<<endl;
    vector<int> breaks;
    breaks.push_back(max(mPos-leftlen, 0));
    breaks.push_back( mPos );
    breaks.push_back( mPos+centerlen );
    breaks.push_back( min(mPos+centerlen+rightlen, mRead->length()));
    mRead->printWithBreaks(breaks);
    if(GlobalSettings::simplifiedMode) {
        delete mRead;
        mRead = NULL;
    }
}

void Match::printHtmlTD(ofstream& file, int leftlen, int centerlen, int rightlen, int mutid, int matchid){
    if(GlobalSettings::simplifiedMode)
        mRead = new Read(mSequence, mMeanQual);
    file<<"<a title='"<<mRead->mName<<"'>";
    file<<"d:" << mDistance;
    if(mReversed)
        file<<", <--";
    else
        file<<", -->";

    file<<"</a></span>";

    vector<int> breaks;
    breaks.push_back(max(mPos-leftlen, 0));
    breaks.push_back( mPos );
    breaks.push_back( mPos+centerlen );
    breaks.push_back( min(mPos+centerlen+rightlen, mRead->length()));
    mRead->printHtmlTDWithBreaks(file, breaks, mutid, matchid);
    if(GlobalSettings::simplifiedMode) {
        delete mRead;
        mRead = NULL;
    }
}

void Match::printJS(ofstream& file, int leftlen, int centerlen, int rightlen) {
    if(GlobalSettings::simplifiedMode)
        mRead = new Read(mSequence, mMeanQual);
    vector<int> breaks;
    breaks.push_back(max(mPos-leftlen, 0));
    breaks.push_back( mPos );
    breaks.push_back( mPos+centerlen );
    breaks.push_back( min(mPos+centerlen+rightlen, mRead->length()));
    mRead->printJSWithBreaks(file, breaks);
    if(GlobalSettings::simplifiedMode) {
        delete mRead;
        mRead = NULL;
    }
}

void Match::printReadsToFile(ofstream& file){
    for(int i=0;i<mOriginalReads.size();i++){
        mOriginalReads[i]->printFile(file);
    }
}

void Match::setReversed(bool flag){
    mReversed = flag;
}

int Match::countUnique(vector<Match*>& matches) {
    if(matches.size()==0)
        return 0;
    int count = 1;
    Match* cur = matches[0];
    for(int i=1;i<matches.size();i++){
        Match* m = matches[i];
        if( *m > *cur || *m < *cur) {
            cur = m;
            count++;
        }
    }
    return count;
}
