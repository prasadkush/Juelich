Source code files:

1) server.c
2) client.py

Steps to compile the code:

1) clone the repository at a location on your machine
2) On the command line:
   gcc server.c -o server

Steps to run the code:

1) store the data file in data-acquisition-task folder of this repository
2) On the command line, run:
   ./server filename
    NOTE: filename only needs to be the name of the file in data-acquisition-task folder in this repository
    eg: ./server case1.data
3) On another terminal, run the following:
   python3 client.py

The image will be displayed after the codes have run.
