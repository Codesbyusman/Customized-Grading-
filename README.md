# Customized-Grading-
TCP client server application .

A concurrent server that is used to handle students and teachers and perform actions.

In this, we designed the concurrent server having three main user categories. Student, Teacher, and Admin. Where Admin was working as a server and the student and teacher were a client. In this, we used the fork() to make the child processes, and whenever the client arrives the child process handles him leaving the server to receive another connection, thus making it a concurrent server.
If we talked about the flow, the admin of the server is always on. When it is on its starts listening and if a client receives then sends it to the child and deal accordingly. As soon, as connected to the server, the client is asked if they are a student or teacher. If the client is a student then take it roll number and search for it in the files if not then print nothing.
If we talked about teachers we take the first name and second name and join both and make a file if it does not exist if the client wants to make a file in the database. To hold the files we have a folder of the database. The file includes the roll number, subject, Total Number, obtained Number, and percentage. We have given the teacher five options:

1.	View Records: shows all the information stored.
2.	Add Records: Ask for the five fields and append them to the teacher file.
3.	Delete Records: Ask for the roll number and subject and all student record is there if a teacher teaches the two courses both entry will be in the same file thus for uniquely identifying using both and deleting if found the record. Moreover, writes new data to the file.
4.	Update records: Again, use the roll number and subject and updated the number by asking the client accordingly.
5.	Exit: simply exit the menu and disconnect from the server.

After a client is gone, the update Percentage function is responsible for the calculating percentage of the students in the file and writing against each student. Thus, the teacher can update the records and when dismissed from his/her activities the calculation of the percentage of the student is done accordingly.
