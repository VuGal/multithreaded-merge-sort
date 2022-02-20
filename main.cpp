#include <iostream>
#include <chrono>
#include <fstream>
#include "osapi.h"
#include "mergeSortLibrary/mergeSortFunctions.h"
#include "mergeSortLibrary/mergeSortThreads.h"

using namespace std::chrono_literals;

std::vector<int> elementsToSort;

int main(int argc, char **argv) {

	if (argc != 3) {
		std::cout << "Error! Wrong number of parameters passed to the program." << std::endl << std::endl;
		std::cout << "Usage: .\\multithreadedMergeSort.exe <input_file_path> <number_of_threads>" << std::endl << std::endl;
		system("Pause");
		return 0;
	}

	std::ifstream inputfile(argv[1]);
	if (inputfile.is_open()) {
		std::string currentInputFileElement;
		std::cout << "Input file successfully opened!" << std::endl;
	    while (getline(inputfile,currentInputFileElement)) {
	    	try {
	    		elementsToSort.push_back(std::stoi(currentInputFileElement));
	    	}
	    	catch (...){
	    		std::cout << "Error! File format is wrong." << std::endl;
	    		system("Pause");
	    		return 0;
	    	}
	    }
	    inputfile.close();
	}
	else {
		std::cout << "Error! Can't open the input file." << std::endl;
		system("Pause");
		return 0;
	}

	const int elementsToSortCount = elementsToSort.size();

	const int requestedNumberOfThreads = std::stoi(argv[2]);

	std::cout << "Working with " << requestedNumberOfThreads << " thread(s)." << std::endl;

	std::cout << "Elements to sort: " << elementsToSortCount << std::endl;

    // save start time
    const auto startTime = std::chrono::system_clock::now();

    if (requestedNumberOfThreads != 1) {

		MergeSortThread* sortThreads[requestedNumberOfThreads];

		const int excess = elementsToSortCount % requestedNumberOfThreads;

		const int elementsPerThread = elementsToSortCount / requestedNumberOfThreads;

		int numberOfUsedThreads;

		std::vector<int> mergeIndices;
		mergeIndices.push_back(0);

		int tempUpperBound;

		if (elementsPerThread < 2) {

			numberOfUsedThreads = elementsToSortCount / 2;

			std::cout << "Not all threads will be used (too few elements to sort!) Only " << numberOfUsedThreads
					  << " of " << requestedNumberOfThreads << " threads will be used." << std::endl;

			for (int i = 0; i < numberOfUsedThreads-1; ++i) {
				tempUpperBound = 2*i+1;
				sortThreads[i] = new MergeSortThread(elementsToSort, 2*i, tempUpperBound);
				mergeIndices.push_back(tempUpperBound);
			}

			if (elementsToSortCount % 2 == 0) {
				tempUpperBound = 2*(numberOfUsedThreads-1)+1;
				sortThreads[numberOfUsedThreads-1] = new MergeSortThread(elementsToSort, 2*(numberOfUsedThreads-1), tempUpperBound);
				mergeIndices.push_back(tempUpperBound);
			}
			else {
				tempUpperBound = 2*numberOfUsedThreads;
				sortThreads[numberOfUsedThreads-1] = new MergeSortThread(elementsToSort, 2*(numberOfUsedThreads-1), tempUpperBound);
				mergeIndices.push_back(tempUpperBound);
			}

		}

		else {

			numberOfUsedThreads = requestedNumberOfThreads;

			for (int i = 0; i < numberOfUsedThreads - excess; ++i) {
				tempUpperBound = (i+1)*elementsPerThread-1;
				sortThreads[i] = new MergeSortThread(elementsToSort, i*elementsPerThread, tempUpperBound);
				mergeIndices.push_back(tempUpperBound);
			}

			if (excess != 0) {

				int currentElement = (numberOfUsedThreads - excess) * elementsPerThread;

				for (int i = numberOfUsedThreads - excess; i < numberOfUsedThreads; ++i) {
					tempUpperBound = currentElement + elementsPerThread;
					sortThreads[i] = new MergeSortThread(elementsToSort, currentElement, tempUpperBound);
					mergeIndices.push_back(tempUpperBound);
					currentElement = tempUpperBound+1;
				}

			}
		}

		for (int i = 0; i < numberOfUsedThreads; ++i) {
			sortThreads[i]->join(INFINITE);
		}

		int currentMergeIndicesSize;
		int mergeThreadsNumber;
		bool isCurrentMergeIndicesSizeOdd;

		while(true) {

			currentMergeIndicesSize = mergeIndices.size();

			if (currentMergeIndicesSize == 2) break;

			else {

				isCurrentMergeIndicesSizeOdd = currentMergeIndicesSize%2;

				if (isCurrentMergeIndicesSizeOdd) {
					mergeThreadsNumber = currentMergeIndicesSize/2;
				}
				else {
					mergeThreadsNumber = (currentMergeIndicesSize/2) - 1;
				}

				MergeThread* mergeThreads[mergeThreadsNumber];

				for (int i = 0; i < mergeThreadsNumber; ++i) {

					if (i == 0) {
						mergeThreads[0] = new MergeThread(elementsToSort, 0, mergeIndices[1], mergeIndices[2]);
					}
					else {
						mergeThreads[i] = new MergeThread(elementsToSort, mergeIndices[2*i]+1, mergeIndices[2*i+1], mergeIndices[2*(i+1)]);
					}

				}

				for (int i = 0; i < mergeThreadsNumber; ++i) {
					mergeIndices.erase(mergeIndices.begin()+i+1);
				}

				for (int i = 0; i < mergeThreadsNumber; ++i) {
					mergeThreads[i]->join(INFINITE);
				}

			}
		}

    }

    else {
    	mergeSort(elementsToSort, 0, elementsToSortCount-1);	// single-threaded sorting
    }

    // save end time
	const auto endTime = std::chrono::system_clock::now();

	// print duration
	const auto duration = endTime - startTime;
	std::cout << "Sorting ended! Duration: " << std::chrono::duration <double, std::milli>(duration).count() << "ms." << std::endl;

	std::cout << "Saving sorted elements to the output file..." << std::endl;

	std::ofstream outputfile("sortedElements.txt");
	if (outputfile.is_open()) {
		for (int i = 0; i < elementsToSortCount; ++i) {
			outputfile << elementsToSort[i] << "\n";
		}
	}
	else {
		std::cout << "Error! Can't save the output file." << std::endl;
		return 0;
	}

	std::cout << "Finished! ";

	system("Pause");

    return 0;
}
