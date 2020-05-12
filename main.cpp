        #include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "Island.h"
#include "List.h"

class ArchipelagoExpedition {
    Island* islandsArray;
    List<std::string> filenames;
    size_t size;

    void reset(int n = 0) {
        delete [] islandsArray;
        size = n;
        if (n) islandsArray = new Island[n];
    }

    bool bfsUtil(int b, List<int>& islandQueue, int previousLocation[]) {
        while (!islandQueue.empty()) {
            int a = islandQueue.front();
            islandQueue.pop_front();

            for (int c : islandsArray[a-1].getReachableIslands()) {
                if (!islandsArray[c-1].isVisited()) {
                    islandsArray[c-1].markVisited();
                    previousLocation[c-1] = a;
                    if (c == b)
                        return true;
                    islandQueue.push_back(c);
                }
            }
        }
        return false;
    }

    List<int> bfs(int x, int y) {
        int prevLocations[size];
        for (int i = 0; i < size; ++i) {
            islandsArray[i].markUnvisited();
            prevLocations[i] = -1;
        }
        List<int> islandQueue;
        List<int> path;
        islandQueue.push_back(x);

        if (!bfsUtil(y, islandQueue, prevLocations))
            printf("You can NOT get from island %d to island %d in one or more ferry rides", x, y);
        else {
            printf("You can get from island %d to island %d in one or more ferry rides\n", x, y);
            int currIsland = y;
            path.push_front(currIsland);
            do {
                currIsland = prevLocations[currIsland - 1];
                path.push_front(currIsland);
            } while (currIsland != x);
        }
        return path;
    }
    
public:

    // Use a constructor to initialize the Data Members for your expedition
    ArchipelagoExpedition() : size(10) {
        islandsArray = new Island[10];
    }


    // The main loop for reading in input
    void processCommandLoop (FILE* inFile)
    {
        char  buffer[300];
        char* input;

        input = fgets ( buffer, 300, inFile );   // get a line of input

        // loop until all lines are read from the input
        while (input != nullptr)
        {
            // process each line of input using the strtok functions
            char* command;
            command = strtok (input , " \r\n\t");

            printf ("*%s*\n", command);

            if ( command == nullptr )
                printf ("Blank Line\n");

            else if ( strcmp (command, "q") == 0)
                exit(1);

            else if ( strcmp (command, "?") == 0)
                showCommands();

            else if ( strcmp (command, "t") == 0)
                doShortestPath();

            else if ( strcmp (command, "r") == 0)
                doResize();

            else if ( strcmp (command, "i") == 0)
                doInsert();

            else if ( strcmp (command, "d") == 0)
                doDelete();

            else if ( strcmp (command, "l") == 0)
                doList();

            else if ( strcmp (command, "f") == 0)
                doFile();

            else if ( strcmp (command, "#") == 0)
                std::cout << std::endl;

            else
                printf ("Command is not known: %s\n", command);

            input = fgets ( buffer, 300, inFile );   // get the next line of input

        }
    }

    static void showCommands() {
        printf ("The commands for this project are:\n");
        printf ("  q \n");
        printf ("  ? \n");
        printf ("  # \n");
        printf ("  t <int1> <int2> \n");
        printf ("  r <int> \n");
        printf ("  i <int1> <int2> \n");
        printf ("  d <int1> <int2> \n");
        printf ("  l \n");
        printf ("  f <filename> \n");
    }

    // get an integer value from the input
    static int getInt() {
        char* next = strtok (nullptr, " \r\n\t");
        printf("%s\n", next);
        if ( next == nullptr )
            return -1;
        int val = atoi (next );
        if ( val == 0 && strcmp (next, "0") != 0)
            return -1;
        return val;
    }

#define GET_INT(var) \
    int var = getInt(); \
    if (var < 0) { \
        printf ("Positive integer value expected\n\n"); \
        return; \
    }

#define CHECK(var) \
if (var > size) { \
    printf("Error: Island #%d is invalid.\n\n", var); \
    return; \
}

#define GET_AND_CHECK(var1, var2) \
    GET_INT(var1) GET_INT(var2) \
    CHECK(var1) CHECK(var2)


    void doShortestPath () {
        GET_AND_CHECK(val1, val2)
        printf ("Performing the Travel Command from %d to %d\n", val1, val2);
        auto path = bfs(val1, val2);

        auto first = path.begin();
        auto last = path.end();

        if (first != last) {
            std::cout << *first;
            while (++first != last) {
                std::cout << " -> " << *first;
            }
        }
        std::cout << std::endl << std::endl;
    }

    void doResize() {
        GET_INT(val)
        printf ("Performing the Resize Command with %d\n\n", val);
        reset(val);
    }

    void doInsert() {
        GET_AND_CHECK(val1, val2)
        if (val1 > size) {
            printf("Error: Island #%d is invalid.", val1);
        }
        printf ("Performing the Insert Command from %d to %d\n\n", val1, val2);
        islandsArray[val1-1].getReachableIslands().push_back(val2);
    }

    void doDelete() {
        GET_AND_CHECK(val1, val2)
        printf ("Performing the delete Command from %d to %d\n\n", val1, val2);
        islandsArray[val1-1].getReachableIslands().remove(val2);
    }

    void doList() {
        std::cout << "Islands  |  Reachable Islands\n";
        std::cout << "-----------------------------\n";
        for (size_t i = 0; i < size; ++i) {
            std::cout << std::setw(7) << i+1 << "  |  " << islandsArray[i].getReachableIslands() << std::endl;
        }
        std::cout << std::endl;
    }

    void doFile() {
        // get a filename from the input
        char* fname = strtok (nullptr, " \r\n\t");
        if ( fname == nullptr ) {
            printf ("Filename expected\n\n");
            return;
        }

        std::string filename = fname;
        // verify the file name is not currently in use
        if (filenames.contains(filename)) {
            fprintf(stderr, "File already open\n\n");
            return;
        }
        filenames.push_back(std::move(filename));

        //open the file using fopen creating a new instance of FILE*
        FILE *inFile;
        inFile = fopen(fname, "r");
        if (inFile == nullptr) {
            fprintf(stderr, "Error while opening the file.\n\n");
            return;
        }
        printf ("Performing the File command with file: %s\n\n", fname);
        //recursively call processCommandLoop() with this new instance of FILE* as the parameter
        processCommandLoop(inFile);

        //close the file when processCommandLoop() returns
        fclose(inFile);
    }
};

int main (int argc, char** argv)
{
    // set up the variable inFile to read from standard input
    FILE* inFile = stdin;

    // set up the data needed for the island adjcency list
    ArchipelagoExpedition islandData;

    // call the method that reads and parses the input
    ArchipelagoExpedition::showCommands();
    printf ("\nEnter commands at blank line\n");
    printf ("    (No prompts are given because of f command)\n");
    islandData.processCommandLoop (inFile);

    printf ("Goodbye\n");
    return 1;
}

#undef GET_INT
#undef GET_AND_CHECK
