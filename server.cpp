/*
    Muhammad Usman Shahid
        20i-1797
          CY-T
*/

#include <iostream>

#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

#define PORT 8080
#define MAXLINE 3048

void dealWithClient(int client_socket);
void dealWithTeacher(int client_socket);
void dealWithStudent(int client_socket);
void updatePercentages(void);
void updatePercentageInFile(string file);

void giveFiles(vector<string> &files);
bool lookintoFile(string fileName, const string rollNo, string &toSend);
bool teacherFound(string findThis);

void viewdataTeacher(string teacher, string &toSend);
void adddataTeacher(string teacher, string addThis);
bool deletedataTeacher(string teacher, string rollno, string sub);
bool updatedataTeacher(string teacher, string rollno, string sub, string newTotalNumber, string newNumber);

int main(void)
{

    cout << "\n\t\t ~~~~~~~~~~~~~~ Customized Grading Server ~~~~~~~~~~~~~~ \n"
         << endl;

    int server_fd, client_socket, valread;
    int opt = 1;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed : ");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        perror("setsockopt : ");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed : ");
        exit(EXIT_FAILURE);
    }

    // listen
    if (listen(server_fd, 5) < 0)
    {
        perror("listen failed : ");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // cout << "\n\n[+] -------------- waiting for clients -------------- [+]\n\n";

        // accept again and again
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address,
                                    (socklen_t *)&addrlen)) < 0)
        {
            perror("accept ");
            exit(EXIT_FAILURE);
        }

        // after accepted make the child to handle client
        if (!fork())
        {
            // closing main socket in child
            close(server_fd);
            dealWithClient(client_socket);
            updatePercentages();
            exit(EXIT_SUCCESS);
        }

        // closing the connected socket
        close(client_socket);
    }

    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);

    return 0;
}

// the main mplementations
void dealWithClient(int client_socket)
{
    char buffer[MAXLINE] = {0};

    recv(client_socket, buffer, sizeof(buffer), 0);
    cout << buffer << endl;

    const char *mainMenu = "\n\t\t1 for Teacher \n\t\t2 for student \n\t\t0 to quit \n\n\t\tEnter your option : ";

    send(client_socket, mainMenu, strlen(mainMenu), 0);

    // for the user choice
    char option = 0;

    do
    {
        recv(client_socket, &option, sizeof(option), 0);

        // switch for handling what user said
        switch (option)
        {
        case '0':

            cout << "\n\n[+] -------------- Client Leaved -------------- [+]\n\n";
            send(client_socket, "\n\n\t --------- Leaving :| --------- \n\n", strlen("\n\n\t --------- Leaving :| --------- \n\n"), 0);
            break;
        case '1':

            cout << "\n\n[+] -------------- New Teacher Arrived -------------- [+]\n\n";

            dealWithTeacher(client_socket);

            cout << "\n\n[+] --------------    Teacher Leaved   -------------- [+]\n\n";

            // work done now will quit this
            option = '0';
            break;
        case '2':
            cout << "\n\n[+] -------------- New Student Arrived -------------- [+]\n\n";

            dealWithStudent(client_socket);

            cout << "\n\n[+] --------------    Student Leaved   -------------- [+]\n\n";

            // work done now will quit this
            option = '0';
            break;
        default:
            cout << "\n\n[+] -------------- Wrong Option -------------- [+]\n\n";
            send(client_socket, "\n\n\t --------- Wrong option --------- \n\n", strlen("\n\n\t --------- Wrong option --------- \n\n"), 0);
        }

    } while (!(option == '0')); // if wrong option askgain
}

