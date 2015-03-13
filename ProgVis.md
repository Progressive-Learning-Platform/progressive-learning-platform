# Purpose #

The PLPTool program visualization provides the users a flowchart view of their program execution flow, allowing easy visualization of program correctness. The visualization is also tied to the simulation core of PLPTool, allowing the users to know which part of the program is being executed, and it also allows easy breakpointing.

# Design #

The program visualizer traverses the program image and populates a directed graph showing possible program execution flow branches.

## Data Structure ##

An object is created for each function that contains the following information:
  * Label
  * Start Address
  * End Address
  * Previous Label
  * Following Label

The actual graph data structure is a two-dimensional doubly-linked list of these function objects. The structure is not unlike a tree. Each node is a function, and node is linked to both its preceding and succeeding objects. For a graph of `functions[i][j]`, with rows `i` and columns `j`, consider each row to correspond to a level in the following flowchart.

![http://progressive-learning-platform.googlecode.com/hg/images/progvis/flowchart.png](http://progressive-learning-platform.googlecode.com/hg/images/progvis/flowchart.png)

## Population ##

The data structure is populated by traversing the program code and creating the objects and array based upon the program's control flow.

## Visualization ##