#ifndef OSAPI_MORTAL_THREAD_H
#define OSAPI_MORTAL_THREAD_H
 
 
class MortalThread : public Thread
{
	
		private:
				sig_atomic_t killSignal {};
	
		public:
				MortalThread(int priority, unsigned int stackSize, const char* name = "unnamed") : Thread(priority, stackSize, JOINABLE, name) {}
		 
				virtual ~MortalThread() {}                   
														
				/** Sends termination signal to the thread. */
				void kill() {
					killSignal = 1;
				}
				
		protected:
				virtual void begin() = 0;
				virtual void loop() = 0;
				virtual void end() = 0;
													
		private:
				/** Implementation of the job method */
				void job(void) {
						begin();
						while (!killSignal) loop();
						end();
				}           
};

#endif // OSAPI_MORTAL_THREAD_H
