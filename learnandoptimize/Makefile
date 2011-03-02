# This makefile is on purpose not made with configure, to show how to use the library
# The make file requires that the fann library is installed (see ../README)


SOURCES = learnoptimize.cpp mappinglearnertest.cpp planner.cpp retrain.cpp

all: learnoptimize mappinglearnertest planner.exe retrain


learnoptimize: learnoptimize.cpp optimizer.h instanceHandler.h mappingLearner.h globals.h genetransferer.h
	g++ learnoptimize.cpp   -lm libfloatfann.a libshark.a -o learnoptimize


mappinglearnertest: mappinglearnertest.cpp optimizer.h instanceHandler.h mappingLearner.h globals.h genetransferer.h
	g++ mappinglearnertest.cpp -lm libfloatfann.a libshark.a -o mappinglearnertest


planner.exe: planner.cpp optimizer.h instanceHandler.h mappingLearner.h globals.h genetransferer.h
	g++ planner.cpp -lm libfloatfann.a libshark.a -o planner.exe

retrain: retrain.cpp optimizer.h instanceHandler.h mappingLearner.h globals.h genetransferer.h
	g++ retrain.cpp -lm libfloatfann.a libshark.a -o retrain


