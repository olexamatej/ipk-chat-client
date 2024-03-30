#TODO



Proper received message writeup
Check if multiple clients on server work - Fix message receiving 
create another constructor for MSG, or find other way to check if received messages are correct - you made legalCheck, just dont exit on invalid
CHECK FOR SIGSEGV IN RECEIVED MESSAGES

add memory freeing on exit (mby its needed only in UDP)
change error to be printed to std::cerr

test TCP
TCP Packet receiver, add exit on error/bye
Check if multiple clients on server work - Fix message receiving 
program exits on using invalid chars but probs shouldnt, check it

MEELEY CHECKER FOR TCP

Add UDP message ID checker std::map ? works probs - probs doesnt work for auth and join packets
check if confirm messages are correct ID
UDP Errorpacket send
add sending BYE packet in udp
error when getting reply without sending join:3
fix rename
check if received messages are correct
wait for confirm after ctrl c 
make what happens if message is not confirmed at all

