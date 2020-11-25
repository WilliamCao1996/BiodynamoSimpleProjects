# CellDistribution
This is a simulation recording position of every cell in a tumor.

The simulation starts with one single cancer cell.

It grows and divides without limit. Eventually you get a lot of cancer cells and they form a tumor.

Each timestep, the cell moves a random distance (within a range you set) in a random direction.

As a result, the tumor is not spherical. It can take on a variety of irregular shapes.

After the formation of the tumor, the programme will output the x coordinate of every cell.

You may edit src/CellPosition.h file to make it output y coordinate or z coordinate.

You may also adjust the migrate ability of cell in src/CellPosition.h file.

Obviously, the stronger the cell's ability to migrate, the more dispersed the cells are in space.

Change the random number seed before each experiment starts to make sure the results are random.

If you keep using the same seeds, you'll get exactly the same result over and over again, which is also know as pseudo-random.

If you want to record the simulation result, you may use "script -f result.txt" command. You will get the output recorded in a txt file.