void dealWithTeacher(int client_socket)
{
    // sending to client
    send(client_socket, "\n\n\t --------- Welcome You as Teacher --- Verifying --------- \n\n\n\n\t Enter your first name : ", strlen("\n\n\t --------- Welcome You as Teacher --- Verifying --------- \n\n\n\n\t Enter your first name : "), 0);

    // reciving the name and crafting that
    char buff[MAXLINE];
    buff[recv(client_socket, buff, sizeof(buff), 0)] = '\0';

    string name = buff;
    name += "_";

    send(client_socket, "\n\t Enter your Last name : ", sizeof("\n\t Enter your Last name : "), 0);

    bzero(buff, sizeof(buff));
    buff[recv(client_socket, buff, sizeof(buff), 0)] = '\0';

    name += buff;

again:
    string searchThis = name + ".csv";

    if (teacherFound(searchThis))
    {
        const char *menuForTeacher = "\n\n\t\t1 for view your students info \n\t\t2 for add records\n\t\t3 for delete record\n\t\t4 for update record\n\t\tany other character to exit \n\n\t\tEnter your option : ";
        send(client_socket, menuForTeacher, strlen(menuForTeacher), 0);

        char newOpt = 0;
        recv(client_socket, &newOpt, sizeof(newOpt), 0);

        // different cases for teacher
        switch (newOpt)
        {
            // view the information
        case '1':
        {
            cout << "\n[+] ---- Teacher : " << name << " want to view data ---- [+]" << endl;
            string data;
            viewdataTeacher(searchThis, data);

            // sending the optput
            send(client_socket, data.c_str(), strlen(data.c_str()), 0);

            break;
        }
        case '2': // adding records
        {
            cout << "\n[+] ---- Teacher : " << name << " want to add data ---- [+]" << endl;

            string toAdd = "\n";
            string temp = "";

            // taking the data
            send(client_socket, "\n\t\tEnter the roll number : ", strlen("\n\t\tEnter the roll number : "), 0);

            bzero(buff, sizeof(buff));
            buff[recv(client_socket, buff, sizeof(buff), 0)] = '\0'; // roll number

            // making string to add
            temp = buff;
            toAdd += temp + ",";

            send(client_socket, "\n\t\tEnter the subject : ", strlen("\n\t\tEnter the subject : "), 0);

            bzero(buff, sizeof(buff));
            recv(client_socket, buff, sizeof(buff), 0); // subject

            temp = buff;
            toAdd += temp + ",";

            send(client_socket, "\n\t\tEnter the Total Numbers : ", strlen("\n\t\tEnter the Total Numbers : "), 0);

            bzero(buff, sizeof(buff));
            recv(client_socket, buff, sizeof(buff), 0); // total number

            temp = buff;
            toAdd += temp + ",";

            send(client_socket, "\n\t\tEnter the Obtained Numbers : ", strlen("\n\t\tEnter the Obtained Numbers : "), 0);

            bzero(buff, sizeof(buff));
            recv(client_socket, buff, sizeof(buff), 0); // obtained marks

            temp = buff;
            toAdd += temp + "," + "-";

            adddataTeacher(searchThis, toAdd);

            send(client_socket, "\n\n\t\t ~~~~~~~~ Data added ~~~~~~~~ \n\n", strlen("\n\n\t\t ~~~~~~~~ Data added ~~~~~~~~ \n\n"), 0);

            cout << "\n[+] ------ Teacher : " << name << " added the data ------ [+]" << endl;

            break;
        }
        case '3': // delete records
        {
            string rollNum = "";
            string subject = "";

            cout << "\n[+] ---- Teacher : " << name << " want to delete data ---- [+]" << endl;

            // taking the data
            send(client_socket, "\n\t\tEnter the roll number : ", strlen("\n\t\tEnter the roll number : "), 0);

            bzero(buff, sizeof(buff));
            buff[recv(client_socket, buff, sizeof(buff), 0)] = '\0'; // roll number

            rollNum = buff;

            send(client_socket, "\n\t\tEnter the subject : ", strlen("\n\t\tEnter the subject : "), 0);

            bzero(buff, sizeof(buff));
            recv(client_socket, buff, sizeof(buff), 0); // subject

            subject = buff;

            if (deletedataTeacher(searchThis, rollNum, subject))
            {
                send(client_socket, "\n\n\t\t ~~~~~~~~ Data Deleted ~~~~~~~~ \n\n", strlen("\n\n\t\t ~~~~~~~~ Data Deleted ~~~~~~~~ \n\n"), 0);
                cout << "\n[+] ------ Teacher : " << name << " deleted the data ----- [+]" << endl;
            }
            else
            {
                send(client_socket, "\n\n\t\t ~~~~~~~~ No Record Found ~~~~~~~~ \n\n", strlen("\n\n\t\t ~~~~~~~~ No Record Found ~~~~~~~~ \n\n"), 0);
                cout << "\n[+] ---- Teacher : " << name << " : No record found to update ---- [+]" << endl;
            }

            break;
        }
        case '4': // update records
        {
            cout << "\n[+] ---- Teacher : " << name << " want to update data ---- [+]" << endl;

            string rollNum = "";
            string subject = "";
            string newTotal = "";
            string newObtained = "";

            // taking the data
            send(client_socket, "\n\t\tEnter the roll number : ", strlen("\n\t\tEnter the roll number : "), 0);

            bzero(buff, sizeof(buff));
            buff[recv(client_socket, buff, sizeof(buff), 0)] = '\0'; // roll number

            rollNum = buff;

            send(client_socket, "\n\t\tEnter the subject : ", strlen("\n\t\tEnter the subject : "), 0);

            bzero(buff, sizeof(buff));
            recv(client_socket, buff, sizeof(buff), 0); // subject

            subject = buff;

            send(client_socket, "\n\t\tEnter the new total marks : ", strlen("\n\t\tEnter the new total marks : "), 0);

            bzero(buff, sizeof(buff));
            recv(client_socket, buff, sizeof(buff), 0); // subject

            newTotal = buff;

            send(client_socket, "\n\t\tEnter the new obtained marks : ", strlen("\n\t\tEnter the new obtained marks : "), 0);

            bzero(buff, sizeof(buff));
            recv(client_socket, buff, sizeof(buff), 0); // subject

            newObtained = buff;

            if (updatedataTeacher(searchThis, rollNum, subject, newTotal, newObtained))
            {
                send(client_socket, "\n\n\t\t ~~~~~~~~ Data Updated ~~~~~~~~ \n\n", strlen("\n\n\t\t ~~~~~~~~ Data Updated ~~~~~~~~ \n\n"), 0);
                cout << "\n[+] ---- Teacher : " << name << " updated data ---- [+]" << endl;
            }
            else
            {
                send(client_socket, "\n\n\t\t ~~~~~~~~ No Record Found ~~~~~~~~ \n\n", strlen("\n\n\t\t ~~~~~~~~ No Record Found ~~~~~~~~ \n\n"), 0);
                cout << "\n[+] ---- Teacher : " << name << " : No record found to update ---- [+]" << endl;
            }

            break;
        }
        default:
            cout << "\n[+] ------ Teacher : " << name << " want to exit ------ [+]" << endl;
        }

        bzero(buff, sizeof(buff));
        buff[recv(client_socket, buff, sizeof(buff), 0)] = '\0';

        // client don't want to exit
        if (newOpt == '1' || newOpt == '2' || newOpt == '3' || newOpt == '4')
            goto again; // go again no exit
    }
    else
    {
        const char *menuForTeacher = "\n\n\t\tNo data found woulld you want to create (y/n): ";
        send(client_socket, menuForTeacher, strlen(menuForTeacher), 0);

        char fileOpt = 0;

        recv(client_socket, &fileOpt, sizeof(fileOpt), 0);

        if (fileOpt == 'y')
        {
            string createFile = "./database/" + searchThis;

            cout << "\n[+] ----- file created for teacher " << name << " ----- [+]" << endl;

            fstream file;
            file.open(createFile, ios::out);

            if (file)
            {
                file << "Roll_no,Subject,Total Numbers,Number,Percentage\n";
                file.close();
            }
            else
            {
                perror("file ");
            }

            // again do all
            goto again;
        }
        else
        {
            cout << "\n[+] ----- No file for teacher ::: leaving ----- [+]" << endl;
            send(client_socket, "\n\n\t --------- Leaving :| --------- \n\n", strlen("\n\n\t --------- Leaving :| --------- \n\n"), 0);
        }
    }
}

