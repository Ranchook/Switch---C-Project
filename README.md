# Switch simulator---C-Project
Final project in Advanced C course. A realization of a 4-port double-sided switch.
Each port has information packets coming in with its info and destination ports and are supposed to be sent there.
Each out port has two queues, one for each packet priority.
The packets are routed in the switch by using a routing table build as a BST.

Each packet looks like this: Time  Da  Sa  Prio  Data_length  Data  Checksum.
Time: Packet arrival time, specified as unsigned int.
Da: A one byte number specifying the destination address.
Sa: A one byte number specifying the sender address.
Prio: A one byte number specifying the priority of the packet. 0 is high, 1 is low.
Data_Length: A one byte number specifying the length of the data.
Data: An array of data seperated with spcaes. Each element in the array contains an integer.
Checksum: A one byte number specifying the bit-wise XOR between all the bytes of data packets except the Time and Checksum fields.

Input instructions were given in a *.txt file.
Input files are *.in files, the packets in each file are sorted according to the Time field.

The purpose of this project was to showcase a proper use of memory allocation and the use of files,
working with structs, constructing a BST and managing it, also a proper work with pointers in functions.

Switch start state:
![start](https://user-images.githubusercontent.com/102590409/221225517-04a925e7-3466-49a7-b16c-7bf905e5ead2.jpg)


Switch end state:
![end](https://user-images.githubusercontent.com/102590409/221225589-6a79c551-716b-4ca5-8e96-3bca94e559b4.jpg)


A packet looks like this:
![switch packet](https://user-images.githubusercontent.com/102590409/221225609-056ad04d-3772-4777-b5a2-260203e88a99.jpg)


Bit-wise XOR done like this:
![XOR](https://user-images.githubusercontent.com/102590409/221225636-adc4e0e8-1744-4089-8704-3e9ffd796a5b.jpg)
