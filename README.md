# square_solver
Consumer-Produser based solver of quadratic equations

* Working Cycle:
QuadReader -> Buffer -> QuadSolver -> Buffer -> QuadPrinter

* Build:
Execute next commands in project directory:
mkdir build
cd build
cmake ..
make

* Run:
Program can read any sequence of integer numbers that fits to INT32 boundaries
and sepparated by space.
Any other symbols or numbers out of boundaries will be ignored.

- Manual input
./square_solver 1 -2, -3

- File input
./square_solver $(cat filename)

* Output sample:
(1 -2 -3) => (-1, 3)