void dealWithStudent(int client_socket)
{
    // sending to client -- and asking the roll number
    send(client_socket, "\n\n ----- Welcome You as Student --- Looking for your Information ----- \n\n\n\n\t Enter your roll no : ", strlen("\n\n\t --------- Welcome You as Student --- Looking for your Information --------- \n\n\n\n\t Enter your roll no : "), 0);

    // reciving the roll no
    char buff[MAXLINE];
    buff[recv(client_socket, buff, sizeof(buff), 0)] = '\0';

    vector<string> allFiles; // the all allFiles
    giveFiles(allFiles);     // getting the file name

    string toSend = "";

    stringstream temp;
    temp << "\n\n " << left << setw(15) << "  Teacher " << setw(11) << " Subject" << setw(15) << " Total Numbers " << setw(12) << " Number  " << setw(10) << "   Percentage "
         << "   "
         << setfill('-') << setw(70) << "\n"
         << endl;

    toSend += temp.str();

    bool found = false;

    // opening each file and lookinf if roll number exist will print the data
    for (auto files = allFiles.cbegin(); files != allFiles.cend(); files++)
    {
        string fileName = *files;

        // look into file
        if (lookintoFile(fileName, buff, toSend))
        {
            found = true;
        }
    }

    if (!found)
    {

        temp << left << setw(15) << "  - " << setw(13) << " - " << setw(15) << " - " << setw(12) << "   -  " << setw(10) << "  - "
             << "   "
             << setfill('-') << setw(70) << "\n"
             << endl;

        toSend = temp.str();
    }

    // sending if data found and - if not
    send(client_socket, toSend.c_str(), strlen(toSend.c_str()), 0);

    // all done from client side
    return;
}

