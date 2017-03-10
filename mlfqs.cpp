#include <bits/stdc++.h>
using namespace std;

typedef pair<int, int> pii;
typedef pair< pii, int> piii;
typedef vector<piii> vpiii;

#define PRIO(x) (((x).first ).first )
#define PID(x)  (((x).first ).second)
#define IND(x)  ( (x).second)

#define MAKE_TRIO(x, y, z) make_pair( make_pair (x, y), z)

#ifdef DEBUG
	#define D(x) x
#else
	#define D(x)
#endif

typedef struct {
	int pid;
	int tm_arrvl;
	int burst;
	int tm_rem;
	int prio;

	int tm_st;
	int tm_fin;
} proc_t;

bool 
comp_tm_arrvl (proc_t p1, proc_t p2) 
{
	return (p1.tm_arrvl <  p2.tm_arrvl) ? true : 
		((p1.tm_arrvl == p2.tm_arrvl) ? (p1.pid < p2.pid) : false);
}

bool 
comp_pid (proc_t p1, proc_t p2) 
{
	return (p1.pid <  p2.pid);
}

bool 
prio_lt (proc_t p1, proc_t p2) 
{
	return (p1.prio <  p2.prio) ? true : 
		((p1.prio == p2.prio) ? (p1.pid < p2.pid) : false);
}

const int qntm = 4;

