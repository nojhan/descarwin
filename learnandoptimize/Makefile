# This makefile is on purpose not made with configure, to show how to use the library
# The make file requires that the fann library is installed (see ../README)


SOURCES = learnoptimize.cpp mappinglearnertest.cpp planner.cpp retrain.cpp mappinglearnerunittest.cpp optimizerunittest.cpp genetransfererunittest.cpp

all: learnoptimize mappinglearnertest planner.exe retrain mappinglearnerunittest optimizerunittest genetransfererunittest


learnoptimize: learnoptimize.cpp optimizer.h instanceHandler.h mappingLearner.h globals.h genetransferer.h mappingLearnerFANN.h mappingLearnerSharkFFNET.h genetransferer.h

	g++ learnoptimize.cpp   -lm libfloatfann.a libshark.a -o learnoptimize


mappinglearnertest: mappinglearnertest.cpp optimizer.h instanceHandler.h mappingLearner.h globals.h genetransferer.h mappingLearnerFANN.h mappingLearnerSharkFFNET.h genetransferer.h

	g++ mappinglearnertest.cpp -lm libfloatfann.a libshark.a -o mappinglearnertest


planner.exe: planner.cpp optimizer.h instanceHandler.h mappingLearner.h globals.h genetransferer.h mappingLearnerFANN.h mappingLearnerSharkFFNET.h genetransferer.h

	g++ planner.cpp -lm libfloatfann.a libshark.a -o planner.exe

retrain: retrain.cpp optimizer.h instanceHandler.h mappingLearner.h globals.h genetransferer.h mappingLearnerFANN.h mappingLearnerSharkFFNET.h    genetransferer.h

	g++ retrain.cpp -lm libfloatfann.a libshark.a -o retrain

mappinglearnerunittest: mappinglearnerunittest.cpp mappingLearnerFANN.h globals.h mappingLearnerFANN.h mappingLearnerSharkFFNET.h genetransferer.h
	g++ mappinglearnerunittest.cpp -lm libfloatfann.a libshark.a -o mappinglearnerunittest

optimizerunittest: optimizerunittest.cpp optimizer.h globals.h instanceHandler.h
	g++ optimizerunittest.cpp -lm libfloatfann.a libshark.a -o optimizerunittest

genetransfererunittest: genetransfererunittest.cpp optimizer.h globals.h genetransferer.h instanceHandler.h
	g++ genetransfererunittest.cpp -lm libfloatfann.a libshark.a -o genetransfererunittest