void giveFiles(vector<string> &files)
{
    // getting all the files of the directory then wil check in each file
    DIR *databaseDir;

    dirent *dirRead; // reading from the dirctory

    if ((databaseDir = opendir("./database")))
    {
        // read file name
        while ((dirRead = readdir(databaseDir)))
        {
            if (dirRead->d_type == 0x8) // only files
                // using vector and storng the files name
                files.push_back(dirRead->d_name);
        }

        closedir(databaseDir);
    }
    else
    {
        perror("opendir : ");
        exit(EXIT_FAILURE);
    }
}

// for looking student info
bool lookintoFile(string fileName, const string rollNo, string &toSend)
{
    string line = "";
    string temp = "";
    bool found = false;

    string fileToOpen = "./database/" + fileName;

    // for putput
    stringstream output;

    fstream file;

    // opening the file
    file.open(fileToOpen, ios::in);

    if (file)
    {

        for (int i = 0; !file.eof(); i++)
        {
            line = "";
            getline(file, line);

            // braking and writing in the data
            stringstream breakLine(line);
            temp = "";

            // first get the roll number
            getline(breakLine, temp, ',');

            // if roll number matches then print the data
            if (temp == rollNo)
            {
                output.clear();

                // geting teacher name
                fileName.erase(fileName.length() - 4);
                fileName.replace(fileName.find('_'), 1, 1, ' ');

                output << left << setw(15) << fileName;

                // till all fields
                while (getline(breakLine, temp, ','))
                {
                    output << setw(15) << temp;
                }

                output << endl;

                toSend += output.str();

                found = true;
            }
        }

        file.close();
    }
    else
    {
        perror("\nfile error ");
    }

    return found; // no data
}

// checking the teacher exist or not
bool teacherFound(string findThis)
{
    vector<string> allFiles;
    giveFiles(allFiles);

    for (auto files : allFiles)
        if (files == findThis)
            return true;

    return false;
}

// to view by the teacher
void viewdataTeacher(string teacher, string &sendThis)
{
    string line = "";
    string temp = "";
    bool found = false;

    string fileToOpen = "./database/" + teacher;

    // for putput
    stringstream output;

    fstream file;

    // opening the file
    file.open(fileToOpen, ios::in);

    output << "\n\n";

    if (file)
    {

        for (int i = 0; !file.eof(); i++)
        {
            line = "";
            getline(file, line);

            // braking and writing in the data
            stringstream breakLine(line);
            temp = "";

            // first get the roll number
            // getline(breakLine, temp, ',');
            // // geting teacher name
            // teacher.erase(teacher.length() - 4);
            // teacher.replace(teacher.find('_'), 1, 1, ' ');

            // output << left << setw(15) << teacher;

            // till all fields
            while (getline(breakLine, temp, ','))
            {
                output << left << setw(15) << temp;
            }

            output << endl;

            output << setfill('-') << setw(73) << "\n"
                   << endl
                   << setfill(' ') << endl;
            sendThis = output.str();
        }

        file.close();
    }
    else
    {
        perror("\nfile error ");
    }

    return;
}

// adding the data
void adddataTeacher(string teacher, string addThis)
{

    string fileToOpen = "./database/" + teacher;

    fstream file;

    // opening the file
    file.open(fileToOpen, ios::out | ios::app);

    // adding to the file
    if (file)
    {
        file << addThis;
        file.close();
    }
    else
    {
        perror("\nfile error ");
    }
}

// delete
bool deletedataTeacher(string teacher, string rollno, string sub)
{
    bool found = false;

    string fileToOpen = "./database/" + teacher;

    fstream file;
    string line = "";
    string temp = "";
    string newData = "";

    // opening the file reading mode
    file.open(fileToOpen, ios::in);

    // adding to the file
    if (file)
    {
        for (int i = 0; !file.eof(); i++)
        {
            line = "";
            getline(file, line);

            // braking and writing in the data
            stringstream breakLine(line);
            temp = "";

            // first get the roll number
            getline(breakLine, temp, ',');

            // if roll number matches then print the data
            if (temp == rollno)
            {
                // first get the subiect
                getline(breakLine, temp, ',');

                if (temp == sub)
                {
                    found = true;
                    continue;
                }
                else
                {
                    // creating new data
                    newData += line;
                    newData += "\n";
                }
            }
            else
            {
                // creating new data
                newData += line;
                newData += "\n";
            }
        }

        file.close();
    }
    else
    {
        perror("\nfile error ");
    }

    newData.erase(newData.length() - 1);

    // writing to the file
    file.open(fileToOpen, ios::out | ios::trunc);

    if (file)
    {
        file << newData;
        file.close();
    }
    else
    {
        perror("\nfile error ");
    }

    return found;
}