int
main (int argc, char *argv[])
{
	if (argc != 3) {
		fprintf (stderr, "Invalid Format\n"
									 	 "Usage: mlfqs input_file output_file\n");
		exit (1);
	}

	FILE *infp = fopen (argv[1], "r");

	int N; /* No. of processes */
	fscanf (infp, "%d", &N);

	vector<proc_t> proc (N); /* process list */
	priority_queue< piii, vpiii, greater<piii> >lvl_prio; /* Level-1 Queue */
	deque<piii> lvl_rr; /* Level-2 Queue */

	/* Read input and populate process list */
	for (int i = 0; i < N; i++) {
		proc_t t;
		fscanf (infp, "%d%d%d%d", &(t.pid), &(t.tm_arrvl), &(t.tm_rem), 
		        &(t.prio));
		t.burst = t.tm_rem;
		proc[i] = t;
	}
	fclose (infp);
	
	/* Sort the process list in ascending order of arrival times
	 * of processes. The process list, thus, also serves as a list
	 * of incoming processes for the scheduler
	 */
	sort (proc.begin (), proc.end (), comp_tm_arrvl);
	
	int glob_tm;	/* The global clock time */
	int n_expired = 0;	/* No. of expired (i.e. exited) processes */
	int n_arr = 0; /* No. of process arrived as of now */
	
	/* Warp is set when we have a gap of inactivity between which
	 * all process that have arrived as of now have already exited
	 * but there are still more to come after some time */
	bool warp;
	
	glob_tm = 0;
	warp = true;	/* The start case is the first and default warp */
	
	int tm_arrvl, tm_slexp, tm_procexp, tm_min, tm_slstart;
	int icur;
	piii cur;
	
	int tm_arrvl_rr, tm_slexp_rr, tm_procexp_rr, tm_min_rr, tm_slstart_rr;
	int icur_rr;
	piii cur_rr;

	tm_slstart_rr = tm_slstart = glob_tm;

	D( printf ("[%d] Scheduling started!\n", glob_tm); )
	
	while (n_expired < N) {
		if (warp) {
			tm_min = INT_MAX;

			D( printf ("[%d] Warping to time [%d]\n", glob_tm, 
			   proc[n_arr].tm_arrvl); )
			
			warp = false; /* End of warp */
			
			do {
				proc_t t = proc[n_arr];
				tm_arrvl = t.tm_arrvl;
				tm_min = min (tm_min, tm_arrvl);

				if (tm_min == tm_arrvl) {
					lvl_prio.push (MAKE_TRIO (t.prio, t.pid, n_arr));
					D( printf ("[%d] WARP: Arrived: %d\n", tm_arrvl, t.pid);)
					n_arr++;
				}
			} while (n_arr < N && tm_min == tm_arrvl);
			
			cur = lvl_prio.top ();
			icur = IND (cur);
			
			glob_tm = proc[icur].tm_arrvl;
			tm_slstart = glob_tm;
		}

		if (lvl_prio.size ()) {
			int ct_arr = 0;
			bool popped = false;
			tm_min = INT_MAX;
			
			if (proc[icur].tm_rem	== proc[icur].burst)
				proc[icur].tm_st = glob_tm;

			D( printf ("[%d] Currently running: %d\n", glob_tm, proc[icur].pid);)
			do { 
				tm_arrvl = (n_arr < N) ? proc[n_arr].tm_arrvl : INT_MAX;
				tm_slexp = (icur >= 0) ? tm_slstart + qntm : INT_MAX;
				tm_procexp = (icur >= 0) ? glob_tm + proc[icur].tm_rem : INT_MAX;

				tm_min = min (tm_min, tm_procexp);
				tm_min = min (tm_min, tm_slexp);
				tm_min = min (tm_min, tm_arrvl);
				
			if (tm_min == tm_arrvl) {
					proc_t t = proc[n_arr];
					if (!popped && prio_lt (t, proc[icur])) {
						lvl_prio.pop ();
						popped = true;
					}

					lvl_prio.push (MAKE_TRIO (t.prio, t.pid, n_arr));
					D( printf ("[%d] Arrived: %d\n", tm_arrvl, t.pid);)
					n_arr++;
					ct_arr++;
				}
			} while (tm_min == tm_arrvl);
			
			if (tm_min == tm_procexp) {
				D( printf ("[%d] Exiting: %d\n", tm_procexp, proc[icur].pid);)
				proc[icur].tm_fin = tm_procexp;
				n_expired++;
				glob_tm = tm_procexp;
				tm_slstart = glob_tm;
				if (!popped)
					lvl_prio.pop (); 
				if (lvl_prio.size ()) {
					cur = lvl_prio.top ();
					icur = IND (cur);
					D( printf ("\tNext to run: %d\n", proc[icur].pid);)
				}
			} else if (tm_min == tm_slexp) {
				D( printf ("[%d] Time slice over: %d\n", tm_slexp, proc[icur].pid);)
				proc[icur].tm_rem -= qntm;
				glob_tm = tm_slexp;
				tm_slstart = glob_tm;
				if (!popped)
					lvl_prio.pop ();
				lvl_rr.push_back (cur);
				
				if (lvl_prio.size ()) {
					cur = lvl_prio.top ();
					icur = IND (cur);
					D( printf ("\tNext to run: %d\n", proc[icur].pid);)
				}
			} else if (ct_arr > 0 && popped) {
				D( printf ("[%d] Preempting: %d\n", tm_min, proc[icur].pid);)
				D( printf ("\tsubtracted %d\n", tm_min - tm_slstart); )
				proc[icur].tm_rem -= (tm_min - tm_slstart);
				glob_tm = tm_min;
				tm_slstart = glob_tm;
				lvl_rr.push_back (cur);
				cur = lvl_prio.top ();
				icur = IND (cur);
			}
		} else if (lvl_rr.size ()) {
			int ct_arr = 0;
			cur_rr = lvl_rr.front ();
			icur_rr = IND (cur_rr);
			tm_slstart_rr = glob_tm;
			icur = -1; /* No process in priority queue */
			tm_min_rr = INT_MAX;

			D( printf ("[%d] RR: Currently running: %d\n", glob_tm, 
				 proc[icur_rr].pid);)
			do { 
				tm_arrvl_rr = (n_arr < N) ? proc[n_arr].tm_arrvl : INT_MAX;
				tm_slexp_rr = (icur_rr >= 0) ? tm_slstart_rr + qntm : INT_MAX;
				tm_procexp_rr = (icur_rr >= 0) ? 
				                glob_tm + proc[icur_rr].tm_rem : INT_MAX;

				tm_min_rr = min (tm_min_rr, tm_procexp_rr);
				tm_min_rr = min (tm_min_rr, tm_slexp_rr);
				tm_min_rr = min (tm_min_rr, tm_arrvl_rr);

				if (tm_min_rr == tm_arrvl_rr) {
					proc_t t = proc[n_arr];
					lvl_prio.push (MAKE_TRIO (t.prio, t.pid, n_arr));
					D( printf ("[%d] RR: Arrived: %d\n", tm_arrvl_rr, t.pid);)
					n_arr++;
					ct_arr++;
				}
			} while (tm_min_rr == tm_arrvl_rr);
			
			if (tm_min_rr == tm_procexp_rr) {
				D( printf ("[%d] RR: Exiting: %d\n", tm_procexp_rr, 
				   proc[icur_rr].pid);)
				
				proc[icur_rr].tm_fin = tm_procexp_rr;
				n_expired++;
				glob_tm = tm_procexp_rr;
				tm_slstart_rr = glob_tm;
				lvl_rr.pop_front ();
			} else if (tm_min_rr == tm_slexp_rr) {
				D( printf ("[%d] RR: Time slice over: %d\n", tm_slexp_rr, 
				   proc[icur_rr].pid);)
			
				proc[icur_rr].tm_rem -= qntm;
				glob_tm = tm_slexp_rr;
				tm_slexp_rr = glob_tm;
				lvl_rr.pop_front ();
				lvl_rr.push_back (cur_rr);
			} else if (ct_arr > 0) {
				D( printf ("[%d] RR: Preempting: %d\n", tm_min_rr, 
				   proc[icur_rr].pid);)
				
				proc[icur_rr].tm_rem -= (tm_min_rr - glob_tm);
				glob_tm = tm_min_rr;
				tm_slstart_rr = glob_tm;
				lvl_rr.pop_front ();
				lvl_rr.push_back (cur_rr);
				cur = lvl_prio.top ();
				icur = IND (cur);
				tm_slstart = glob_tm;
			}
		} else {
			warp = true;
		}
	}

	D( printf ("All processes have exited!\n"); )

	/* Sort process list in the order of ascending pid's for display */
	sort (proc.begin (), proc.end (), comp_pid);

	FILE *outfp = fopen (argv[2], "w");
	vector<proc_t>::iterator it;
	for (it = proc.begin (); it != proc.end (); it++) {
		int pid, resp_time, fin_time, wait_time;
		pid = it->pid;
		resp_time = it->tm_st - it->tm_arrvl;
		fin_time = it->tm_fin;
		wait_time = it->tm_fin - it->tm_arrvl - it->burst;
		
		fprintf (outfp, "%d %d %d %d\n", pid, resp_time, fin_time, wait_time);
	}
	fclose (outfp);
	
	return 0;
}
