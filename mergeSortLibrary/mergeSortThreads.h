#ifndef MERGESORTTHREADS_H_
#define MERGESORTTHREADS_H_

/**
 *  When this class is instantiated, it performs a merge sort algorithm on the array with
 *  the specified bounds.
*/
class MergeSortThread : public osapi::MortalThread
{
public:
	/**
	 *  A few MergeSortThread objects can work on a same array (but with different bounds that
	 *  are specified in the constructor).
	 *
	 *  @param[in] &a     reference to the array on which the merge sort algorithm will work
	 *  @param[in] l      lower bound of the array
	 *  @param[in] h      upper bound of the array
	 */
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
		// Run the merge sort algorithm, then kill the thread.
		mergeSort(*arr, lower, higher);
		kill();
	}
	virtual void end()
	{
		;
	}
};


/**
 *  This type of thread performs only the merge part of the algorithm (it doesn't divide).
 *  It is used for merging the results from all MergeSortThread instances.
*/
class MergeThread : public osapi::MortalThread
{
public:

	/**
	 *  A few MergeThread objects can work on a same array (but with different bounds that
	 *  are specified in the constructor).
	 *
	 *  @param[in] &a     reference to the array on which the merge algorithm will work
	 *  @param[in] l      lower bound of the array
	 *  @param[in] l      middle of the array
	 *  @param[in] h      upper bound of the array
	 */
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
		// Run the merge algorithm, then kill the thread.
		merge(*arr, lower, mid, higher);
		kill();
	}
	virtual void end()
	{
		;
	}
};


#endif /* MERGESORTTHREADS_H_ */
