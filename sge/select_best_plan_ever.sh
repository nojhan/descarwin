#!/bin/bash

grep -H Makespan plans*/plan.soln* | awk '{ print $3 " " $1 }' | sort -n | head -n1

