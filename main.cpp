#include <iostream>
#include <chrono>
#include <fstream>
#include "osapi.h"
#include "mergeSortFunctions.h"
#include "mergeSortThreads.h"

using namespace std::chrono_literals;


std::vector<int> elementsToSort;	// vector which stores elements read from input file

int main(int argc, char **argv) {

	// Check if passed parameters are correct, if not give a warning and close the program.
	if (argc != 3) {
		std::cout << "Error! Wrong number of parameters passed to the program." << std::endl << std::endl;
		std::cout << "Usage: .\\multithreadedMergeSort.exe <input_file_path> <number_of_threads>" << std::endl << std::endl;
		system("Pause");
		return 0;
	}

	// Open the file (path provided as a first parameter) and copy the elements to the "elementsToSort" vector
	std::ifstream inputfile(argv[1]);
	if (inputfile.is_open()) {
		std::string currentInputFileElement;
		std::cout << "Input file successfully opened!" << std::endl;
	    while (getline(inputfile, currentInputFileElement)) {
	    	try {
	    		// file lines (strings) are converted to integers before storing them in the vector
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

	const int elementsToSortCount = elementsToSort.size();	  // size of the vector which will be sorted

	const int requestedNumberOfThreads = std::stoi(argv[2]);  // number of threads that will be used are provided as a second parameter

	std::cout << "Working with " << requestedNumberOfThreads << " thread(s)." << std::endl;

	std::cout << "Elements to sort: " << elementsToSortCount << std::endl;

    // save start time
    const auto startTime = std::chrono::system_clock::now();

    // multithreaded merge sort
    if (requestedNumberOfThreads != 1) {

		MergeSortThread* sortThreads[requestedNumberOfThreads];		// create sorting threads

		// how many elements will be sorted by each thread
		const int elementsPerThread = elementsToSortCount / requestedNumberOfThreads;

		// if the work can't be equally divided into threads, some of them will take one more element
		// (program knows how many from this parameter)
		const int excess = elementsToSortCount % requestedNumberOfThreads;

		int numberOfUsedThreads;

		// this vector saves the indices that will be used as parameters for the final merging (with MergeThread objects)
		std::vector<int> mergeIndices;
		mergeIndices.push_back(0);

		int tempUpperBound;		// temp variable used for storing the upper bounds in the following sections


		// SORTING PART

		// input array is too small to use all threads
		if (elementsPerThread < 2) {

			numberOfUsedThreads = elementsToSortCount / 2;

			std::cout << "Not all threads will be used (too few elements to sort!) Only " << numberOfUsedThreads
					  << " of " << requestedNumberOfThreads << " threads will be used." << std::endl;

			// instantiating threads (each takes 2 elements to sort) - last thread is omitted here
			for (int i = 0; i < numberOfUsedThreads-1; ++i) {
				tempUpperBound = 2*i+1;
				sortThreads[i] = new MergeSortThread(elementsToSort, 2*i, tempUpperBound);
				mergeIndices.push_back(tempUpperBound);
			}

			// if number of elements is even, last thread takes 2 elements
			if (elementsToSortCount % 2 == 0) {
				tempUpperBound = 2*(numberOfUsedThreads-1)+1;
				sortThreads[numberOfUsedThreads-1] = new MergeSortThread(elementsToSort, 2*(numberOfUsedThreads-1), tempUpperBound);
				mergeIndices.push_back(tempUpperBound);
			}

			// if number of elements is odd, last thread takes 3 elements
			else {
				tempUpperBound = 2*numberOfUsedThreads;
				sortThreads[numberOfUsedThreads-1] = new MergeSortThread(elementsToSort, 2*(numberOfUsedThreads-1), tempUpperBound);
				mergeIndices.push_back(tempUpperBound);
			}

		}

		// input array is large enough to use all threads
		else {

			numberOfUsedThreads = requestedNumberOfThreads;

			// instantating threads that take "elementsPerThread" elements to sort
			for (int i = 0; i < numberOfUsedThreads - excess; ++i) {
				tempUpperBound = (i+1)*elementsPerThread-1;
				sortThreads[i] = new MergeSortThread(elementsToSort, i*elementsPerThread, tempUpperBound);
				mergeIndices.push_back(tempUpperBound);
			}

			// work can't be equally divided - some of the threads will take "elementsPerThread" + 1 elements to sort
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

		// waiting for all threads to finish their sorting work
		for (int i = 0; i < numberOfUsedThreads; ++i) {
			sortThreads[i]->join(INFINITE);
		}


		// MERGING PART

		int currentMergeIndicesSize;
		int mergeThreadsNumber;
		bool isCurrentMergeIndicesSizeOdd;

		// Merging part is also performed by multiple threads. Each thread takes a pair of sorted subvectors before and merge
		// them. "mergeIndices" vector which was filled during sorting part is now used for passing the bounds to MergeSort threads
		// constructors. After each iteration, indices that were used as "mid" parameter are erased and the ones that were used
		// as upper bounds become the "mid" ones. In this way number of subvectors to merge is divided by 2 after each iteration
		// until all subvectors will be merged.
		while(true) {

			currentMergeIndicesSize = mergeIndices.size();

			// "mergeIndices" contains only lower and upper bound of the final vector - merging is completed
			if (currentMergeIndicesSize == 2) break;

			else {

				isCurrentMergeIndicesSizeOdd = currentMergeIndicesSize%2;

				// all threads can take 2 arrays to merge
				if (isCurrentMergeIndicesSizeOdd) {
					mergeThreadsNumber = currentMergeIndicesSize/2;
				}

				// one array will not be merged as it doesn't have a pair
				else {
					mergeThreadsNumber = (currentMergeIndicesSize/2) - 1;
				}

				MergeThread* mergeThreads[mergeThreadsNumber];

				// perform the merging by instantiating MergeThread objects
				for (int i = 0; i < mergeThreadsNumber; ++i) {

					if (i == 0) {
						mergeThreads[0] = new MergeThread(elementsToSort, 0, mergeIndices[1], mergeIndices[2]);
					}
					else {
						mergeThreads[i] = new MergeThread(elementsToSort, mergeIndices[2*i]+1, mergeIndices[2*i+1], mergeIndices[2*(i+1)]);
					}

				}

				// remove the "mid" indices as they are no longer needed (upper bounds will become "mid" ones in the next iterations)
				for (int i = 0; i < mergeThreadsNumber; ++i) {
					mergeIndices.erase(mergeIndices.begin()+i+1);
				}

				// waiting for all threads to finish their merging work
				for (int i = 0; i < mergeThreadsNumber; ++i) {
					mergeThreads[i]->join(INFINITE);
				}

			}
		}

    }

    // single-threaded merge sort
    else {
    	mergeSort(elementsToSort, 0, elementsToSortCount-1);
    }

    // save end time
	const auto endTime = std::chrono::system_clock::now();

	// print duration
	const auto duration = endTime - startTime;
	std::cout << "Sorting ended! Duration: " << std::chrono::duration <double, std::milli>(duration).count() << "ms." << std::endl;

	// saving the sorted elements to the output file
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
