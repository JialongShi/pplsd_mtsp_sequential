
#include "problem.h"
#include "solution.h"
#include "archive.h"
#include "misc.h"
#include "pplsd.h"

using namespace std;

int main(int argc, char *argv[])
{
	if (argc < 5) {
		printf("ERROR: Arguments missing! The arguments should be <problem_filename> <weight_vector_filename> <process_number> <max_runtime>\n");
		return 1;
	}
	//argv[1] is the problem instance filename
	//argv[2] is the weight vector filename
	int numProcs = strtol(argv[3], NULL, 10);
	int maxRuntime = strtol(argv[4], NULL, 10);

	problem inst;
	if (!inst.readProblem_TransferToMaxProblem(argv[1])) {
		printf("ERROR: cannot open problem instance file %s\n", argv[1]);
		return 1;
	}

	int n = inst.n;
	int m = inst.m;

	//the initial archive contains only one randomly generated solution
	//srand(1);
	solution iniSol;
	iniSol.randomInitialize(inst);
	archive iniArchive;
	iniArchive.updateArchive(iniSol);

	//the reference point is set to be equal to the function vector of the randomly generated solution
	vector<double> refPoint(m);
	for (int mi = 0; mi < m; mi++) {
		refPoint[mi] = iniSol.fitness[mi];
	}

	int runtimePerProcs = maxRuntime / numProcs;

	archive finalResult;

	//since this is a sequential version, execute each "process" one by one.
	for (int procsId = 0; procsId < numProcs; procsId++) {
		archive result = PPLSD(inst, argv[2], iniArchive, runtimePerProcs, numProcs, procsId, refPoint);


		char resultFilename[200];
		sprintf(resultFilename, "result_process%d.txt", procsId);
		result.printToFile_RestoreMinProblem(resultFilename);

		printf("Process %d finished, archive size = %d, output file : result_process%d.txt\n", procsId, result.solList.size(), procsId);

		for (list<solution>::iterator itSol = result.solList.begin(); itSol != result.solList.end(); itSol++) {
			if (!finalResult.judgeBeDomd_MaxCase(*itSol)) {
				finalResult.updateArchive(*itSol);
			}
		}
	}

	char finalResultFilename[200];
	sprintf(finalResultFilename, "result_final.txt");
	finalResult.printToFile_RestoreMinProblem(finalResultFilename);

	printf("All processes finished, final archive size = %d, output file : result_final.txt\n", finalResult.solList.size());

	return 0;
}








