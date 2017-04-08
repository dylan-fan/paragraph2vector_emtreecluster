/**
 * This file contains helper functions for loading and generating signatures.
 */
#ifndef LOADSIGNATURES_H
#define	LOADSIGNATURES_H

#include "lmw/StdIncludes.h"
#include <cstdlib>

void genData(vector<SVector<bool>*> &vectors, size_t sigSize, size_t numVectors) {

    // Setup Beroulli random number generator
    RND_ENG eng((unsigned int) std::time(0));
    RND_BERN bd(0.5);
    RND_BERN_GEN_01 gen(eng, bd);

    // Define the types we want to use
    typedef VectorGenerator<RND_BERN_GEN_01, SVector<bool>> vecGenerator;

    // Create the seeder
    //seederType seeder;

    // Data.
    vecGenerator::genVectors(vectors, numVectors, gen, sigSize);
}

void readSignatures(vector<SVector<bool>*> &vectors, string docidFile, string signatureFile,
        size_t sigSize, size_t maxVectors) {
    using namespace std;
    cout << docidFile << endl << signatureFile << endl;

    // setup streams
    const size_t numBytes = sigSize / 8;
    char *data = new char[numBytes];
    ifstream docidStream(docidFile);
    ifstream sigStream(signatureFile, ios::in | ios::binary);
    string docid;
    if (!docidStream || !sigStream) {
        cout << "unable to open file" << endl;
        return;
    }

    // read data
    while (getline(docidStream, docid)) {
        sigStream.read(data, numBytes);
        SVector<bool>* vector = new SVector<bool>(data, sigSize);
        vector->setID(docid);
        vectors.push_back(vector);
        if (vectors.size() % 1000 == 0) {
            cout << "." << flush;
        }
        if (vectors.size() % 100000 == 0) {
            cout << vectors.size() << flush;
        }
        if (maxVectors != -1 && vectors.size() == maxVectors) {
            break;
        }
    }
    cout << endl << vectors.size() << endl;
    delete[] data;
}

void loadWikiSignatures(vector<SVector<bool>*>& vectors, int veccount) {
    const char docidFile[] = "data/wikisignatures/wiki.4096.docids";
    const char signatureFile[] = "data/wikisignatures/wiki.4096.sig";
    const size_t signatureLength = 4096;
    readSignatures(vectors, docidFile, signatureFile, signatureLength, veccount);
}


// add  by fantao at 2015-8-16 ;
void loadSubset_doc2vec(char * doc2vecFile, size_t vec_length, vector<SVector<double>*>& vectors, int max_subset_count){
	//const char doc2vecFile[] = "data/doc2vec.txt";
	using namespace std;
	
	string vec_str;
	//const size_t vec_length = 200;

	ifstream doc2vecStream(doc2vecFile);
	long long  line_num = 0;

	// get sample data size;
	vector<string> docids;

	if (!doc2vecStream) {
        cout << "unable to open file" << endl;
        return;
    }
	
	while(getline(doc2vecStream, vec_str)){
		line_num++;	
		int pos = vec_str.find(" ", 0);
		if (pos < vec_str.size() && pos >= 0 ){
			string docid = vec_str.substr(0, pos);
			docids.push_back(docid);			
			}	
		
		}

	random_shuffle(docids.begin(), docids.end());

	int sample_size = line_num * 0.1;

	if (sample_size > max_subset_count ){
		sample_size = max_subset_count;
		}

	set<string> subdocids;

	for (int i = 0; i < sample_size; i++){
		subdocids.insert(docids[i]);
		}
	
	doc2vecStream.close();
	doc2vecStream.clear();
	doc2vecStream.open(doc2vecFile);
	while(getline(doc2vecStream, vec_str)){
		int size = vec_str.size();
		int flag = 0;

		int vec_pos = 0;
		string v1;
		string docid;

		SVector<double>* vector = NULL;

		for(int i = 0; i < size; i++){
			int pos = vec_str.find(" ", i);
			if (pos < size && pos >= 0 ){
				v1 = vec_str.substr(i, pos - i);				
				i = pos;				           
				}            
			else{				 
				v1 = vec_str.substr(i, size-i);				
				i=size;            
				}
			
			if (flag == 0){
				docid = v1;
				flag = 1;				

				if (subdocids.find(docid) == subdocids.end())
					break;	
				
				vector = new SVector<double>(vec_length);
				vector->setID(docid);				
				}
			
			else{			
				double vec = atof(v1.c_str());
				vector->set(vec_pos, vec);
				vec_pos++;
				
				}
			
			}

		
		if (subdocids.find(docid) != subdocids.end()){			
			vectors.push_back(vector);
			}
		
       /*
		if (vectors.size() % 1000 == 0) {
            cout << "." << flush;
        }
        */
		
				
		}	
	
}

void loadSubset(vector<SVector<bool>*>& vectors, vector<SVector<bool>*>& subset,
        string docidFile) {
    using namespace std;
    ifstream docidStream(docidFile);
    string docid;
    set<string> docids;
    while (getline(docidStream, docid)) {
        docids.insert(docid);
    }
    for (SVector<bool>* vector : vectors) {
        if (docids.find(vector->getID()) != docids.end()) {
            subset.push_back(vector);
        }
    }
    cout << "filtered " << subset.size() << " vectors to create the INEX 2010 XML Mining subset" << endl;
}

void testReadVectors() {
    vector < SVector<bool>*> vectors;
    loadWikiSignatures(vectors, 100 * 1000);
}

#endif	/* LOADSIGNATURES_H */