// update
bool updatedataTeacher(string teacher, string rollno, string sub, string newTotalNumber, string newNumber)
{
    bool found = false;

    string fileToOpen = "./database/" + teacher;

    fstream file;
    string line = "";
    string temp = "";
    string newData = "";

    // opening the file reading mode
    file.open(fileToOpen, ios::in);

    // adding to the file
    if (file)
    {
        for (int i = 0; !file.eof(); i++)
        {
            line = "";
            getline(file, line);

            // braking and writing in the data
            stringstream breakLine(line);
            temp = "";

            // first get the roll number
            getline(breakLine, temp, ',');

            // if roll number matches then print the data
            if (temp == rollno)
            {
                // first get the subiect
                getline(breakLine, temp, ',');

                if (temp == sub)
                {
                    // will update the data
                    line = rollno + "," + sub + "," + newTotalNumber + "," + newNumber + "," + "-";
                    // creating new data
                    newData += line;
                    newData += "\n";
                    found = true;
                    continue;
                }
                else
                {
                    // creating new data
                    newData += line;
                    newData += "\n";
                }
            }
            else
            {
                // creating new data
                newData += line;
                newData += "\n";
            }
        }

        file.close();
    }
    else
    {
        perror("\nfile error ");
    }

    newData.erase(newData.length() - 1);

    // writing to the file
    file.open(fileToOpen, ios::out | ios::trunc);

    if (file)
    {
        file << newData;
        file.close();
    }
    else
    {
        perror("\nfile error ");
    }

    return found;
}

// to update the percentages
void updatePercentages(void)
{
    cout << "\n\n\t\t    [+] -------- Updating Percentages --------- [+]\n"
         << endl;

    vector<string> allFiles; // containg files
    giveFiles(allFiles);     // getting all files in the directory

    string toOpen = "";

    // going in each file and updating the percentage
    for (auto files : allFiles)
    {
        // to open the file --- the path setting
        toOpen = "./database/";
        toOpen += files;

        // update in the specific file
        updatePercentageInFile(toOpen);
    }

    cout << "\n\n\t\t    [+] --------- Percentage Updated ---------- [+]\n"
         << endl;

    return;
}

// updates the percent of each student
void updatePercentageInFile(string fileToOpen)
{
    cout << "\n[+] ----------- Updating percentages in file : " << fileToOpen << " --------- [+]" << endl;

    string updatedData = "";
    string line = "";
    string temp = "";

    double total = 0.0;
    double obtained = 0.0;

    // opening file and getting values and calculating them
    fstream file;

    file.open(fileToOpen, ios::in); // reading mode

    // checking oened or not
    if (file)
    {
        getline(file, line);
        updatedData += line;

        for (int i = 0; !file.eof(); i++)
        {

            line = "";
            getline(file, line);

            // braking the line
            stringstream breakLine(line);
            temp = "";

            // first get the roll number
            getline(breakLine, temp, ',');
            line = temp + ",";

            getline(breakLine, temp, ','); // subject
            line += temp + ",";

            getline(breakLine, temp, ','); // total number
            line += temp + ",";
            total = stod(temp);

            getline(breakLine, temp, ','); // obtained number
            line += temp + ",";
            obtained = stod(temp);

            // calculating the total
            total = (obtained / total) * 100;

            // for handling the floating points
            stringstream output;
            output << setprecision(4) << total;

            // adding to the line
            line += output.str() + "%";

            // updating the final string
            updatedData += "\n" + line;
        }

        file.close();
    }
    else
    {
        perror("file error ");
        exit(EXIT_FAILURE);
    }

    // now writing to the file

    file.open(fileToOpen, ios::out | ios::trunc); // writing mode

    // checking oened or not
    if (file)
    {
        file << updatedData;
        file.close();
    }
    else
    {
        perror("file error ");
        exit(EXIT_FAILURE);
    }

    return;
}
