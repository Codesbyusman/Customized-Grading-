
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

#define PORT 8080
#define MAXLINE 3048
int main()
{
    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("\nSocket Creation Error ");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if ((client_fd = connect(sock, (struct sockaddr *)&serv_addr,
                             sizeof(serv_addr))) < 0)
    {
        perror("\nConnection Failed ");
        exit(EXIT_FAILURE);
    }

    const char *greetings = "\n\n\t[+] --------- Client There --------- [+] \n\n";
    char buffer[MAXLINE] = {0};

    send(sock, greetings, strlen(greetings), 0);
    recv(sock, buffer, sizeof(buffer), 0);

    string mainmenu = buffer;

    cout << buffer;

    char opt;
    int a = 0;

    do
    {
        if (a == 0)
            cin >> opt;
        else
        {
            cout << mainmenu;
            cin >> opt;
        }

        a++;

        send(sock, &opt, sizeof(opt), 0);

        bzero(buffer, sizeof(buffer));
        valread = recv(sock, buffer, sizeof(buffer), 0);
        buffer[valread] = '\0';

        cout << buffer;

    } while (!strcmp(buffer, "\n\n\t --------- Wrong option --------- \n\n"));

    cin.ignore();
    // // for taking name from teacher and roll number from student
    char response[MAXLINE];
    cin.getline(response, MAXLINE);

    // sending the roll number or name accordingly
    send(sock, response, strlen(response), 0);

    bzero(buffer, sizeof(buffer));
    valread = recv(sock, buffer, sizeof(buffer), 0);
    buffer[valread] = '\0';

    cout << buffer;

    // dealing accordingly to the server replies
    if (opt == '1')
    {
        // teacher
        bzero(response, sizeof(response));
        cin.getline(response, MAXLINE); // last name

        // sending the last name accordingly
        send(sock, response, strlen(response), 0);

    again:
        bzero(buffer, sizeof(buffer));
        valread = recv(sock, buffer, sizeof(buffer), 0); // the menu
        buffer[valread] = '\0';

        if (!strcmp(buffer, "\n\n\t\tNo data found woulld you want to create (y/n): "))
        {
            char fileopt = 0;

            do
            {
                cout << buffer;
                cin >> fileopt; // file option
            } while (!(fileopt == 'y' || fileopt == 'n'));

            send(sock, &fileopt, sizeof(fileopt), 0);

            if (fileopt == 'y')
                goto again;
        }
        else
        {
            cout << buffer;

            char newOpt = 0;
            cin >> newOpt;

            send(sock, &newOpt, sizeof(newOpt), 0);

            // bzero(buffer, sizeof(buffer));
            // buffer[recv(sock, buffer, sizeof(buffer), 0)] = '\0';
            cin.ignore();

            if (newOpt == '1')
            {
                // view
            }
            else if (newOpt == '2') // add
            {

                for (int i = 0; i < 4; i++) // iterating for the 4 fields --- percentage will be calculated by the server
                {
                    bzero(buffer, sizeof(buffer));
                    recv(sock, buffer, sizeof(buffer), 0);

                    cout << buffer;

                    char input[MAXLINE] = {'\0'};
                    cin.getline(input, MAXLINE);

                    send(sock, input, sizeof(input), 0); // sending accordingly
                }

                bzero(buffer, sizeof(buffer));
                recv(sock, buffer, sizeof(buffer), 0);

                cout << buffer;
            }
            else if (newOpt == '3')
            {

                for (int i = 0; i < 2; i++) // iterating for the ]]2
                {
                    bzero(buffer, sizeof(buffer));
                    recv(sock, buffer, sizeof(buffer), 0);

                    cout << buffer;

                    char input[MAXLINE] = {'\0'};
                    cin.getline(input, MAXLINE);

                    send(sock, input, sizeof(input), 0); // sending accordingly
                }

                bzero(buffer, sizeof(buffer));
                recv(sock, buffer, sizeof(buffer), 0);

                cout << buffer;
            }
            else if (newOpt == '4')
            {
                for (int i = 0; i < 4; i++) // iterating for the
                {
                    bzero(buffer, sizeof(buffer));
                    recv(sock, buffer, sizeof(buffer), 0);

                    cout << buffer;

                    char input[MAXLINE] = {'\0'};
                    cin.getline(input, MAXLINE);

                    send(sock, input, sizeof(input), 0); // sending accordingly
                }

                bzero(buffer, sizeof(buffer));
                recv(sock, buffer, sizeof(buffer), 0);

                cout << buffer;
            }

            send(sock, "again", strlen("again"), 0);

            // client don't want to exit
            if (newOpt == '1' || newOpt == '2' || newOpt == '3' || newOpt == '4')
                goto again;
        }
    }

    cout << endl;

    // closing the connected socket
    close(client_fd);
    return 0;
}
