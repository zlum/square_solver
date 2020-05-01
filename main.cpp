#include "buffer.hpp"
#include "quadprinter.h"
#include "quadreader.h"
#include "quadsolver.h"

#include <iostream>
#include <memory>
#include <thread>

using namespace std;

int main(int argc, char* argv[])
{
    // Create shared buffer objects to transmit tasks
    // from producers to consumers
    auto bufCoeffs = make_shared<Buffer<QuadCoeffs>>();
    auto bufEquation = make_shared<Buffer<QuadEquation>>();

    // Create ProducerConsumer instances to read, solve and print
    // quadratic equations
    QuadReader reader(argc, argv, bufCoeffs);
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

    // Producers and Consumers threads also joins in ~ProducerConsumer()

    return 0;
}
