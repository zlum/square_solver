#include "bignumber/bignumberbuilder.h"
#include "producerconsumer/buffer.hpp"
#include "quadequation/quadprinter.h"
#include "quadequation/quadreader.h"
#include "quadequation/quadsolver.h"

#include <iostream>
#include <memory>
#include <thread>

using namespace std;

// Debug info about main() execution time
//#define DBG_SHOW_ELAPSED

#ifdef DBG_SHOW_ELAPSED
using start_time_t = chrono::time_point<chrono::high_resolution_clock>;

int64_t calcElapsedMS(const start_time_t& begin)
{
    auto end = chrono::high_resolution_clock::now();
    auto dur = end - begin;
    auto ms = chrono::duration_cast<chrono::milliseconds>(dur).count();

    return ms;
}
#endif // DBG_SHOW_ELAPSED

int main(int argc, char* argv[])
{
#ifdef DBG_SHOW_ELAPSED
    auto begin = chrono::high_resolution_clock::now();
#endif // DBG_SHOW_ELAPSED

    // Create shared buffer objects to transmit tasks
    // from producers to consumers
    auto bufCoeffs = make_shared<Buffer<unique_ptr<QuadCoeffs>>>();
    auto bufEquation = make_shared<Buffer<unique_ptr<QuadEquation>>>();

    //
    auto builderCoeff = make_unique<BigNumberBuilder>();

    // Create ProducerConsumer instances to read, solve and print
    // quadratic equations
    QuadReader reader(argc, argv, bufCoeffs, move(builderCoeff));
    QuadSolver solver(bufCoeffs, bufEquation);
    QuadPrinter printer(bufEquation);

    // Start worker threads of producers and consumers
    reader.start();
    solver.start();
    printer.start();

    // Stop threads after theirs work done
    reader.join();
    solver.stopLater();
    solver.join();
    printer.stopLater();
    printer.join();

#ifdef DBG_SHOW_ELAPSED
    cout << endl << "Overall elapsed time " <<
            calcElapsedMS(begin) << " ms" << endl;
#endif // DBG_SHOW_ELAPSED

    // Producers and Consumers threads also joins in destructors

    return 0;
}
