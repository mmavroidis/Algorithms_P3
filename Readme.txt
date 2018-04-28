Mavroidis Mavroidis 
Seam Carving Project Algorithms

In order to run this project enter in the following commands:

//To Build 
g++ -std=c++14 sc435.cpp -o sc435

//To Run
./sc435 File_name  Number_of_Vert_seams Number_of_Horz_seams


This project takes in a file and populates a pixel matrix. 
From the pixel matrix it then computes the energy matrix and then following the cumulative energy matrix. 
After that the shortest path is then found and keeps track as to what positions need to removed. 
I then remove those positions and resize the matricies and output the file. 