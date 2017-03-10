# Multi-Level Feedback Queue Scheduler Simulator (course)
This was a mini-project for CSE531 (Operating Systems) taught at
IIIT - Hyderabad in Monsoon-2016 semester.

## Build
```
make
```

## Run
```
./mlfqs INPUTFILE OUTPUTFILE
```

## Input File Format
The input file is formatted as follows
- The first line contains an `N` integer denoting the no. of processes to
  schedule
- The subsequent `N` lines contain the description of each process as four
  space separated integers `PID ARRIVAL_TIME BURST_TIME PRIORITY`
	where
		- `PID`: pid of the process
		- `ARRIVAL_TIME`: the time at which process arrives. The actual time in
		  secondsa and microseconds is scaled up to yield integer values.
		- `BURST_TIME`: the expected run duration of the process in the same
		  scaled time-units as the arrival time
		- `PRIORITY`: the priority of the process (lower number is higher
		  priority)
- Example:
```
5
1 0 14 2
2 7 8 1
3 3 10 0
4 5 7 2
5 1 5 3
```

## Output File Format
The output file format is as follows:
- There are `N` lines, one per process describing the details of execution
  of that process as four space separated integers: `PID`, `RESPONSE_TIME`,
	`FINISH_TIME` and `WAITING_TIME`
- Example (For the previously given input)
```
1 0 44 30
2 0 31 16
3 0 41 28
4 6 34 22
5 14 35 29
```

## Queue Behaviour
Here, we implement a Multilevel Feedback Queue with two levels:
**Level 1** : Fixed priority preemptive Scheduling.
**Level 2** : Round Robin Scheduling

- Fixed priority preemptive Scheduling ( Queue 1)
    - Priority 0 is highest priority.
    - Quantum : 4 unit time
    - Preemptive: If one process e.g. P1 is scheduled and running, 
		  now another process with higher priority comes e.g.P2. New process 
			(high priority) process P2 preempts currently running process P1 
			and process P1 will go to second level queue.

- Round Robin Scheduling (Queue 2)
    - Quantum : 4 unit time.
    - All the processes in second level queue will complete their execution
		  according to round robin scheduling.
    - Queue 2 will be processed after Queue 1 becomes empty.
    - Priority of Queue 2 is lower than that of Queue 1.

Suppose Queue 1 is empty and currently process from Queue 2 is being 
executed Now, If at this time a new process arrives then new process 
will be added to Queue 1. So, new process should be scheduled since 
Queue 1 has higher priority than Queue 2. Again after Queue 1
becomes empty Queue 2 will resume execution.
