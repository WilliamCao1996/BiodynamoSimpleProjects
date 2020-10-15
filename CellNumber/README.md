# CellNumber
This is a very simple simulation about cancer cell division.
The simulation starts with one single cancer cell. 
It grows and divides without limit. Eventually you get a lot of cancer cells and they form a Spherical tumor.
The programme will output the number of cells every 10 timesteps. 
You may adjust the the number of time steps that the simulation takes, or the frequency of the output.
You may adjust the probability of cell division in src/CancerCellNumbers.h file. 
Obviously, the greater the probability of cell division, the greater the total number of cells after the same time step.
Change the random number seed before each experiment starts to make sure the results are random. 
If you keep using the same seeds, you'll get exactly the same result over and over again, which is also know as pseudo-random.
If you want to record the simulation result, you may use "script -f result.txt" command. You will get the output recorded in a txt file.
