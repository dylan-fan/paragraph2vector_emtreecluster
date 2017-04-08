// EMTree.cpp : Defines the entry point for the console application.
//

#include "ExperimentTypedefs.h"
#include "CreateSignatures.h"
#include "StreamingEMTreeExperiments.h"
//#include "JournalPaperExperiments.h"
//#include "GeneralExperiments.h"
using namespace std;
int main(int argc, char** argv) {
    std::srand(std::time(0));	
	
	if (argc < 5){
		cerr<<"Usage: emtree [filename] [vector_size] [m-tree] [depth]"<<endl;
		return 1;
	}

	char* doc2vecfile = argv[1];
	
	int vector_length = atoi(argv[2]);
	
	int m = atoi(argv[3]);
	int d = atoi(argv[4]);
	
    streamingEMTree(doc2vecfile, vector_length, m, d);
   
    return EXIT_SUCCESS;
}


