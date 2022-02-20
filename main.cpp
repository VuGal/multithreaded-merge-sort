#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include "osapi.h"

using namespace std::chrono_literals;

// Merges two subarrays of array[].
// First subarray is arr[begin..mid]
// Second subarray is arr[mid+1..end]
void merge(std::vector<int> &array, int const left, int const mid, int const right)
{
    auto const subArrayOne = mid - left + 1;
    auto const subArrayTwo = right - mid;

    // Create temp arrays
    auto *leftArray = new int[subArrayOne],
         *rightArray = new int[subArrayTwo];

    // Copy data to temp arrays leftArray[] and rightArray[]
    for (auto i = 0; i < subArrayOne; i++)
        leftArray[i] = array[left + i];
    for (auto j = 0; j < subArrayTwo; j++)
        rightArray[j] = array[mid + 1 + j];

    auto indexOfSubArrayOne = 0, // Initial index of first sub-array
        indexOfSubArrayTwo = 0; // Initial index of second sub-array
    int indexOfMergedArray = left; // Initial index of merged array

    // Merge the temp arrays back into array[left..right]
    while (indexOfSubArrayOne < subArrayOne && indexOfSubArrayTwo < subArrayTwo) {
        if (leftArray[indexOfSubArrayOne] <= rightArray[indexOfSubArrayTwo]) {
            array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
            indexOfSubArrayOne++;
        }
        else {
            array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }
    // Copy the remaining elements of
    // left[], if there are any
    while (indexOfSubArrayOne < subArrayOne) {
        array[indexOfMergedArray] = leftArray[indexOfSubArrayOne];
        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }
    // Copy the remaining elements of
    // right[], if there are any
    while (indexOfSubArrayTwo < subArrayTwo) {
        array[indexOfMergedArray] = rightArray[indexOfSubArrayTwo];
        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }
}

// begin is for left index and end is
// right index of the sub-array
// of arr to be sorted */
void mergeSort(std::vector<int> &array, int const begin, int const end)
{
	if (begin < end) {
		auto mid = begin + (end - begin) / 2;
		mergeSort(array, begin, mid);
		mergeSort(array, mid + 1, end);
		merge(array, begin, mid, end);
	}
}

class MergeSortThread : public osapi::MortalThread
{
public:
	MergeSortThread(std::vector<int> &a, int l, int h) : MortalThread(0, 0) {
		arr = &a;
		lower = l;
		higher = h;
		run();
	}

private:

	std::vector<int> *arr;
	int lower;
	int higher;

	virtual void begin()
	{
		;
	}
	virtual void loop()
	{
		mergeSort(*arr, lower, higher);
		kill();
	}
	virtual void end()
	{
		;
	}
};

class MergeThread : public osapi::MortalThread
{
public:
	MergeThread(std::vector<int> &a, int l, int m, int h) : MortalThread(0, 0) {
		arr = &a;
		lower = l;
		mid = m;
		higher = h;
		run();
	}

private:

	std::vector<int> *arr;
	int lower;
	int mid;
	int higher;

	virtual void begin()
	{
		;
	}
	virtual void loop()
	{
		merge(*arr, lower, mid, higher);
		kill();
	}
	virtual void end()
	{
		;
	}
};


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
	    	elementsToSort.push_back(std::stoi(currentInputFileElement));
	    }
	    inputfile.close();
	}
	else {
		std::cout << "Error! Can't open the input file." << std::endl;
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

		if (elementsPerThread == 0) {

			numberOfUsedThreads = elementsToSortCount / 2;

			std::cout << "Not all threads will be used (too little elements to sort!) Only " << numberOfUsedThreads
					  << " of " << requestedNumberOfThreads << " threads will be used." << std::endl;

			for (int i = 0; i < numberOfUsedThreads-1; ++i) {
				sortThreads[i] = new MergeSortThread(elementsToSort, 2*i, 2*i+1);
				mergeIndices.push_back(2*i+1);
			}

			if (elementsToSortCount % 2 == 0) {
				sortThreads[numberOfUsedThreads-1] = new MergeSortThread(elementsToSort, 2*(numberOfUsedThreads-1), 2*(numberOfUsedThreads-1)+1);
				mergeIndices.push_back(2*(numberOfUsedThreads-1)+1);
			}
			else {
				sortThreads[numberOfUsedThreads-1] = new MergeSortThread(elementsToSort, 2*(numberOfUsedThreads-1), 2*numberOfUsedThreads);
				mergeIndices.push_back(2*numberOfUsedThreads);
			}

		}

		else {

			numberOfUsedThreads = requestedNumberOfThreads;

			for (int i = 0; i < numberOfUsedThreads - excess; ++i) {
				sortThreads[i] = new MergeSortThread(elementsToSort, i*elementsPerThread, (i+1)*elementsPerThread-1);
				mergeIndices.push_back((i+1)*elementsPerThread-1);
			}

			if (excess != 0) {
				int currentElement = (numberOfUsedThreads - excess) * elementsPerThread;
				int newCurrentElement = 0;

				for (int i = numberOfUsedThreads - excess; i < numberOfUsedThreads; ++i) {
					newCurrentElement = currentElement + elementsPerThread;
					sortThreads[i] = new MergeSortThread(elementsToSort, currentElement, newCurrentElement);
					mergeIndices.push_back(newCurrentElement);
					currentElement = newCurrentElement;
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
					if (i == 0) mergeThreads[0] = new MergeThread(elementsToSort, 0, mergeIndices[1], mergeIndices[2]);
					else mergeThreads[i] = new MergeThread(elementsToSort, mergeIndices[2*i]+1, mergeIndices[2*i+1], mergeIndices[2*(i+1)]);
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
